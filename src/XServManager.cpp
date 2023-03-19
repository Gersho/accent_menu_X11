#include "../include/XServManager.hpp"

XServManager::XServManager()
{
	_display = XOpenDisplay(NULL);
	if (_display == NULL)
		throw XServManager::OpenDisplayFailException();
    _screen = DefaultScreen(_display);
    _root_win = RootWindow(_display, _screen);
	_xfd = ConnectionNumber(_display);
	_get_keycode();
	std::cout << "Borrowing keycode: " << _borrowed_keycode << std::endl;
}
XServManager::~XServManager()
{
	KeySym ks = NoSymbol;

	if(_active)
		_ungrab_triggers();
	XUngrabKey(_display, _borrowed_keycode, AnyModifier, _root_win);
	XChangeKeyboardMapping(_display, _borrowed_keycode, 1, &ks, 1);
	XCloseDisplay(_display);
	std::cout << "Closing." << std::endl;
}

void XServManager::_get_keycode()
{
	int min = -1;
	int max = -1;
	KeySym ks = NoSymbol;

	XDisplayKeycodes(_display, &min, &max);

	if(!min || !max)
		throw XServManager::GetKeycodeFailException();

	while(min <= max)
	{
		//ks = XKeycodeToKeysym(_display, min, 0);
		ks = XkbKeycodeToKeysym(_display, min, 0, 0);

		if(!ks)
		{
			_borrowed_keycode = min;
			return;
		}
		min++;
	}
	throw XServManager::GetKeycodeFailException();
}

void XServManager::_add_input_remap(
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
	KeySym 	zero)
{
	KeyCode kc;
	Bool upper;
	KeySym ks;

	if((upper = isupper(*trigger)));
	else if(islower(*trigger));
	else throw XServManager::ConfigErrorException();

	ks = XStringToKeysym(trigger);
	if (!ks)
		throw XServManager::ConfigErrorException();
	kc = XKeysymToKeycode(_display, ks);

	_remap_list.push_back(InputRemap(kc,upper,ks,
		one,two,three,four,five,six,seven,eight,nine,zero));
}

void XServManager::_grab_triggers()
{
	int min = 10;
	int max = 19;

	for(std::vector<InputRemap>::iterator it = _remap_list.begin(); it != _remap_list.end(); it++)
	{
		int ret = XGrabKey(_display, it->get_trigger_keycode(), AnyModifier, _root_win, True, GrabModeAsync, GrabModeAsync);
		if(!ret)
			throw XServManager::XGrabKeyException();
	}

	while(min <= max)
	{
		int ret = XGrabKey(_display, min, AnyModifier, _root_win, True, GrabModeAsync, GrabModeAsync);
		if(!ret)
			throw XServManager::XGrabKeyException();
		min++;
	}

	std::cout << "accent_menu_X11 ON" << std::endl;
	_active = True;
}

void XServManager::_ungrab_triggers()
{
	int min = 10;
	int max = 19;

	for(std::vector<InputRemap>::iterator it = _remap_list.begin(); it != _remap_list.end(); it++)
	{
		int ret = XUngrabKey(_display, it->get_trigger_keycode(), AnyModifier, _root_win);
		if(!ret)
			throw XServManager::XUnGrabKeyException();
	}

	while(min <= max)
	{
		int ret = XUngrabKey(_display, min, AnyModifier, _root_win);
		if(!ret)
			throw XServManager::XUnGrabKeyException();
		min++;
	}

	std::cout << "accent_menu_X11 OFF" << std::endl;
	_active = False;
}

void XServManager::start()
{
	XEvent	ev;
	
	int ret = XGrabKey(_display, _on_off_keycode, AnyModifier, _root_win, True, GrabModeAsync, GrabModeAsync);
	if(!ret)
		throw XServManager::XGrabKeyException();

	_grab_triggers();

 	fd_set set_read, set_save;
    FD_ZERO(&set_save); 
    FD_SET(_xfd, &set_save);
    FD_SET(pip[0], &set_save);
    int max_fd = _xfd > pip[0] ? _xfd : pip[0];

	while(!done)
	{

		memcpy(&set_read, &set_save, sizeof(set_save));
		select(max_fd+1, &set_read, NULL, NULL, NULL);

		if(!XPending(_display))
		{
            char a[1];
			read(pip[0], a, 1);
			continue; 
        }

		XNextEvent(_display, &ev);

		if(ev.type == KeyPress)
		{
			if (ev.xkey.keycode == _on_off_keycode)
			{
				if(_active)
					_ungrab_triggers();
				else
					_grab_triggers();
				continue;
			}

			if (ev.xkey.keycode >= 10 && ev.xkey.keycode <= 19)
			{
				_resend_event(ev);
				continue;
			}

			std::vector<InputRemap>::iterator it;
			for(it = _remap_list.begin(); it != _remap_list.end(); it++)
			{
				if(it->check_input(ev))
				{
					_resend_event(ev);
					_trigger_mode(*it);
					break;
				}
			}

			if(it == _remap_list.end())
			{
				_resend_event(ev);
				continue;
			}
		}
		else if(ev.type == KeyRelease)
		{
			_resend_event(ev);
		}
	}
}


void XServManager::_resend_event(XEvent &src)
{
	Window winFocus;
	int    revert;

	XGetInputFocus(_display, &winFocus, &revert);
	src.xkey.window = winFocus;
	XSendEvent(_display, winFocus, True, (src.xkey.type - 1), &src);
	XSync(_display, True);
}


void XServManager::_send_backspace()
{
	XKeyEvent event;
	Window winFocus;
	int    revert;

	XGetInputFocus(_display, &winFocus, &revert);
	event.display     = _display;
	event.window      = winFocus;
	event.root        = _root_win;
	event.subwindow   = None;
	event.time        = CurrentTime;
	event.x           = 1;
	event.y           = 1;
	event.x_root      = 1;
	event.y_root      = 1;
	event.same_screen = True;
	event.keycode     = 22;
	event.state       = 0;
	event.type = KeyPress;

	XSendEvent(_display, winFocus, True, KeyPressMask, (XEvent *)&event);
	event.type = KeyRelease;
	XSendEvent(_display, winFocus, True, KeyReleaseMask, (XEvent *)&event);

}

void XServManager::_trigger_mode(InputRemap &remap)
{
	XEvent	ev;

	while(!done)
	{
		XNextEvent(_display, &ev);

		if(ev.type == KeyPress)
		{
			if (ev.xkey.keycode >= 10 && ev.xkey.keycode <= 19)
			{
				KeySym ks = remap.get_output_keysym(ev.xkey.keycode);
				ev.xkey.state = remap.get_trigger_isupper() ? 1 : 0;
				_send_backspace();
				_send_remapped_event(ev, ks);
				break;
			}
			else
			{
				_resend_event(ev);
			};
		}
		else if(ev.type == KeyRelease)
		{
			if(ev.xkey.keycode == remap.get_trigger_keycode())
			{
				_resend_event(ev);
				
				break;
			}
			else
			{
				_resend_event(ev);
			}
		}
	}
}

void XServManager::_send_remapped_event(XEvent &src, KeySym ks)
{
	Window winFocus;
	int    revert;

	XChangeKeyboardMapping(_display, _borrowed_keycode, 1, &ks, 1);
	XGetInputFocus(_display, &winFocus, &revert);
	src.xkey.window = winFocus;
	src.xkey.keycode = _borrowed_keycode;
	XSendEvent(_display, winFocus, True, (src.xkey.type - 1), &src);
}

void XServManager::load_config_from_file(const char* path)
{
	std::string line;
	std::ifstream infile(path);

	if(infile.fail())
		throw XServManager::OpenFileException();

	std::cout << "Loading config from path " << path << std::endl;

    while (std::getline(infile, line))
	{
		if (line.length() && line[0] != '#')
		{
			if (line[0] == '!' && line.length() > 1)
			{
				_on_off_keycode = std::stoi(&line[1]);
				std::cout << "on/off switch kc: " << _on_off_keycode << std::endl;
			}
			else if(line[0] == '@' && line.length() > 4)
			{
				char trig;
				int i = 0;
				KeySym options[10];
				std::istringstream line_stream;

				trig = line[1];
    			line_stream.str(&line[3]);

				for (std::string bloc; std::getline(line_stream, bloc, ':'); )
				{
					options[i] = std::stoi(bloc, 0, 16);
					i++;
				}
				while(i < 10)
				{
					options[i] = NoSymbol;
					i++;
				}

				_add_input_remap(&trig,
							options[0],
							options[1],
							options[2],
							options[3],
							options[4],
							options[5],
							options[6],
							options[7],
							options[8],
							options[9]);

				std::cout << trig << ":" <<
							options[0]<< ":" <<
							options[1]<< ":" <<
							options[2]<< ":" <<
							options[3]<< ":" <<
							options[4]<< ":" <<
							options[5]<< ":" <<
							options[6]<< ":" <<
							options[7]<< ":" <<
							options[8]<< ":" <<
							options[9]<< std::endl;
			}


		}
    }
	std::cout << "Config loaded." << std::endl;
}

const char * XServManager::OpenDisplayFailException::what() const throw()
{
	return ("Unable to connect to X server");
}

const char * XServManager::GetKeycodeFailException::what() const throw()
{
	return ("Unable to obtain unused Keycode");
}

const char * XServManager::ConfigErrorException::what() const throw()
{
	return ("Invalid Configuration");
}

const char * XServManager::XGrabKeyException::what() const throw()
{
	return ("Unable to grab key events");
}

const char * XServManager::XUnGrabKeyException::what() const throw()
{
	return ("Unable to ungrab key events");
}

const char * XServManager::OpenFileException::what() const throw()
{
	return ("Unable to open config file");
}