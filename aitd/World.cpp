#include "World.hpp"
#include "Components.hpp"
#include "ActorLoader.hpp"
#include "AITDEngine.hpp"
#include <graphics/RenderSystem.hpp>

using namespace Components;

void World::load() {

	loadFloor(0);
}

//TODO: shall we preload the floors??
void World::loadFloor(int floor_id) {

	//remove entities in previous floor?
	
	//load floor
	Floor::Ptr floor_data = Floor::Ptr(new Floor());
	floor_data->load(floor_id);

	current_floor_id = floor_id;
	current_room_id = 0;
	int camera_index = 0;

	//create entities in this floor
	//Cameras ==================================================================
	std::vector<int> camera_indices = floor_data->getRoom(current_room_id)->camera_indices;
	assert(camera_index < camera_indices.size());
	RoomCamera::Ptr room_cam = floor_data->getCamera(camera_indices[camera_index]);

	//TODO: refactor in createCameraEntity()
	//NOTE: The camera coordinates are expressed wrt the room (We can consider that is "attached" to
	//the room node. We temporarily alter the location but in practice we should have a proper
	// scene-tree)
	Vec3f room_world = floor_data->getRoom(current_room_id)->world_pos.cast<float>();
	Vec3f cam_pos = room_cam->transform.col(3).head(3)/10;
	cam_pos(0) = (cam_pos(0) - room_world(0));
	cam_pos(1) = (room_world(1) - cam_pos(1));
	cam_pos(2) = (room_world(2) - cam_pos(2));
 	room_cam->transform.col(3).head(3) = cam_pos * 10;
	////////////////////////////////////////////////////////////////////////////////////////////////
		
	Entity camera = entity_manager->createLocal();	
	// view matrix is the inverse of the camera trasnformation matrix;
	entity_manager->assign<CameraComponent>(camera.id(), room_cam->projection,
											room_cam->transform.inverse()); 
	entity_manager->assign<BgImageComponent>(camera.id(), room_cam->getBackgroundImagePtr());
	current_camera_id = camera.id();

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
			
			//Add debug objects for each colision vector
			Entity debug_obj = entity_manager->createLocal();
			entity_manager->assign<DebugComponent>(
				debug_obj.id(),				
				Geometry::DebugMesh::Ptr(new Geometry::DebugBox(p1.cast<float>(),
																p2.cast<float>()))
				);
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

void World::createObjectEntities(const ObjectData& object) {

	//Create all entities at this floor
	Entity object_entity = entity_manager->createLocal();

	// if track mode is user input, this is the player entity
	// TODO: we should find a better way to figure this out?
	if (object.trackMode == 1) {
		entity_manager->assign<UserInputComponent>(object_entity.id());
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
												  object.life_mode);
	}
	
	if (object.body != -1) {	
	
		//check if actor is created (create it otherwise)?
		Actor::Ptr actor_data = ActorLoader::load(object.body);		
		entity_manager->assign<MeshComponent>(object_entity.id(), actor_data);
		
		// is animated? TODO: We should include all animations (deep copies!)
		if (object.anim >= 0) {
			if (object.body == 12) {
				Animation::Ptr anim_data = ActorLoader::loadAnimation(actor_data->skeleton, 254);
				entity_manager->assign<Components::AnimationComponent>(object_entity.id(), anim_data);
			}
		}
	}	
}

void World::render(float dt) {

	auto camera = entity_manager->getComponentPtr<CameraComponent>(current_camera_id);
	auto bg_image = entity_manager->getComponentPtr<BgImageComponent>(current_camera_id);

	camera->render(dt);
	bg_image->render(dt);
	
}
