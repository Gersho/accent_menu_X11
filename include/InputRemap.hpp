#ifndef INPUT_REMAP_HPP
# define INPUT_REMAP_HPP

#include "accent_menu.h"

class InputRemap
{
	private:
		InputRemap();
		const unsigned int 	_trigger_keycode;
		const Bool			_trigger_isupper;
		const KeySym		_trigger_keysym;

		const KeySym 	_one;
		const KeySym 	_two;
		const KeySym 	_three;
		const KeySym 	_four;
		const KeySym 	_five;
		const KeySym 	_six;
		const KeySym 	_seven;
		const KeySym 	_eight;
		const KeySym 	_nine;
		const KeySym 	_zero;

	public:
		InputRemap(
			KeyCode trigger_keycode,
			Bool trigger_isupper,
			KeySym trigger_keysym,
			KeySym 	one,
			KeySym 	two,
			KeySym 	three,
			KeySym 	four,
			KeySym 	five,
			KeySym 	six,
			KeySym 	seven,
			KeySym 	eight,
			KeySym 	nine,
			KeySym 	zero
			);

		~InputRemap();

		Bool get_trigger_isupper();
		KeySym get_output_keysym(unsigned int option_keycode);
		unsigned int get_trigger_keycode();
		Bool check_input(XEvent &ev);	
};

#endif 
