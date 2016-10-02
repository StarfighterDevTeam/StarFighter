#pragma once

#include "Inputs.h"

bool Inputs::isFiring(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			return true;
		}
	}

	if (device == AllControlDevices || device >= JoystickControl1)
	{
		int joystick = device - JoystickControl1;
		if (device == AllControlDevices)
			joystick = 0;// = joystick 1

		if (sf::Joystick::isConnected(joystick))
		{
			if (sf::Joystick::hasAxis(joystick, sf::Joystick::Axis::Z))
			{
				if (sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::Z) < -JOYSTICK_MIN_AXIS_VALUE) // right trigger
				{
					return true;
				}
			}
			if (sf::Joystick::isButtonPressed(joystick, 0)) // A button
			{
				return true;
			}
		}
	}

	return false;
}
