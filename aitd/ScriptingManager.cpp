#include "ScriptingManager.hpp"
#include "AITDEngine.hpp"
#include "ObjectManager.hpp"
#include "Components.hpp"
#include <hqr.h>


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
	LM_WAIT_GAME_OVER,
	LM_GET_MATRICE,
	LM_STAGE_LIFE,
	LM_CONTINUE_TRACK,
	LM_ANIM_RESET,
	LM_RESET_MOVE_MANUAL,
	LM_RAIN,
	LM_ANIM_HYBRIDE_ONCE,
	LM_ANIM_HYBRIDE_REPEAT,
	LM_MODIF_C_VAR,
	LM_CALL_INVENTORY, // 0x60
	LM_BODY_RESET,
	LM_DEL_INVENTORY,
	LM_SET_INVENTORY,
	LM_PLAY_SEQUENCE,
	LM_2D_ANIM_SAMPLE,
	LM_SET_GROUND,
	LM_PROTECT,
	LM_DEF_ABS_ZV,
	LM_DEF_SEQUENCE_SAMPLE,
	LM_READ_ON_PICTURE,
	LM_FIRE_UP_DOWN
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

int16 getArg(char *life_ptr) {
	int16 arg = *life_ptr;
	life_ptr += 2;
	return arg;
}

#define ARG getArg(life_ptr)

/* Access to attributes of the current processed actor*/
int ScriptingManager::evalVar(char* life_ptr) {

	//CMD VAR1 VAR2
	//IF(VAR1 ==-1) ret VAR2
	//IF(VAR1 == 0) ret globals[VAR2]

	
	int16 var1 = getArg(life_ptr); 

	if (var1 == -1) {
		int16 var2 = getArg(life_ptr);
		return var2;
	}
	else if (var1 == 0) {
		int16 var2 = getArg(life_ptr);
//		return varTable[var2];
		return 0;
	}
	else {

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

		switch(var1) {
		case 0x0: { //return colliding actor (-1) otherwise

			break;
		}
		case 0x1: { //return HARD_DEC field (no idea what this does)
			
			break;
		}
		case 0x2: { //return HARD_COL field (hard collision - with environment?)

			break;
		}
		case 0x3: { //return HIT field (index in actor table of actor being hit)

		}
		case 0x4: { // HIT_BY field (actor that is hitting "us")

		}
		case 0x5: { // ANIM field (animation index?)

		}
		case 0x6: { // END_ANIM field. Signals animation has ended
			
		}
		case 0x7: { // FRAME field. key-frame index
			
		}	
		case 0x8: { // FRAME_END field. signals key frame change
			
		}
		case 0x9: { // BODY_NUM. Body index of current actor (from listBody buffer)

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
		}
		case 0x15: { //COL_BY again?
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
		case 0x20: { // query bits of object flags2
			int16 object_id = getArg(life_ptr);
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
//			return globals[g_var];
			break;
		}
		case 0x25: { // etage

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


void ScriptingManager::loadObjectCommand(CommandId command, char* life_ptr) {


	std::cout << "obj: " << command << std::endl;

	
	switch(command) {
		/*
	case LM_BODY: //body = var
		return OCmdSetBody(evalVar());
		break;
	case LM_BODY_RESET: //body = var, anim = var
		return OCmdResetBody(arg1, arg2);
		break;
	case LM_TYPE: //flags = flags & (data1 & TYPE_MASK)
		return OCmdSetType(ARG, ARG);
		break;
	case LM_ANIM_ALL_ONCE: //anim = data1, animInfo = data2, animType = ANIM_ONCE
		return OCmdAnimAllOnce(ARG, ARG);
		break;
	case LM_ANIM_RESET:
		return OCmdAnimReset(ARG, ARG);
		break;
	case LM_MOVE:
		//etage, room
		return OCmdMove(ARG, ARG);
		break;
	case LM_ANGLE:
		//alpha, beta, gamma
		return OCmdAngle(ARG, ARG, ARG);
		break;
	case LM_STAGE:
		//stage, room, x, y, z
		return OCmdStage(ARG, ARG, ARG, ARG, ARG);
		break;
	case LM_TEST_COL:
		return OCmdTestCol(ARG);
		break;
	case LM_LIFE:
		return OCmdSetLife(ARG);
		break;
	case LM_LIFE_MODE:
		return OCmdSetLifeMode(ARG);
		break;
	case LM_FOUND_NAME:
		return OCmdSetFoundName(ARG);
		break;
	case LM_FOUND_BODY:
		return OCmdSetFoundBody(ARG);
		break;
	case LM_FOUND_FLAG:
		return OCmdSetFoundFlag(ARG);
		break;
	case LM_FOUND_WEIGHT:
		return OCmdSetPosInTrack(ARG);
		break;
		case LM_START_CHRONO:
		break;
	default:
		std::cout << "Unsupported opcode" << std::endl;
		break;
		*/
	}
}

void ScriptingManager::loadActionCommand(CommandId command, Entity::Id entity_id, char* life_ptr) {

	std::cout << "action: " << command << std::endl;

	Components::MetaDataComponent *mdc =
		entity_manager->getComponentPtr<Components::MetaDataComponent>(entity_id);

	static int switch_val;
	
	switch(command) {
	case LM_BODY: { //body = var

		int new_object_id = evalVar(life_ptr);
		if (mdc->body_num != new_object_id) {
			mdc->body_num = new_object_id;

			//assign new mesh/body/animation in the current frame
			
		}								
		break;
	}	
	case LM_BODY_RESET: { //body = var, anim = var

		//assign new mesh/body/animation in the frame zero
		int new_object_id = evalVar(life_ptr);
		int new_anim_id = evalVar(life_ptr);
		

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
		life_ptr += 4;
		break;
	}
		
	case LM_ANIM_HYBRIDE_REPEAT: { //TODO
		life_ptr += 4;
		break;
	}
    case LM_HIT: {
		int v1 = getArg(life_ptr); //anim_id
		int v2 = getArg(life_ptr); //frame
		int v3 = getArg(life_ptr); //?
		int v4 = getArg(life_ptr); //anim param
		int v5 = evalVar(life_ptr);//force
		int v6 = getArg(life_ptr); //next_anim
		break;
	}
		
	case LM_FIRE: {
		int fire_anim = getArg(life_ptr); //anim_id
		int shoot_frame = getArg(life_ptr); //frame
		int emit_point = getArg(life_ptr); //?
		int zv_size = getArg(life_ptr); 
		int hit_force = evalVar(life_ptr);//force
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
		int v5 = evalVar(life_ptr);//rotated
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
		break;
	}
		
	case LM_ANIM_MOVE: {
		int v1 = getArg(life_ptr); //anim_id1
		int v2 = getArg(life_ptr); //anim_id2
		int v3 = getArg(life_ptr); //anim_id3
		int v4 = getArg(life_ptr); //anim_id4
		int v5 = evalVar(life_ptr);//anim_id5
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
		life_ptr += 2;
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
		life_ptr += 16;
		break;
	}
	case LM_ANIM_SAMPLE: {
		int sound_id = evalVar(life_ptr);
		int anim_id = getArg(life_ptr);
		int frame = getArg(life_ptr);		
		break;
	}
	case LM_2D_ANIM_SAMPLE: {
		int sound_id = evalVar(life_ptr);
		int anim_id = getArg(life_ptr);
		int frame = getArg(life_ptr);	
		break;
	}
	case LM_REP_SAMPLE: {
		int sound_id = evalVar(life_ptr);
		life_ptr += 2;
		break;
	}
	case LM_STOP_SAMPLE: {		
		//TODO
		break;
	}
	case LM_SAMPLE_THEN: {
		int sound_id = evalVar(life_ptr);
		int var = evalVar(life_ptr);
		break;
	}
	case LM_SAMPLE_THEN_REPEAT: {
		int sound_id = evalVar(life_ptr);
		int var = evalVar(life_ptr);
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
		life_ptr += 2;
		break;
	}
	case LM_LIGHT: {
		int light = getArg(life_ptr);
		break;
	}
	case LM_SHAKING: {
		life_ptr += 2;
		break;
	}
	case LM_RAIN: {
		break;
	}
	case LM_WATER: {
		life_ptr += 2;
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
		AITDEngine::globals[var_id] = evalVar(life_ptr);
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
		AITDEngine::globals[var_id] += evalVar(life_ptr);
		break;
	}  
	case LM_SUB: {
		int var_id = getArg(life_ptr);
		AITDEngine::globals[var_id] -= evalVar(life_ptr);
		break;
	}
	case LM_C_VAR: {
		int var_id = getArg(life_ptr);
		AITDEngine::C_globals[var_id] = evalVar(life_ptr);
		break;
	}
	case LM_IF_EGAL: {
		int valueA = evalVar(life_ptr);
		int valueB = evalVar(life_ptr);
		if (valueA == valueB) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;			
		}		
		break;
	}
	case LM_IF_DIFFERENT: {
		int valueA = evalVar(life_ptr);
		int valueB = evalVar(life_ptr);
		if (valueA != valueB) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;			
		}
		break;
	}
	case LM_IF_SUP_EGAL: {
		int valueA = evalVar(life_ptr);
		int valueB = evalVar(life_ptr);
		if (valueA >= valueB) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);			
			life_ptr += jump*2 + 2;			
		}
		break;
	}
	case LM_IF_SUP: {
		int valueA = evalVar(life_ptr);
		int valueB = evalVar(life_ptr);
		if (valueA > valueB) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;			
		}
		break;
	}
	case LM_IF_INF_EGAL: {
		int valueA = evalVar(life_ptr);
		int valueB = evalVar(life_ptr);
		if (valueA <= valueB) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;			
		}
		break;
	}
	case LM_IF_INF: {
		int valueA = evalVar(life_ptr);
		int valueB = evalVar(life_ptr);		
		if (valueA < valueB) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;			
		}
		break;
	}
	case LM_GOTO: {
		int jump = getArg(life_ptr);
		life_ptr += jump*2 + 2;
		break;
	}
	case LM_SWITCH: {
		switch_val = evalVar(life_ptr);
		break;
	}
	case LM_CASE: {
		int case_val = getArg(life_ptr);
		if (case_val == switch_val) {
			life_ptr += 2;
		}
		else {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;
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
			life_ptr += 2;
		}
		
		if (switch_flag) {
			int jump = getArg(life_ptr);
			life_ptr += jump*2 + 2;
		}
		life_ptr += 2;		
		break;
	}
	case LM_RETURN: {		
		finished_loading = true;
		break;
	}
	case LM_END: {
		finished_loading = true;
		break;
	}
	default: {
		std::cout << "Unsupported opcode" << std::endl;
		break;
	}

	}
}

void ScriptingManager::runScript(int script_id, Entity::Id entity_id, const EntityManager& em) {

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
							loadObjectCommand(command, life_ptr);
						}
						else {
							loadActionCommand(command, entity_id, life_ptr);
						}
					}
				}
				else {
					loadActionCommand(command, entity_id, life_ptr);
				}

				getchar();
//		script.command_vector.push_back();
			}

		}
