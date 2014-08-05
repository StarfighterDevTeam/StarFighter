
#include "InputGuy.h"

bool InputGuy::isFiring()
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		return true;
	}

	if(sf::Joystick::hasAxis(0,sf::Joystick::Axis::R))
	{
		if(sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::R) > 5)
		{
			return true;
		}
	}
	return false;
}

bool InputGuy::isBraking()
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
	{
		return true;
	}

	else
		return false;
}

bool InputGuy::setAutomaticFire()
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		return true;
	}	
		
	else
		return false;
}

Vector2f InputGuy::getDirections()
{
	short dirX = 0;
	short dirY = 0;


	//Keyboard inputs
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		dirX++;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		dirY--;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		dirX--;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		dirY++;
	}

	//Joystick inputs (if connected)
	if(sf::Joystick::hasAxis(0,sf::Joystick::Axis::PovX))
	{
		if(sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovX) > 1)
		{	
			dirX++;
		}

		if(sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovX) < 1)
		{
			dirX--;
		}

		if(sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovY) > 1)
		{
			dirY--;
		}

		if(sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovY) < 1)
		{
			dirY++;
		}
	}

	//Conputing directions
	static float x;
	static float y;

	x = dirX > 0 ? 1 : (dirX < 0 ? -1 : 0);
	y = dirY > 0 ? 1 : (dirY < 0 ? -1 : 0);

	//diagonal movement ? 
	if( abs(x) == 1 && abs(y) == 1)
	{
		x = (x)*SQRT2BY2;
		y = (y)*SQRT2BY2;
	}

	return Vector2f(x,y);
}