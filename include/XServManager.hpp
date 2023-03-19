#ifndef XSERV_MANAGER_HPP
#define XSERV_MANAGER_HPP

#include "accent_menu.h"

class InputRemap;

class XServManager
{
	private:
		unsigned int _borrowed_keycode;
		std::vector<InputRemap> _remap_list;
		Display *_display;
		unsigned long    _screen;
		Window        _root_win;
		Bool		_active;
		unsigned int _on_off_keycode;
		int _xfd;

		void _get_keycode();

	public:
		XServManager();
		~XServManager();

		//need to handle signals
		int pip[2];
		sig_atomic_t done;

		void load_config_from_file(const char* path);
		void start();

	private:
		void _add_input_remap(
					char *trigger,
					KeySym 	one,
					KeySym 	two,
					KeySym 	three,
					KeySym 	four,
					KeySym 	five,
					KeySym 	six,
					KeySym 	seven,
					KeySym 	eight,
					KeySym 	nine,
					KeySym 	zero);
		void _grab_triggers();
		void _ungrab_triggers();
		void _resend_event(XEvent &src);
		void _trigger_mode(InputRemap &remap);
		void _send_remapped_event(XEvent &src, KeySym ks);
		void _send_backspace();


	class OpenDisplayFailException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};

	class GetKeycodeFailException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};

	class ConfigErrorException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};

	class XGrabKeyException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};

	class XUnGrabKeyException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	
	class OpenFileException : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
};

#endif 
