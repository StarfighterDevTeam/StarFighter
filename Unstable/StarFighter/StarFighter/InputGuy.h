#ifndef INPUTGUY_H_INCLUDED
#define INPUTGUY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include "Globals.h"

#define SQRT2BY2 0.707 // racine(2) / 2

using namespace sf;

enum ControlerType
{
	AllControlDevices,
	KeyboardControl,
	JoystickControl1,
	JoystickControl2,
	JoystickControl3,
	JoystickControl4,
};

class InputGuy
{
public:
	static Vector2f getDirections(ControlerType device = AllControlDevices);
	static bool isSelecting(ControlerType device = AllControlDevices);
	static bool isAssigning(ControlerType device = AllControlDevices);
	static bool isChangingResolution();
	static bool isUsingDebugCommand();
	static bool isRestartingScript();
	static bool isMuting();
	static bool isPausing();
};


#endif // INPUTGUY_H_INCLUDED