#include "SEInputs.h"

SEInputs* InputsManager = NULL;

bool SEInputs::isRestartingScript()
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

bool SEInputs::isUsingDebugCommand()
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

bool SEInputs::isChangingResolution()
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

bool SEInputs::isMuting()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		return true;
	}

	return false;
}

bool SEInputs::isPausing()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		return true;
	}

	return false;
}

Vector2f SEInputs::getDirections(ControlerType device)
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
			if (fabs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::X)) > JOYSTICK_MIN_AXIS_VALUE)
				x = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::X) / 100.0f;
			else if (fabs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovX)) > JOYSTICK_MIN_AXIS_VALUE)
				x = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovX) / 100.0f;

			if (fabs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::Y)) > JOYSTICK_MIN_AXIS_VALUE)
				y = sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::Y) / 100.0f;
			else if (fabs(sf::Joystick::getAxisPosition(joystick, sf::Joystick::Axis::PovY)) > JOYSTICK_MIN_AXIS_VALUE)
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