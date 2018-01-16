#include "ScriptingManager.hpp"
#include "AITDEngine.hpp"
#include "ObjectManager.hpp"
#include "Components.hpp"
#include <hqr.h>

using namespace Components;

CommandId CommandMacroTable[] = {
	LM_DO_MOVE,
	LM_ANIM_ONCE,
	LM_ANIM_ALL_ONCE,
	LM_BODY,
	LM_IF_EGAL,
	LM_IF_DIFFERENT,
	LM_IF_SUP_EGAL,
	LM_IF_SUP,
	LM_IF_INF_EGAL,
	LM_IF_INF,
	LM_GOTO,
	LM_RETURN,
	LM_END,
	LM_ANIM_REPEAT,
	LM_ANIM_MOVE,
	LM_MOVE,
	LM_HIT,
	LM_MESSAGE,
	LM_MESSAGE_VALUE,
	LM_VAR,
	LM_INC,
	LM_DEC,
	LM_ADD,
	LM_SUB,
	LM_LIFE_MODE,
	LM_SWITCH,
	LM_CASE,
	LM_CAMERA,
	LM_START_CHRONO,
	LM_MULTI_CASE,
	LM_FOUND,
	LM_LIFE,
	LM_DELETE,
	LM_TAKE,
	LM_IN_HAND,
	LM_READ,
	LM_ANIM_SAMPLE,
	LM_SPECIAL,
	LM_DO_REAL_ZV,
	LM_SAMPLE,
	LM_TYPE,
	LM_GAME_OVER,
	LM_MANUAL_ROT,
	LM_RND_FREQ,
	LM_MUSIC,
	LM_SET_BETA,
	LM_DO_ROT_ZV,
	LM_STAGE,
	LM_FOUND_NAME,
	LM_FOUND_FLAG,
	LM_FOUND_LIFE,
	LM_CAMERA_TARGET,
	LM_DROP,
	LM_FIRE,
	LM_TEST_COL,
	LM_FOUND_BODY,
	LM_SET_ALPHA,
	LM_STOP_BETA,
	LM_DO_MAX_ZV,
	LM_PUT,
	LM_C_VAR,
	LM_DO_NORMAL_ZV,
	LM_DO_CARRE_ZV,
	LM_SAMPLE_THEN,
	LM_LIGHT,
	LM_SHAKING,
	LM_INVENTORY,
	LM_FOUND_WEIGHT,
	LM_UP_COOR_Y,
	LM_SPEED,
	LM_PUT_AT,
	LM_DEF_ZV,
	LM_HIT_OBJECT,
	LM_GET_HARD_CLIP,
	LM_ANGLE,
	LM_REP_SAMPLE,
	LM_THROW,
	LM_WATER,
	LM_PICTURE,
	LM_STOP_SAMPLE,
	LM_NEXT_MUSIC,
	LM_FADE_MUSIC,
	LM_STOP_HIT_OBJECT,
	LM_COPY_ANGLE,
	LM_END_SEQUENCE,
	LM_SAMPLE_THEN_REPEAT,
	LM_WAIT_GAME_OVER		
};

ScriptingManager::ScriptingManager(EntityManager::Ptr em, World::Ptr w) :
	entity_manager(em),
	world(w)
{
	listLife = new Fitd::hqrEntryStruct("LISTLIFE", 10000, 100);
}

// void ScriptingManager::loadScriptsFromObjects(const std::map<int, ObjectData> &object_map) {

// 	//pre-load all script indices
// 	std::vector<int16> life_ids;
// 	for (auto& obj : object_map) {
// 		if (obj.second.life != -1) {
// 			life_ids.push_back(obj.second.life);
// 		}
// 	}

// 	//remove duplicates and load
// 	std::unique(life_ids.begin(), life_ids.end());	
// 	for (auto idx : life_ids) {
// 		std::cout << "loading " << idx << std::endl;
// 		char *life_ptr = listLife->get(idx);
// 	 	script_map[idx] = loadScript(life_ptr);
// 	}
// }

int16 getArg(char **life_ptr) {
	int16 arg = **life_ptr;
	*life_ptr += 2;
	return arg;
}

#define ARG getArg(life_ptr)

/* Access to attributes of the current processed actor*/
int ScriptingManager::evalVar(char** life_ptr, Entity::Id entity_id) {

	//CMD VAR1 VAR2
	//IF(VAR1 ==-1) ret VAR2
	//IF(VAR1 == 0) ret globals[VAR2]

	std::cout << "RUN EVAL: ";
		
	int16 var1 = getArg(life_ptr); 

	std::cout << "var1=" << var1 << " ";
		
	if (var1 == -1) {
		int16 var2 = getArg(life_ptr);
		std::cout << "|" << var2;
		return var2;
	}
	else if (var1 == 0) {
		int16 var2 = getArg(life_ptr);
		std::cout << "| vars[" << var2 << "] = " << AITDEngine::globals[var2];
		return AITDEngine::globals[var2];
	}
	else {

		std::cout << "| reg-";

		//if 5th most significant bit is active. (is gonna query about another actor?). 
		if(var1 & 0x8000) { 
			int16 object_id = getArg(life_ptr);
			int actor_id = ObjectManager::object_map[object_id].ownerIdx; // look for corresponding actor
			
			if(actor_id == -1) { //no owner of the object (actor not in room???)
				switch(var1 & 0x7FFFF) { //check the rest of bits
				case 0x1F: {
					return 0;//getObjectRoomIndex(object_id);
					break;
				}
				case 0x26: {
					return 0;//getObjectStageIndex(object_id);
					break;
				}
				default: {
					//	log_error("Unsupported code %X when actor not in room\n", var1 & 0x7FFF);
				}
				}
									
			}
		}

		var1 &= 0x7FFF;
		var1--;

		std::cout << "evalVar switch: " << var1 << std::endl;
		
		switch(var1) {			
		case 0x0: { //return colliding actor (-1) otherwise			
			CollisionComponent* cc = entity_manager->getComponentPtr<CollisionComponent>(entity_id);
			if (cc->isCollidingWithActor()) {
				return cc->getCollidingActorIdx();
			}
			return -1;
			break;
		}
		case 0x1: { //return HARD_DEC field (no idea what this does)
			//TODO
			break;
		}
		case 0x2: { //return HARD_COL field (hard collision - with environment?)
			CollisionComponent* cc = entity_manager->getComponentPtr<CollisionComponent>(entity_id);
			if (cc->isCollidingWithScene()) {
				return cc->getCollidingWithSceneIdx();
			}
			return -1;
			break;
		}
		case 0x3: { //return HIT field (index in actor table of actor being hit)
			CollisionComponent* cc = entity_manager->getComponentPtr<CollisionComponent>(entity_id);
			if (cc->isHittingActor()) {
				return cc->getHittingActorIdx(); //Entity or idx?
			}
			return -1;
			break;
		}
		case 0x4: { // HIT_BY field (actor that is hitting "us")

		}
		case 0x5: { // ANIM field (animation index?)
			AnimationComponent* ac = entity_manager->getComponentPtr<AnimationComponent>(entity_id);
			return ac->anim_id;
		}
		case 0x6: { // END_ANIM field. Signals animation has ended
			
		}
		case 0x7: { // FRAME field. key-frame index
			
		}	
		case 0x8: { // FRAME_END field. signals key frame change
			
		}
		case 0x9: { // BODY_NUM. Body index of current actor (from listBody buffer)
			MetaDataComponent* mdc = entity_manager->getComponentPtr<MetaDataComponent>(entity_id);
			std::cout << "BodyNum" << mdc->body_num << "|";
			return mdc->body_num;
		}
		case 0xA: { // MARK. Current mark in the track (track control point?)

		}
		case 0xB: { // TRACK: id of track
			
		}
		case 0xC: { // CHRONO: 

		}
		case 0xD: { // ROOM CHRONO: 
					
		}
		case 0xE: { // Distance between current and object-actor

			int16 object_idx = getArg(life_ptr);
//			int actor_idx = objectTable[object_idx].ownerIdx;
			//if(actor_idx == -1) {
			//	return(32000);
			//}
			//else {
			//	
			//}
		}
		case 0xF: { // COL_BY actor collided by
			return -1;
		}
		case 0x10: { // Found
			//	return objectTable[evalVar(life_ptr)].flags2 & 0x8000;
			break;
		}
		case 0x11: { // User action
			
		}
		case 0x12: { // POSREL: get relative pos between two actors
			int16 object_idx = getArg(life_ptr);

		}
		case 0x13: { // User input

		}
		case 0x14: { // User button
			std::cout << "BUTTON|";
			UserInputComponent* uic = entity_manager->getComponentPtr<UserInputComponent>(entity_id);
			return 0;
		}
		case 0x15: { //COL_BY again?
//			CollisionComponent* mdc = entity_manager->getComponentPtr<CollisionComponent>(entity_id);
			return -1;
		}
		case 0x16: { // actor alpha
		}
		case 0x17: { // actor beta			
		}
		case 0x18: { // actor gamma

		}
		case 0x19: { // IN_HAND object?
//			return(inHand);
			break;
		}
		case 0x1B: { // current camera????
//			return(*(uint16 *)(((currentCamera + 6) * 2) + cameraPtr));
			break;
		}
		case 0x1C: { // random number in range
			int16 range = getArg(life_ptr);
//			return randRange(0, range);
		}
		case 0x1D: { // falling flag

		}
		case 0x1E: { // room id

		}
		case 0x1F: { // life

		}
		case 0x20: { //query two first bits of flags
			int16 object_id = getArg(life_ptr); 
			if (ObjectManager::object_map[object_id].flags & 0xC000) {
				return 1;
			}
			else {
				return 0;
			}
		}
		case 0x21: { // room Y component
			
		}
		case 0x22: { // TEST_ZV_END_ANIM. Tests for location updates after anim
			int16 anim_id = getArg(life_ptr);
			int16 vertical_displacement = getArg(life_ptr);
			
		}
		case 0x23: { // TODO: music
			return(1);
			break;
		}
		case 0x24: { // Returns value of global variable
			int16 g_var = getArg(life_ptr);
			return AITDEngine::C_globals[g_var];
			break;
		}
		case 0x25: { // etage
//			MetaDataComponent* mdc = entity_manager->getComponentPtr<MetaDataComponent>(entity_id);
//			return mdc->etage;
		}
		case 0x26: { // THROW
			int16 object_id = getArg(life_ptr);
			// check flags
		}
		default: {
			//	error("Unhandled test type %X in evalVar\n", var1);
			break;
		}
		}
	}
}


void ScriptingManager::loadObjectCommand(CommandId command, int object_idx, char** life_ptr) {

	std::cout << "switch object" <<  command << std::endl;

	
	switch(command) {
	case LM_BODY: {//body = var
		ObjectManager::object_map[object_idx].body = getArg(life_ptr);
		break;
	}
	case LM_BODY_RESET: {//body = var, anim = var
		ObjectManager::object_map[object_idx].body = getArg(life_ptr);
		ObjectManager::object_map[object_idx].anim = getArg(life_ptr);
		break;
	}
	case LM_TYPE: {//flags = flags & (data1 & TYPE_MASK)
		int flags = getArg(life_ptr) & TYPE_MASK;
		ObjectManager::object_map[object_idx].flags =
			(ObjectManager::object_map[object_idx].flags & (~TYPE_MASK)) + flags;
		break;
	}
	case LM_ANIM_ONCE: { //anim = data1, animInfo = data2, animType = ANIM_ONCE
		ObjectManager::object_map[object_idx].anim = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animInfo = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animType = ANIM_ONCE;

		break;
	}
	case LM_ANIM_REPEAT: {
		ObjectManager::object_map[object_idx].anim = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animInfo = -1;
		ObjectManager::object_map[object_idx].animType = ANIM_REPEAT;
		break;
	}
	case LM_ANIM_ALL_ONCE: {
		ObjectManager::object_map[object_idx].anim = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animInfo = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animType = ANIM_ONCE | ANIM_UNINTERRUPTABLE;
		break;
	}
	case LM_ANIM_RESET: {
		ObjectManager::object_map[object_idx].anim = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animInfo = getArg(life_ptr);
		ObjectManager::object_map[object_idx].animType = ANIM_ONCE | ANIM_RESET;
		break;
	}
	case LM_MOVE: {
		//etage, room
		ObjectManager::object_map[object_idx].stage = getArg(life_ptr);
		ObjectManager::object_map[object_idx].room = getArg(life_ptr);
		ObjectManager::object_map[object_idx].life_mode = 0;
		break;
	}
	case LM_ANGLE: {
		ObjectManager::object_map[object_idx].alpha = getArg(life_ptr);
		ObjectManager::object_map[object_idx].beta = getArg(life_ptr);
		ObjectManager::object_map[object_idx].gamma = getArg(life_ptr);
		break;
	}
	case LM_STAGE: {
		//stage, room, x, y, z
		ObjectManager::object_map[object_idx].stage = getArg(life_ptr);
		ObjectManager::object_map[object_idx].room = getArg(life_ptr);
		ObjectManager::object_map[object_idx].x = getArg(life_ptr);
		ObjectManager::object_map[object_idx].y = getArg(life_ptr);
		ObjectManager::object_map[object_idx].z = getArg(life_ptr);
		break;
	}
	case LM_TEST_COL: {
		int test = getArg(life_ptr);
		if(test) {
			ObjectManager::object_map[object_idx].flags |= 0x20;
		}
		else {
			ObjectManager::object_map[object_idx].flags &= 0xFFDF;
		}
		break;
	}
	case LM_LIFE: {
		ObjectManager::object_map[object_idx].life = getArg(life_ptr);
		break;
	}
	case LM_LIFE_MODE: {
		ObjectManager::object_map[object_idx].life_mode = getArg(life_ptr);
		break;
	}
	case LM_FOUND_NAME: {
		ObjectManager::object_map[object_idx].found_name = getArg(life_ptr);
		break;
	}
	case LM_FOUND_BODY: {
		ObjectManager::object_map[object_idx].found_body = getArg(life_ptr);
		break;
	}
	case LM_FOUND_FLAG: {
		ObjectManager::object_map[object_idx].flags &= 0xE000;
		ObjectManager::object_map[object_idx].flags |= getArg(life_ptr);
		break;
	}
	case LM_FOUND_WEIGHT: {
		ObjectManager::object_map[object_idx].pos_in_track =  getArg(life_ptr);
		break;
	}
	case LM_START_CHRONO:
		break;
	default:
		std::cout << "(Object) Unsupported opcode" << std::endl;
		getchar();
		break;	   
	}
}

void ScriptingManager::loadActionCommand(CommandId command, Entity::Id entity_id, char** life_ptr) {

	std::cout << "switch action" <<  command << std::endl;
	
	Components::MetaDataComponent *mdc =
		entity_manager->getComponentPtr<Components::MetaDataComponent>(entity_id);

	static int switch_val;
	
	switch(command) {
	case LM_BODY: { //body = var

		int new_object_id = evalVar(life_ptr, entity_id);
		if (mdc->body_num != new_object_id) {
			mdc->body_num = new_object_id;

			//assign new mesh/body/animation in the current frame
			
		}								
		break;
	}	
	case LM_BODY_RESET: { //body = var, anim = var

		//assign new mesh/body/animation in the frame zero
		int new_object_id = evalVar(life_ptr, entity_id);
		int new_anim_id = evalVar(life_ptr, entity_id);
		

		break;
	}
	case LM_DO_REAL_ZV: { // defines actor bounding box from mesh points?

		break;
	}
	case LM_DEF_ZV: { // loads actor bounding box from data (in global coords)

		int x1 = getArg(life_ptr);
		int x2 = getArg(life_ptr);
		int y1 = getArg(life_ptr);
		int y2 = getArg(life_ptr);
		int z1 = getArg(life_ptr);
		int z2 = getArg(life_ptr);
		
		break;
	}
	case LM_DEF_ABS_ZV: { // loads actor bounding box (local coords)?

		int x1 = getArg(life_ptr);
		int x2 = getArg(life_ptr);
		int y1 = getArg(life_ptr);
		int y2 = getArg(life_ptr);
		int z1 = getArg(life_ptr);
		int z2 = getArg(life_ptr);
		
		break;
	}
	case LM_DO_ROT_ZV: { // rotates actor bb

		break;
	}
	case LM_DO_CARRE_ZV: { // translates actor bb

		break;
	}
	case LM_TYPE: { //flags = flags & (data1 & TYPE_MASK) modifies actor flags

		int flags = getArg(life_ptr);
		int current_flags = mdc->flags;
		mdc->flags = (mdc->flags & 0xFE2E) + flags;

		// top anim??? deleteSub???
		
		break;
	}
	case LM_GET_HARD_CLIP: { // 

		break;
	}
	case LM_ANIM_ONCE: { //loads anim info/state

		int anim_id = getArg(life_ptr);
		int anim_id2 = getArg(life_ptr); // no idea what this is (field_44?)				
		break;
	}
		
	case LM_ANIM_REPEAT: { //loads anim info/state
		int anim_id = getArg(life_ptr);		
		break;
	}
		
	case LM_ANIM_ALL_ONCE: { //anim = data1, animInfo = data2, animType = ANIM_ONCE
		int anim_id = getArg(life_ptr);
		int next_anim_id = getArg(life_ptr); //(field_44)		
		break;
	}
		
	case LM_ANIM_RESET: {
		int anim_id = getArg(life_ptr);
		int next_anim_id = getArg(life_ptr); //(field_44)		
		break;
	}
		
	case LM_ANIM_HYBRIDE_ONCE: { //TODO
		*life_ptr += 4;
		break;
	}
		
	case LM_ANIM_HYBRIDE_REPEAT: { //TODO
		*life_ptr += 4;
		break;
	}
    case LM_HIT: {
		int v1 = getArg(life_ptr); //anim_id
		int v2 = getArg(life_ptr); //frame
		int v3 = getArg(life_ptr); //?
		int v4 = getArg(life_ptr); //anim param
		int v5 = evalVar(life_ptr, entity_id);//force
		int v6 = getArg(life_ptr); //next_anim
		break;
	}
		
	case LM_FIRE: {
		int fire_anim = getArg(life_ptr); //anim_id
		int shoot_frame = getArg(life_ptr); //frame
		int emit_point = getArg(life_ptr); //?
		int zv_size = getArg(life_ptr); 
		int hit_force = evalVar(life_ptr, entity_id);//force
		int next_anim = getArg(life_ptr); //next_anim
		break;
	}
		
	case LM_HIT_OBJECT: {
		int action_param = getArg(life_ptr);
		int hit_force = getArg(life_ptr);
		break;
	}
		
	case LM_STOP_HIT_OBJECT: {	
		break;
	}
		
	case LM_THROW: {
		int v1 = getArg(life_ptr); //anim_id
		int v2 = getArg(life_ptr); //frame
		int v3 = getArg(life_ptr); //?
		int v4 = getArg(life_ptr); //object to throw
		int v5 = evalVar(life_ptr, entity_id);//rotated
		int v6 = getArg(life_ptr); //force
		int v7 = getArg(life_ptr); //next anim		
		break;
	}
		
	case LM_MOVE: {
		int track_mode = getArg(life_ptr);
		int track_num = getArg(life_ptr);		
		break;
	}
		
	case LM_RESET_MOVE_MANUAL: {
		break;
	}
		
	case LM_CONTINUE_TRACK: {
		break;
	}
		
	case LM_DO_MOVE: {
		std::cout << "LM_DO_MOVE" << std::endl;						
		switch(mdc->track_mode) {
		case 1: //MANUAL
			//perform movement of controlled entity
			break;
		case 2: //FOLLOW
			break;			
		case 3: //TRACK
			break;
		};
		break;
	}
		
	case LM_ANIM_MOVE: {
		int v1 = getArg(life_ptr); //anim_id1
		int v2 = getArg(life_ptr); //anim_id2
		int v3 = getArg(life_ptr); //anim_id3
		int v4 = getArg(life_ptr); //anim_id4
		int v5 = getArg(life_ptr); //anim_id5
		int v6 = getArg(life_ptr); //?
		int v7 = getArg(life_ptr); //?
		break;
	}
		
	case LM_MANUAL_ROT: {
		break;
	}
		
	case LM_SET_BETA: {
		int beta = getArg(life_ptr);
		int param = getArg(life_ptr);
		break;
	}
		
	case LM_SET_ALPHA: {
		int alpha = getArg(life_ptr);
		int param = getArg(life_ptr);
		break;
	}
		
	case LM_ANGLE: {
		//alpha, beta, gamma
		int alpha = getArg(life_ptr);
		int beta = getArg(life_ptr);
		int gamma = getArg(life_ptr);
		break;
	}
		
	case LM_COPY_ANGLE: { //TODO
		*life_ptr += 2;
		break;
	}
		
	case LM_STAGE: {
		//stage, room, x, y, z
		int etage = getArg(life_ptr);
		int room = getArg(life_ptr);
		int x = getArg(life_ptr);
		int y = getArg(life_ptr);
		int z = getArg(life_ptr);		
		break;
	}
		
	case LM_TEST_COL: {
		int flags = getArg(life_ptr);
		break;
	}
		
	case LM_UP_COOR_Y: {
		break;
	}
		
	case LM_LIFE: {
		int life = getArg(life_ptr);
		mdc->life = life;
		break;
	}
		
	case LM_STAGE_LIFE: {
		int life = getArg(life_ptr); //set life on the object structure? (default life?)
		break;
	}
	case LM_LIFE_MODE: {
		int life_mode = getArg(life_ptr);
		mdc->life_mode = life_mode;
		break;
	}
	case LM_DELETE: {
		int object_id = getArg(life_ptr);
		//delete object
		break;
	}
	case LM_SPECIAL: {
		int special = getArg(life_ptr);
		break;
	}
	case LM_START_CHRONO: {
		break;
	}
	case LM_FOUND: {
		int object_id = getArg(life_ptr); //found object
		break;
	}
	case LM_TAKE: {
		int object_id = getArg(life_ptr); //take object
		break;
	}		
	case LM_IN_HAND: {
		int object_id = getArg(life_ptr); //put obj in hand
		break;
	}
	case LM_DROP: {
		int object_id = getArg(life_ptr); //drop obj
		break;
	}
	case LM_PUT: {
		int object_id = getArg(life_ptr);
		int x = getArg(life_ptr);
		int y = getArg(life_ptr);
		int z = getArg(life_ptr);
		int room = getArg(life_ptr);
		int stage = getArg(life_ptr);
		int alpha = getArg(life_ptr);
		int beta = getArg(life_ptr);
		int gamma = getArg(life_ptr);		
		break;
	}		
	case LM_PUT_AT: {
		int object_id1 = getArg(life_ptr);
		int object_id2 = getArg(life_ptr);
		break;
	}
	case LM_FOUND_NAME: {
		int text_id = getArg(life_ptr);
		break;
	}
	case LM_FOUND_BODY: {
		int object_id = getArg(life_ptr);
		break;
	}
	case LM_FOUND_FLAG: {
		int flags = getArg(life_ptr);
		break;
	}
	case LM_FOUND_WEIGHT: {
		int pos_in_track = getArg(life_ptr);
		break;
	}
	case LM_FOUND_LIFE: {
		int life_id = getArg(life_ptr);
		break;
	}
	case LM_READ: {
		int v1 = getArg(life_ptr); //read book
		int v2 = getArg(life_ptr);		
		break;
	}
	case LM_READ_ON_PICTURE: { //TODO
		*life_ptr += 16;
		break;
	}
	case LM_ANIM_SAMPLE: {
		int sound_id = evalVar(life_ptr, entity_id);
		int anim_id = getArg(life_ptr);
		int frame = getArg(life_ptr);		
		break;
	}
	case LM_2D_ANIM_SAMPLE: {
		int sound_id = evalVar(life_ptr, entity_id);
		int anim_id = getArg(life_ptr);
		int frame = getArg(life_ptr);	
		break;
	}
	case LM_REP_SAMPLE: {
		int sound_id = evalVar(life_ptr, entity_id);
		*life_ptr += 2;
		break;
	}
	case LM_STOP_SAMPLE: {		
		//TODO
		break;
	}
	case LM_SAMPLE_THEN: {
		int sound_id = evalVar(life_ptr, entity_id);
		int var = evalVar(life_ptr, entity_id);
		break;
	}
	case LM_SAMPLE_THEN_REPEAT: {
		int sound_id = evalVar(life_ptr, entity_id);
		int var = evalVar(life_ptr, entity_id);
		break;
	}
	case LM_MUSIC: {
		int music_id = getArg(life_ptr);
		break;
	}
	case LM_NEXT_MUSIC: {
		int music_id = getArg(life_ptr);
		break;
	}
	case LM_FADE_MUSIC: {
		int music_id = getArg(life_ptr);
		break;
	}
	case LM_RND_FREQ: { //TODO
		*life_ptr += 2;
		break;
	}
	case LM_LIGHT: {
		int light = getArg(life_ptr);
		break;
	}
	case LM_SHAKING: {
		*life_ptr += 2;
		break;
	}
	case LM_RAIN: {
		break;
	}
	case LM_WATER: {
		*life_ptr += 2;
		break;
	}
	case LM_CAMERA_TARGET: {
		int camera_id = getArg(life_ptr);
		break;
	}
	case LM_PICTURE: {
		int picture_id = getArg(life_ptr);
		int max_time = getArg(life_ptr);
		int sound_id = getArg(life_ptr);
		break;
	}
	case LM_PLAY_SEQUENCE: {
		int seq_id = getArg(life_ptr);
		int fade_in = getArg(life_ptr);
		int fade_out = getArg(life_ptr);
		break;
	}
	case LM_DEF_SEQUENCE_SAMPLE: {
		int num_params = ARG;
		std::vector<int> params;
		for (int i = 0; i < num_params; i++) {
			params.push_back(ARG);
		}
		break;
	}
	case LM_PROTECT: { //TODO
		break;
	}
	case LM_INVENTORY: {
		std::cout << "LM_INVENTORY" << std::endl;
		int status_screen_flag = getArg(life_ptr);
		break;
	}
	case LM_SET_INVENTORY: {
		int inventory_id = getArg(life_ptr);
		break;
	}
	case LM_SET_GROUND: {
		int ground_level = getArg(life_ptr);
		break;
	}
	case LM_MESSAGE: {
		int message_id = getArg(life_ptr);
		break;
	}
	case LM_MESSAGE_VALUE: {
		int message_id = getArg(life_ptr);
		int message_value = getArg(life_ptr);
		break;
	}
	case LM_END_SEQUENCE: { //TODO
		break;
	}
	case LM_VAR: {
		int var_id = getArg(life_ptr);
		AITDEngine::globals[var_id] = evalVar(life_ptr, entity_id);
		break;
	}
	case LM_INC: {
		int var_id = getArg(life_ptr);
		AITDEngine::globals[var_id]++;
		break;
	}
	case LM_DEC: {
		int var_id = getArg(life_ptr);
		AITDEngine::globals[var_id]--;
		break;
	}
	case LM_ADD: {
		int var_id = getArg(life_ptr);
		AITDEngine::globals[var_id] += evalVar(life_ptr, entity_id);
		break;
	}  
	case LM_SUB: {
		int var_id = getArg(life_ptr);
		AITDEngine::globals[var_id] -= evalVar(life_ptr, entity_id);
		break;
	}
	case LM_MODIF_C_VAR: {
	case LM_C_VAR: 
		int var_id = getArg(life_ptr);
		AITDEngine::C_globals[var_id] = evalVar(life_ptr, entity_id);
		break;
	}
	case LM_IF_EGAL: {

		int valueA = evalVar(life_ptr, entity_id);
		int valueB = evalVar(life_ptr, entity_id);
		std::cout << "IF " << valueA << "==" << valueB << std::endl;

		if (valueA == valueB) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;			
		}		
		break;
	}
	case LM_IF_DIFFERENT: {
		int valueA = evalVar(life_ptr, entity_id);
		int valueB = evalVar(life_ptr, entity_id);
		if (valueA != valueB) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;			
		}
		break;
	}
	case LM_IF_SUP_EGAL: {
		int valueA = evalVar(life_ptr, entity_id);
		int valueB = evalVar(life_ptr, entity_id);
		if (valueA >= valueB) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);			
			*life_ptr += jump*2;			
		}
		break;
	}
	case LM_IF_SUP: {
		int valueA = evalVar(life_ptr, entity_id);
		int valueB = evalVar(life_ptr, entity_id);
		if (valueA > valueB) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;			
		}
		break;
	}
	case LM_IF_INF_EGAL: {
		int valueA = evalVar(life_ptr, entity_id);
		int valueB = evalVar(life_ptr, entity_id);
		if (valueA <= valueB) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;			
		}
		break;
	}
	case LM_IF_INF: {
		int valueA = evalVar(life_ptr, entity_id);
		int valueB = evalVar(life_ptr, entity_id);
		if (valueA < valueB) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;			
		}
		break;
	}
	case LM_GOTO: {
		int jump = getArg(life_ptr);
		*life_ptr += jump*2;
		std::cout << "GOTO :" << jump << std::endl;
		break;
	}
	case LM_SWITCH: {
		switch_val = evalVar(life_ptr, entity_id);
		break;
	}
	case LM_CASE: {
		int case_val = getArg(life_ptr);
		std::cout << "LM_CASE " << case_val << "==" << switch_val;
		if (case_val == switch_val) {
			*life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;
		}
		break;
	}
	case LM_MULTI_CASE: {
		int num_cases = getArg(life_ptr);
		bool switch_flag = false;
		for (int i = 0; i < num_cases; i++) {
			int case_val = getArg(life_ptr);
			if (case_val == switch_val) {
				switch_flag = true;
			}
			*life_ptr += 2;
		}
		
		if (switch_flag) {
			int jump = getArg(life_ptr);
			*life_ptr += jump*2;
		}
		*life_ptr += 2;		
		break;
	}
	case LM_RETURN: {
		std::cout << "RET" << std::endl;
		finished_loading = true;
		break;
	}
	case LM_END: {
		std::cout << "END" << std::endl;
		finished_loading = true;
		break;
	}
	default: {
		std::cout << "Unsupported opcode" << std::endl;
		getchar();
		break;
	}

	}
}

void ScriptingManager::runScript(int script_id, Entity::Id entity_id, const EntityManager& em) {

	std::cout << "processing life:" << script_id << std::endl;
	getchar();
	char *life_ptr = listLife->get(script_id);
	
	finished_loading = false;
	while(!finished_loading) {
	
		int16 current_opcode = *(int16 *)(life_ptr);
		life_ptr += 2;
		
		CommandId command = CommandMacroTable[current_opcode & 0x7FFF];

		if(current_opcode & 0x8000) {
			int16 object_idx = *(int16 *)life_ptr;
			life_ptr += 2;

			//	assert(object_idx != -1);
			if (object_idx != -1) {

				// if the object has owner, run as actorCommand instead
				// getEntityOwner(object_idx) <- return entity (actor) corresponding to object
				int16 actor_idx = ObjectManager::object_map[object_idx].ownerIdx;

				std::cout << "actor:" << actor_idx << std::endl;
				
				if (actor_idx != -1) {
					loadObjectCommand(command, object_idx, &life_ptr);
				}
				else {
					loadActionCommand(command, entity_id, &life_ptr);
				}
			}
		}
		else {
			loadActionCommand(command, entity_id, &life_ptr);
		}

		getchar();
//		script.command_vector.push_back();
	}
}
