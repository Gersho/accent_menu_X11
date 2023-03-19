#include "../include/accent_menu.h"

XServManager *manager = NULL;

void show_version_message()
{
	std::cout << "accent_menu_x11 " << ACCENT_MENU_VERSION << std::endl;
	std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
	std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
	std::cout << "Written by Karim Zennoune." << std::endl;
	exit(0);
}

void show_help_message()
{
	std::cout << "usage: ./accent_menu_X11 [CONFIG FILE] " << std::endl;
	std::cout << "remaps key combos based on config file" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "--help    display this help and exit" << std::endl;
	std::cout << "--version  output version information and exit" << std::endl;
	exit(0);
}

void signal_handler(int n)
{
    write(manager->pip[1], &manager->done, 1);  // wake up select in X11 event loop 
	if(n == SIGINT || n == SIGTERM)
		manager->done = 1;
}

void signal_init()
{ 
    struct sigaction sigact;        // no race conditions like signal()
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = &signal_handler;
    sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
    memset(&manager->pip, 0, sizeof(manager->pip));
	memset(&manager->done, 0, sizeof(manager->done));
    pipe(manager->pip);
    write(manager->pip[1], &manager->done, 1);  // wake up select on first iteration of X11 event loop
}

int main(int ac, char** av)
{
	if(ac != 2)
	{
		std::cout << "Wrong usage, see --help" << std::endl;
		return -1;
	}

	if(strcmp("--help", av[1]) == 0)
		show_help_message();
	if(strcmp("--version", av[1]) == 0)
		show_version_message();

	try
	{
		manager = new XServManager;
		signal_init();
		manager->load_config_from_file(av[1]);
		manager->start();
	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

    close(manager->pip[0]);
    close(manager->pip[1]);
	delete manager;
	return 0;
}
