
#include "../include/InputRemap.hpp"

InputRemap::InputRemap(
	KeyCode trigger_keycode,
	Bool trigger_isupper,
	KeySym trigger_keysym,
	KeySym one,
	KeySym two,
	KeySym three,
	KeySym four,
	KeySym five,
	KeySym six,
	KeySym seven,
	KeySym eight,
	KeySym nine,
	KeySym zero) : _trigger_keycode(trigger_keycode),
				   _trigger_isupper(trigger_isupper),
				   _trigger_keysym(trigger_keysym),
				   _one(one),
				   _two(two),
				   _three(three),
				   _four(four),
				   _five(five),
				   _six(six),
				   _seven(seven),
				   _eight(eight),
				   _nine(nine),
				   _zero(zero)
{
}

InputRemap::~InputRemap() {}

KeySym InputRemap::get_output_keysym(unsigned int option_keycode)
{
	switch (option_keycode)
	{
	case 10:
		return _one;
	case 11:
		return _two;
	case 12:
		return _three;
	case 13:
		return _four;
	case 14:
		return _five;
	case 15:
		return _six;
	case 16:
		return _seven;
	case 17:
		return _eight;
	case 18:
		return _nine;
	case 19:
		return _zero;
	default:
		return _trigger_keysym;
	}
}

unsigned int InputRemap::get_trigger_keycode()
{
	return _trigger_keycode;
}

Bool InputRemap::get_trigger_isupper()
{
	return _trigger_isupper;
}

Bool InputRemap::check_input(XEvent &ev)
{
	Bool upper;
	unsigned int state;

	if(ev.xkey.keycode != _trigger_keycode)
		return False;

	// ignore numlock
	state = ev.xkey.state;
	if(ev.xkey.state & (unsigned int)16)
		state -= 16;

	//check for other modifier apart from shift/caps/num, see X.h line 220
	if(236U & state)
		return False;

	//check for the different permutations of shift and CAPS to get proper case
	if(state & (unsigned int)1 && state & (unsigned int)2)
		upper = False;
	else if(state & (unsigned int)1)
		upper = True;
	else if(state & (unsigned int)2)
		upper = True;
	else
		upper = False;

	if((upper && !_trigger_isupper) || (!upper && _trigger_isupper))
		return False;

	return True;
}