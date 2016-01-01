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
	static bool isFiring(ControlerType device = AllControlDevices);
	static bool isTackling(ControlerType device = AllControlDevices);
	static bool isUsing(ControlerType device = AllControlDevices);
	static bool isChangingResolution();
	static bool isUsingDebugCommand();
	static bool isRestartingScript();

	static bool isSpawningModule1();
	static bool isSpawningModule2();
	static bool isSpawningModule3();
	static bool isSpawningModule4();
	static bool isSpawningModule5();
	static bool isSpawningModule6();
	static bool isSpawningModule7();
	static bool isSpawningModule8();
	static bool isSpawningModule9();
	static bool isSpawningModule0();
	static bool isErasingModule();
};


#endif // INPUTGUY_H_INCLUDED