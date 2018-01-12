#include "InputManager.hpp"
#include <common/bgfx_utils.h>

uint16_t InputManager::m_keys;
uint32_t InputManager::m_debug = BGFX_DEBUG_TEXT;
uint32_t InputManager::m_reset = BGFX_RESET_VSYNC;
float InputManager::m_mouse_angle;
entry::MouseState InputManager::m_mouseState;
entry::MouseState InputManager::m_oldMouseState;
Mouse InputManager::m_mouse;

float computeMouseAngle(entry::MouseState mstate, const uint32_t & width,
			const uint32_t & height) {

	Eigen::Vector2f mvec;
	Eigen::Vector2d refvec = Eigen::Vector2d(-1.0f, -1.0f);
	refvec.normalize();

	mvec(0) = mstate.m_mx - width/2.0;
	mvec(1) = mstate.m_my - height/2.0;
	mvec.normalize();

	return atan2f(mvec(0)*refvec(1) - mvec(1)*refvec(0),
		      mvec(0)*refvec(0) + mvec(1)*refvec(1));
}

int cmdRotateCamera(CmdContext* /*_context*/, void* /*_userData*/, int _argc, char const* const* _argv)
{
	if (_argc > 1)
	{
		if (0 == strcmp(_argv[1], "left")) {
			InputManager::setKeyState(CAMERA_KEY_ROTATE_LEFT, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "right")) {
			InputManager::setKeyState(CAMERA_KEY_ROTATE_RIGHT, true);
			return 0;
		}
	}
}

int cmdMoveChar(CmdContext* /*_context*/, void* /*_userData*/, int _argc, char const* const* _argv)
{
	if (_argc > 1)
	{
		if (0 == strcmp(_argv[1], "left") )
		{
			InputManager::setKeyState(ACTOR_KEY_LEFT, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "right") )
		{
			InputManager::setKeyState(ACTOR_KEY_RIGHT, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "up") )
		{
			InputManager::setKeyState(ACTOR_KEY_UP, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "down") )
		{
			InputManager::setKeyState(ACTOR_KEY_DOWN, true);
			return 0;
		}
	}

	return 1;
}

int cmdMove(CmdContext* /*_context*/, void* /*_userData*/, int _argc, char const* const* _argv)
{
	if (_argc > 1)
	{
		if (0 == strcmp(_argv[1], "left") )
		{
			InputManager::setKeyState(CAMERA_KEY_LEFT, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "right") )
		{
			InputManager::setKeyState(CAMERA_KEY_RIGHT, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "up") )
		{
			InputManager::setKeyState(CAMERA_KEY_UP, true);
			return 0;
		}
		else if (0 == strcmp(_argv[1], "down") )
		{
			InputManager::setKeyState(CAMERA_KEY_DOWN, true);
			return 0;
		}
	}

	return 1;
}

static void cmd(const void* _userData)
{
	cmdExec( (const char*)_userData);
}



// static const InputBinding s_camBindings[] =
//  {
//  	{ entry::Key::KeyW,             entry::Modifier::None, 0, cmd, "move up"  },
//  	{ entry::Key::GamepadUp,        entry::Modifier::None, 0, cmd, "move up"  },
//  	{ entry::Key::GamepadDown,      entry::Modifier::None, 0, cmd, "move down" },
// 	{ entry::Key::KeyA,             entry::Modifier::None, 0, cmd, "move left"     },
// 	{ entry::Key::GamepadLeft,      entry::Modifier::None, 0, cmd, "move left"     },
//  	{ entry::Key::KeyD,             entry::Modifier::None, 0, cmd, "move right"    },
//  	{ entry::Key::GamepadRight,     entry::Modifier::None, 0, cmd, "move right"    },
// 	{ entry::Key::KeyQ,             entry::Modifier::None, 0, cmd, "rotate left"    },
// 	{ entry::Key::KeyE,             entry::Modifier::None, 0, cmd, "rotate right"    },

//  	INPUT_BINDING_END
// };
 
static const InputBinding s_moveBindings[] =
{
	{ entry::Key::KeyW,             entry::Modifier::None, 0, cmd, "move up"  },
//	{ entry::Key::GamepadUp,        entry::Modifier::None, 0, cmd, "move up"  },
	{ entry::Key::KeyA,             entry::Modifier::None, 0, cmd, "move left"     },
//	{ entry::Key::GamepadLeft,      entry::Modifier::None, 0, cmd, "move left"     },
	{ entry::Key::KeyS,             entry::Modifier::None, 0, cmd, "move down" },
//	{ entry::Key::GamepadDown,      entry::Modifier::None, 0, cmd, "move down" },
	{ entry::Key::KeyD,             entry::Modifier::None, 0, cmd, "move right"    },
//	{ entry::Key::GamepadRight,     entry::Modifier::None, 0, cmd, "move right"    },
	INPUT_BINDING_END
};

void InputManager::init() {
//	cmdAdd("move", cmdMove);
//	cmdAdd("rotate", cmdRotateCamera);
	cmdAdd("move", cmdMoveChar);
	inputAddBindings("moveBindings", s_moveBindings);
//	inputAddBindings("camBindings", s_camBindings);

}
