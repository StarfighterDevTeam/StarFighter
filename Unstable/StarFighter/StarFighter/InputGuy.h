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
	KeyboardControl2,
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

	static bool isSpawningModule1(ControlerType device = AllControlDevices);
	static bool isSpawningModule2(ControlerType device = AllControlDevices);
	static bool isSpawningModule3(ControlerType device = AllControlDevices);
	static bool isSpawningModule4(ControlerType device = AllControlDevices);
	static bool isSpawningModule5(ControlerType device = AllControlDevices);
	static bool isSpawningModule6(ControlerType device = AllControlDevices);
	static bool isSpawningModule7(ControlerType device = AllControlDevices);
	static bool isSpawningModule8(ControlerType device = AllControlDevices);
	static bool isSpawningModule9(ControlerType device = AllControlDevices);
	static bool isSpawningModule0(ControlerType device = AllControlDevices);
	static bool isSellingModule(ControlerType device = AllControlDevices);
	static bool isFinishModuleConstruction(ControlerType device = AllControlDevices);
	static bool isRefillingFlux(ControlerType device = AllControlDevices);
};


#endif // INPUTGUY_H_INCLUDED