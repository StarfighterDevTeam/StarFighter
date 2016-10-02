#ifndef SEINPUTS_H_INCLUDED
#define SEINPUTS_H_INCLUDED

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

class SEInputs;
extern SEInputs* InputsManager;

class SEInputs
{
public:
	Vector2f getDirections(ControlerType device = AllControlDevices);
	bool isChangingResolution();
	bool isUsingDebugCommand();
	bool isRestartingScript();
	bool isMuting();
	bool isPausing();
};

#endif // SEINPUTS_H_INCLUDED
