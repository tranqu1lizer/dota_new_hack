#include <cstdint>

enum EPanelEventSource_t
{
	k_ePanelEventSourceProgram,
	k_ePanelEventSourceGamepad,
	k_ePanelEventSourceKeyboard,
	k_ePanelEventSourceMouse,

	k_ePanelEventSourceInvalid
};

enum MouseCode
{
	MOUSE_INVALID = -1,
	MOUSE_LEFT = 0,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_LAST,
};

struct MouseData_t
{
	EPanelEventSource_t m_source; // this needs to be the first field of the struct, since it's unioned in InputMessage_t

	MouseCode m_mouse_code;
	uint32_t m_modifiers;
	uint8_t m_repeat_count;
	int m_delta;
};