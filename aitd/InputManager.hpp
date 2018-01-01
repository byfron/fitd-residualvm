#ifndef INPUTMANAGER_H_HEADER_GUARD
#define INPUTMANAGER_H_HEADER_GUARD

#include <common/common.h>
#include <common/entry/input.h>
#include <common/entry/cmd.h>
#include <graphics/Camera.hpp>

#define KEY_ROTATE_CAM_LEFT   UINT8_C(0x01)
#define KEY_ROTATE_CAM_RIGHT  UINT8_C(0x02)
#define KEY_MOVE_LEFT      UINT8_C(0x04)
#define KEY_MOVE_RIGHT     UINT8_C(0x08)
#define KEY_MOVE_UP        UINT8_C(0x10)
#define KEY_MOVE_DOWN      UINT8_C(0x20)

struct Mouse
{
	Mouse()
	{
		m_dx = 0.0f;
		m_dy = 0.0f;
		m_prevMx = 0.0f;
		m_prevMx = 0.0f;
		m_scroll = 0;
		m_scrollPrev = 0;
	}

	void update(float _mx, float _my, int32_t _mz, uint32_t _width, uint32_t _height)
	{
		const float widthf  = float(int32_t(_width));
		const float heightf = float(int32_t(_height));

		// Delta movement.
		m_dx = float(_mx - m_prevMx)/widthf;
		m_dy = float(_my - m_prevMy)/heightf;

		m_prevMx = _mx;
		m_prevMy = _my;

		// Scroll.
		m_scroll = _mz - m_scrollPrev;
		m_scrollPrev = _mz;
	}



	float m_dx; // Screen space.
	float m_dy;
	float m_prevMx;
	float m_prevMy;
	int32_t m_scroll;
	int32_t m_scrollPrev;


};


float computeMouseAngle(entry::MouseState mstate, const uint32_t & width,
			const uint32_t & height);

class InputManager {

public:

	void init();


	static void setKeyState(uint8_t _key, bool _down)
	{
		m_keys &= ~_key;
		m_keys |= _down ? _key : 0;
	}

	static bool isLeftMouseButtonJustPressed() {
		if (not m_oldMouseState.m_buttons[entry::MouseButton::Left] &&
		    m_mouseState.m_buttons[entry::MouseButton::Left]) return true;
		return false;
	}

	static bool isLeftMouseButtonPressed() {
		return m_mouseState.m_buttons[entry::MouseButton::Left];
	}

	static bool isRightMouseButtonPressed() {
		return m_mouseState.m_buttons[entry::MouseButton::Right];
	}

	static bool processEvents(uint32_t & width, uint32_t & height) {

		m_oldMouseState = m_mouseState;

		m_mouse.update(float(m_mouseState.m_mx),
			       float(m_mouseState.m_my),
			       m_mouseState.m_mz, width, height);

		m_mouse_angle = computeMouseAngle(m_mouseState, width, height);

		return entry::processEvents(width, height, m_debug, m_reset, &m_mouseState);
	}

	static uint32_t m_debug;
	static uint32_t m_reset;
	static entry::MouseState m_mouseState;
	static entry::MouseState m_oldMouseState;
	static Mouse m_mouse;
	static uint16_t m_keys;
	static float m_mouse_angle;

};

#endif
