#pragma once
#include "entities/Entity.hpp"
#include "World.hpp"
#include <fitd.h>
#include <map>

class ObjectData;

#define TYPE_MASK 0x1D1
#define ANIM_ONCE             0
#define ANIM_REPEAT           1
#define ANIM_UNINTERRUPTABLE  2
#define ANIM_RESET            4

enum CommandId {
	LM_DO_MOVE = 0,
	LM_ANIM_ONCE,
	LM_ANIM_ALL_ONCE,
	LM_BODY,
	LM_IF_EGAL,
	LM_IF_DIFFERENT,
	LM_IF_SUP_EGAL,
	LM_IF_SUP,
	LM_IF_INF_EGAL,
	LM_IF_INF,
	LM_GOTO, //10
	LM_RETURN,
	LM_END,
	LM_ANIM_REPEAT,
	LM_ANIM_MOVE,
	LM_MOVE,
	LM_HIT,
	LM_MESSAGE,
	LM_MESSAGE_VALUE,
	LM_VAR,
	LM_INC, //20
	LM_DEC,
	LM_ADD,
	LM_SUB,
	LM_LIFE_MODE,
	LM_SWITCH,
	LM_CASE,
	LM_CAMERA,
	LM_START_CHRONO,
	LM_MULTI_CASE,
	LM_FOUND, //30
	LM_LIFE,
	LM_DELETE,
	LM_TAKE,
	LM_IN_HAND,
	LM_READ,
	LM_ANIM_SAMPLE, //36
	LM_SPECIAL,  
	LM_DO_REAL_ZV,
	LM_SAMPLE,
	LM_TYPE, //40
	LM_GAME_OVER,
	LM_MANUAL_ROT,
	LM_RND_FREQ, //43
	LM_MUSIC,
	LM_SET_BETA,
	LM_DO_ROT_ZV,
	LM_STAGE,
	LM_FOUND_NAME,
	LM_FOUND_FLAG,
	LM_FOUND_LIFE, //50
	LM_CAMERA_TARGET,
	LM_DROP,
	LM_FIRE,
	LM_TEST_COL,
	LM_FOUND_BODY,
	LM_SET_ALPHA,
	LM_STOP_BETA,
	LM_DO_MAX_ZV,
	LM_PUT,
	LM_C_VAR, //60
	LM_DO_NORMAL_ZV,
	LM_DO_CARRE_ZV,
	LM_SAMPLE_THEN,
	LM_LIGHT, 
	LM_SHAKING,
	LM_INVENTORY, //66
	LM_FOUND_WEIGHT,
	LM_UP_COOR_Y,
	LM_SPEED,
	LM_PUT_AT, //70
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
	LM_CALL_INVENTORY,
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
	LM_FIRE_UP_DOWN, // AITD3 only
};


// class ObjectCommand {
// public:
// //	CommandId command_id;
// 	std::string command_str;
// 	int num_args;
// 	std::vector<int16> arg_list;

// };

// class ActionCommand {
// };

class Command {
public:
	typedef std::shared_ptr<Command> Ptr;
	virtual void run() {};
};

class Script {
public:
	
protected:

	std::vector<Command::Ptr> command_vector;
};

class ScriptingManager {
public:
	typedef std::shared_ptr<ScriptingManager> Ptr;
	ScriptingManager(EntityManager::Ptr em, World::Ptr w);
	void runScript(int script_id, Entity::Id entity_id, const EntityManager& em);
	void loadObjectCommand(CommandId command, int object_idx, char** life_ptr);
	void loadActionCommand(CommandId command, Entity::Id, char** life_ptr);
	int evalVar(char** life_ptr, Entity::Id entity_id);

protected:
	
	EntityManager::Ptr entity_manager;
	World::Ptr world;
	Fitd::hqrEntryStruct* listLife;
	bool finished_loading = false;
};
