#include "World.hpp"
#include "Components.hpp"
#include "ActorLoader.hpp"
#include "AITDEngine.hpp"
#include <graphics/RenderSystem.hpp>

using namespace Components;


void World::load() {

	loadFloor(0);
}


Entity World::createCameraEntity(Floor::Ptr floor_data, int room_index, int camera_index) {

	RoomCamera::Ptr room_cam = floor_data->getCamera(camera_index);
	Vec3f room_world = floor_data->getRoom(room_index)->world_pos.cast<float>();
	Vec3f cam_pos = room_cam->transform.col(3).head(3)/10;
	cam_pos(0) = (cam_pos(0) - room_world(0));
	cam_pos(1) = (room_world(1) - cam_pos(1));
	cam_pos(2) = (room_world(2) - cam_pos(2));
 	cam_pos *= 10;
	
	Entity camera = entity_manager->createLocal();
	Eigen::Matrix4f transform = room_cam->transform;
	transform.col(3).head(3) = cam_pos;
	
	// view matrix is the inverse of the camera trasnformation matrix;
	entity_manager->assign<CameraComponent>(camera.id(), room_cam->projection,
											transform.inverse()); 
	entity_manager->assign<BgImageComponent>(camera.id(), room_cam->getBackgroundImagePtr());

	std::vector<Geometry::Polygon<Vec2f> > polygons;
	for (auto z : room_cam->zone_vector) {
		for (auto e : z->entry_vector) {
			polygons.push_back(Geometry::Polygon<Vec2f>(e->points));
		}
	}
	entity_manager->assign<CameraZoneComponent>(camera.id(), polygons);

	return camera;
}


//TODO: shall we preload the floors??
void World::loadFloor(int floor_id) {

	//remove entities in previous floor?
	
	//load floor
	Floor::Ptr floor_data = Floor::Ptr(new Floor());
	floor_data->load(floor_id);

	current_floor_id = floor_id;
	current_room_id = 0;
	int current_camera_index = 0; //tmp

	//create entities in this floor
	//Cameras ==================================================================
	std::vector<int> camera_indices = floor_data->getRoom(current_room_id)->camera_indices;
	for (int camera_index : camera_indices) {
		Entity camera = createCameraEntity(floor_data, current_room_id, camera_index);
		AITDEngine::CameraEntityToIndex[camera.id().id] = camera_index;
	}

	for (auto cam : AITDEngine::CameraEntityToIndex) {
		if (cam.second == current_camera_index) {
			current_camera_id.id = cam.first;
		}
	}

	//maybe instead keep a map id/Entity
	//Objects ==================================================================	
	for (auto object_it : ObjectManager::object_map) {
		if (object_it.second.stage == floor_id) {
			if (object_it.second.room == current_room_id) //TODO: all rooms seen by the camera!!
			{
				createObjectEntities(object_it.second);
			}
		}
	}

	// DEBUG STUFF =============================================================
	// Display debug meshes with collison stuff
	// Collision meshes are stored as world coordinates for each floor
	Room::Ptr room = floor_data->getRoom(current_room_id);
	{
		for (auto& box : room->colision_vector) {

			Vector3i p1 = box->p1;
			Vector3i p2 = box->p2;
			p1(1) = -p1(1);
			p2(1) = -p2(1); //TODO: revert the Y coordinate at loading time

			//Add collision entities for the scene
			Entity scene_entity = entity_manager->createLocal();
			entity_manager->assign<SceneCollisionComponent>(
				scene_entity.id(),
				Geometry::BBox(Vec3f(p1(0), p2(1), p1(2)),
							   Vec3f(p2(0), p1(1), p2(2))));
				
			// //Add debug objects for each colision vector
			// Entity debug_obj = entity_manager->createLocal();
			// entity_manager->assign<DebugComponent>(
			// 	debug_obj.id(),				
			// 	Geometry::DebugMesh::Ptr(new Geometry::DebugBox(p1.cast<float>(),
			// 													p2.cast<float>()))
			// 	);
		}
	}

	// Display location of camera
	for (auto &cam_idx : floor_data->getRoom(current_room_id)->camera_indices) {
		Room::Ptr room = floor_data->getRoom(current_room_id);		
		RoomCamera::Ptr cam = floor_data->getCamera(cam_idx);
		Vec3f cam_pos = Vec3f(cam->position(0), cam->position(1), cam->position(2));		

		// Display also a vector of direction		
		Entity debug_cam2 = entity_manager->createLocal();
		entity_manager->assign<DebugComponent>(
		debug_cam2.id(),
		Geometry::DebugMesh::Ptr(new Geometry::DebugAxis(cam->transform)));
	}
}

void World::switchToCamera(Entity::Id camera_id) {
	current_camera_id = camera_id;
}

void World::createObjectEntities(const ObjectData& object) {


	//how do we know which one is the main character??

	//TODO
	//keep a global map that relates objects to entities????
	//not sure we needed but the legacy code uses this
	
	//Create all entities at this floor
	Entity object_entity = entity_manager->createLocal();

	// if track mode is user input, this is the player entity
	// TODO: we should find a better way to figure this out?
	if (object.track_mode == 1) {
		entity_manager->assign<UserInputComponent>(object_entity.id());
		AITDEngine::player_entity_id  = object_entity.id();
	}

	// TODO give objects a debug name that makes it easier to check
	// to figure out properties
	
	// can move?
	entity_manager->assign<MoveComponent>(object_entity.id(), 1000.0, 2.0);
	
	// is displayed in screen?
	entity_manager->assign<TransformComponent>(
		object_entity.id(),
		object.x,
	   -object.y,
		object.z,
		getRotationMatrixFromRotIndices(object.alpha,
										object.beta,
										object.gamma)
		);

	if (object.life != -1) {
		// is scripted?
		entity_manager->assign<ScriptComponent>(object_entity.id(), object.life);

		// metadata used by the scripting/logic
		// I put it temporarily in the component but we should probably split it		
		entity_manager->assign<MetaDataComponent>(object_entity.id(),
												  object.flags,
												  object.body,
												  object.life,
												  object.life_mode,
												  object.track_mode);
	}
	
	if (object.body != -1) {		
	
		//check if actor is created (create it otherwise)?
		Actor::Ptr actor_data = ActorLoader::load(object.body);		
		entity_manager->assign<MeshComponent>(object_entity.id(), actor_data);
		
		// is animated? TODO: We should include all animations (deep copies!)
		if (object.anim >= 0) {
			if (object.body == 12) {
				Animation::Ptr anim_data = ActorLoader::loadAnimation(actor_data->skeleton, 254);
				entity_manager->assign<Components::AnimationComponent>(object_entity.id(), object.anim, anim_data);
			}
		}

		// can collide?
		// get corresponding actor bounding box
		Geometry::BBox box = actor_data->bounding_box;
		box.translation = Vec3f(object.x, -object.y, object.z);
		entity_manager->assign<ActorCollisionComponent>(object_entity.id(), box);
		
	}	
}

void World::render(float dt) {

	auto camera = entity_manager->getComponentPtr<CameraComponent>(current_camera_id);
	auto bg_image = entity_manager->getComponentPtr<BgImageComponent>(current_camera_id);

	camera->render(dt);
	bg_image->render(dt);
	
}
