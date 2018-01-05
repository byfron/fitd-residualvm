#include "World.hpp"
#include <graphics/RenderSystem.hpp>

void World::load() {
	//TODO  Floor:Ptr floor_data = FloorFactory::create(0);
	Floor::Ptr floor_data = Floor::Ptr(new Floor());
	floor_data->load(0);

	int room_id = 0;
	
	//Create camera entities with background and camera parameters
	//TODO: keep track of entities/data ids
	RoomCamera::Ptr room_cam = floor_data->getCamera(0);
	Entity camera = entity_manager->createLocal();
	entity_manager->assign<CameraComponent>(camera.id(), room_cam->transform);
	entity_manager->assign<BgImageComponent>(camera.id(), room_cam->getBackgroundImagePtr());
	current_camera_id = camera.id();

	// Display debug meshes with collison stuff
	for (auto &box : floor_data->getRoom(room_id)->colision_vector) {
		//Add debug objects for each colision vector
		Entity debug_obj = entity_manager->createLocal();
		entity_manager->assign<DebugComponent>(
			debug_obj.id(),
			Geometry::DebugMesh::Ptr(new Geometry::DebugBox(box->p1.cast<float>(),
															box->p2.cast<float>()))
			);	
	}

	// Display location of camera
	for (auto &cam_idx : floor_data->getRoom(room_id)->camera_indices) {
		Room::Ptr room = floor_data->getRoom(room_id);		
		RoomCamera::Ptr cam = floor_data->getCamera(cam_idx);
		Vec3f cam_pos = Vec3f(cam->position(0), cam->position(1), cam->position(2));		
		Entity debug_cam = entity_manager->createLocal();		
		entity_manager->assign<DebugComponent>(
			debug_cam.id(),
			Geometry::DebugMesh::Ptr(new Geometry::DebugSphere(cam_pos, 0.15f))
			);

		// Display also a vector of direction		
		Entity debug_cam2 = entity_manager->createLocal();
		Vec3f dir = //cam->transform.topLeftCorner(3,3).cast<float>() * Vec3f(0.0, 1.0, 0.0);
			cam->look_at;
		entity_manager->assign<DebugComponent>(
		debug_cam2.id(),
		Geometry::DebugMesh::Ptr(new Geometry::DebugLine(cam_pos,
														 cam_pos + dir)));
	}

	

}

void World::render(float dt) {

	auto camera = entity_manager->getComponentPtr<CameraComponent>(current_camera_id);
	auto bg_image = entity_manager->getComponentPtr<BgImageComponent>(current_camera_id);

	camera->render(dt);
	bg_image->render(dt);
	
}
