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
	JoystickControl,
};

class InputGuy
{
public:
	static Vector2f getDirections(ControlerType device = AllControlDevices);
	static bool isFiring(ControlerType device = AllControlDevices);
	static bool isBraking(ControlerType device = AllControlDevices);
	static bool isSwitchingRotation(ControlerType device = AllControlDevices);
	static bool isSlowMotion();
	static bool setAutomaticFire();
	static bool isChangingResolution();
	static bool isOpeningHud();
	static bool isUsingDebugCommand();
};


#endif // INPUTGUY_H_INCLUDED