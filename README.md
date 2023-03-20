# accent_menu_X11

## TL;DR:
You don't have a mac but like being able to use accents on a qwerty layout ? this is for you

## description:
accent_menu_X11 aims to reproduce the behavior of MacOS's accent menu bar.
accent_menu_X11 allows you to change the output of a chosen key combo.

## key differences with MacOS's accent menu
no display of the menu bar. (for now, see TODO)
has to be turned OFF manually for terminal use. (for now, see TODO)
no delay before starting accent mode.
can modify trigger keys and output.

## usage:
(the following example is based on accent_menu_X11.example.conf)
ScrollLock is used to turn ON/OFF the remapping.
hit **1** while holding **e** will output **é**
#### nomentlature:
in above example:
* **e** is trigger key
* **1** is option key
* **é** is wanted output

## configuration file:
lines starting with # are ignored
line starting with ! indicate ON/OFF keycode
lines starting with @ are remaps
format @char:keysym1:keysym2:keysym3 (...) keysym9:keysym0
char is the trigger key
char must be [a-z] or [A-Z]
keysym must be hex from keysymdef.h
see https://github.com/D-Programming-Deimos/libX11/blob/master/c/X11/keysymdef.h  for details
keysym in order of option keys 1234567890
separator is :
if you want to add an empty option key, set it to 0x0
#### example
@A:0x00c1:0x0:0x00c2

## how does it work ?:
The program scans the keyboard's keycodes to find an unused one and borrows it.
The program listens to events on the keys you have chosen as triggers in the configuration file.
When such an event occurs, the program listens to the options keys events.
If an option key is pressed, the associated keysym from the configuration file is assigned to the borrowed keycode.
An event saying you pressed the borrowed keycode is sent.

## limitations and known issues:
* Does not work in terminals (this is by design on the terminal side for security purposes)(current workaround: turn if off manually with the ON/OFF keycode)
* Trigger keys are limited to [a-z] or [A-Z]
* Chosen output must be a valid keysym from X11/keysymdef.h (NO UNICODE)
* Chosen output must be of the same case, for example, you cannot remap e + 1 to output É and must remap an uppercase letter (for example E) to output the upper case character É
* if the process is SIGKILL'd (instead of SIGTERM or SIGINT) the borrowed keycode is not properly set back to NoSymbol (resolves itself upon reboot, but if you were to kill it enough times (40+ but dependant on system) without restarting you would end up unable to run the program )

## TODO:
* Find a way to either detect if the currently focused window is a terminal or if the current window refuses sent events. Disable/Enable functionnality automatically.
* Display the menu bar (and ON/OFF feedback if previous TODO isn't done by then)
* add support for more trigger keys/ keyboard layouts

## HELP WANTED:
* Need infos on how some input modes work for various languages (stuff like Hangul/Henkan modes, i have no idea how those work and thus cannot make my program compatible with those for now)
* Tell me what kind of functionality would make this program more usefull to you.
* Any help on the TODO list

## License:
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Also known as: i don't care what you do with it but don't complain if it breaks your computer.
