#ifndef INPUTGUY_H_INCLUDED
#define INPUTGUY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include "Globals.h"

#define SQRT2BY2 0.707 // racine(2) / 2

using namespace sf;

class InputGuy
{
public:
	static Vector2f getDirections();
	static bool isFiring();
	static bool isBraking();
	static bool isHyperspeeding();
	static bool isSlowMotion();
	static bool setAutomaticFire();
	static bool isChangingResolution();
	static bool isOpeningHud();
};


#endif // INPUTGUY_H_INCLUDED