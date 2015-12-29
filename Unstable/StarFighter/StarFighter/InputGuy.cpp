#include "InputGuy.h"

bool InputGuy::isFiring(ControlerType device)
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

bool InputGuy::isSwitchingRotation(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
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
			if (sf::Joystick::isButtonPressed(joystick, 3))// Y button
				return true;
		}
	}

	return false;
}

bool InputGuy::isTackling(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
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
			if (sf::Joystick::isButtonPressed(joystick, 2)) // X button
				return true;
		}
	}

	return false;
}

bool InputGuy::isRestartingScript()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
	{
		return true;
	}

	if (sf::Joystick::isConnected(0))
	{
		if (sf::Joystick::isButtonPressed(0, 4)) //Upper left trigger
			return true;
	}

	return false;
}

bool InputGuy::isUsingDebugCommand()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
	{
		return true;
	}

	if (sf::Joystick::isConnected(0))
	{
		if (sf::Joystick::isButtonPressed(0, 6)) //Select button
			return true;
	}

	return false;
}

bool InputGuy::isChangingResolution()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
	{
		return true;
	}

	if (sf::Joystick::isConnected(0))
	{
		if (sf::Joystick::isButtonPressed(0, 7)) //Start button
			return true;
	}

	return false;
}

Vector2f InputGuy::getDirections(ControlerType device)
{
	short dirX = 0;
	short dirY = 0;

	//Keyboard inputs
	if (device == AllControlDevices || device == KeyboardControl)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			dirX++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			dirY--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			dirX--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			dirY++;
		}
	}

	//Conputing directions
	static float x;
	static float y;

	x = dirX > 0 ? 1 : (dirX < 0 ? -1.f : 0.f);
	y = dirY > 0 ? 1 : (dirY < 0 ? -1.f : 0.f);

	//Joystick inputs (if connected)
	if (device == AllControlDevices || device >= JoystickControl1)
	{
		int joystick = device - JoystickControl1;
		if (device == AllControlDevices)
			joystick = 0;// = joystick 1

		if (sf::Joystick::isConnected(joystick))
		{
			if (abs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::X)) > JOYSTICK_MIN_AXIS_VALUE)
				x = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::X) / 100.0f;
			else if (abs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovX)) > JOYSTICK_MIN_AXIS_VALUE)
				x = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovX) / 100.0f;

			if (abs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::Y)) > JOYSTICK_MIN_AXIS_VALUE)
				y = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::Y) / 100.0f;
			else if (abs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovY)) > JOYSTICK_MIN_AXIS_VALUE)
				y = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovY) / 100.0f;
		}
	}

	//diagonal movement?
	if (x*x + y*y > 1)
	{
		float p = (1 / sqrt((x*x) + (y*y)));
		x *= p;
		y *= p;
	}

	return Vector2f(x, y);
}