
#include "InputGuy.h"

bool InputGuy::isFiring()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		return true;
	}

	if (sf::Joystick::isConnected(0))
	{
		if (sf::Joystick::hasAxis(0, sf::Joystick::Axis::Z))
		{
			if (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z) < -JOYSTICK_MIN_AXIS_VALUE)
			{
				return true;
			}
		}
		if (sf::Joystick::isButtonPressed(0, 0))
		{
			return true;
		}
	}
	return false;
}

bool InputGuy::isBraking()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
	{
		return true;
	}

	if (sf::Joystick::isConnected(0))
	{
		if (sf::Joystick::isButtonPressed(0, 2))
			return true;
	}

	return false;
}

bool InputGuy::setAutomaticFire()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		return true;
	}

	if (sf::Joystick::isConnected(0))
	{
		if (sf::Joystick::isButtonPressed(0, 5))
			return true;
	}

	return false;
}

Vector2f InputGuy::getDirections()
{
	short dirX = 0;
	short dirY = 0;


	//Keyboard inputs
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

	//Conputing directions
	static float x;
	static float y;

	x = dirX > 0 ? 1 : (dirX < 0 ? -1 : 0);
	y = dirY > 0 ? 1 : (dirY < 0 ? -1 : 0);

	//Joystick inputs (if connected)
	if (sf::Joystick::isConnected(0))
	{
		if (abs(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X)) > JOYSTICK_MIN_AXIS_VALUE)
			x = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) / 100.0;
		else if (abs(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX)) > JOYSTICK_MIN_AXIS_VALUE)
			x = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) / 100.0;
		
		if (abs(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y)) > JOYSTICK_MIN_AXIS_VALUE)
			y = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) / 100.0;
		else if (abs(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY)) > JOYSTICK_MIN_AXIS_VALUE)
			y = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) / 100.0;
	}

	//diagonal movement?
	if (abs(x) + abs(y) > 1)
	{
		float p = (1 / sqrt((x*x) + (y*y)));
		x = x*p;
		y = y*p;
	}

	return Vector2f(x, y);
}