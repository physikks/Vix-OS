#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../common/common.h"

enum keyboard_disposition{
	QWERTY,
	AZERTY,
};

enum special_key{
	BACKSPACE,
	ESCAPE,
	SHIFT,
	TAB,
	CAPS_LOCK,
};


union key
{
	char ch;
	enum special_key sp;
};

#ifdef __cplusplus
extern "C" {
#endif

uint8 get_scancode();
union key keyboard_handler();

#ifdef __cplusplus
}
#endif

#endif