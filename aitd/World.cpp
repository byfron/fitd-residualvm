#include "World.hpp"
#include <graphics/RenderSystem.hpp>

void World::load() {
	//TODO  Floor:Ptr floor_data = FloorFactory::create(0);
	Floor::Ptr floor_data = Floor::Ptr(new Floor());
	floor_data->load(0);

	//Create camera entities with background and camera parameters
	//TODO: keep track of entities/data ids
	RoomCamera::Ptr room_cam = floor_data->getCamera(0);
	Entity camera = entity_manager->createLocal();
	entity_manager->assign<CameraComponent>(camera.id());
	entity_manager->assign<BgImageComponent>(camera.id(), room_cam->getBackgroundImagePtr());
	current_camera_id = camera.id();
	
}

void World::render(float dt) {

	auto camera = entity_manager->getComponentPtr<CameraComponent>(current_camera_id);
	auto bg_image = entity_manager->getComponentPtr<BgImageComponent>(current_camera_id);

	bg_image->render(dt);	
	
}
