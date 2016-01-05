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

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Return);
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

bool InputGuy::isUsing(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		{
			return true;
		}
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			dirX++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			dirY--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			dirX--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			dirY++;
		}
	}

	else if (device == KeyboardControl2)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			dirX++;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			dirY--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			dirX--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
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

//DEBUG - FLUX SPECIFIC
bool InputGuy::isSpawningModule1(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1);
	}

	return false;
}
bool InputGuy::isSpawningModule2(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2);
	}

	return false;
}
bool InputGuy::isSpawningModule3(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3);
	}

	return false;
}
bool InputGuy::isSpawningModule4(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4);
	}

	return false;
}
bool InputGuy::isSpawningModule5(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num5);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5);
	}

	return false;
}
bool InputGuy::isSpawningModule6(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num6);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6);
	}

	return false;
}
bool InputGuy::isSpawningModule7(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num7);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7);
	}

	return false;
}
bool InputGuy::isSpawningModule8(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num8);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8);
	}

	return false;
}
bool InputGuy::isSpawningModule9(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num9);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9);
	}

	return false;
}
bool InputGuy::isSpawningModule0(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Num0);
	}

	else if (device == KeyboardControl2)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0);
	}

	return false;
}

bool InputGuy::isErasingModule(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract);
	}
	return false;
}

bool InputGuy::isFinishModuleConstruction(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Add);
	}
	return false;
}

bool InputGuy::isRefillingFlux(ControlerType device)
{
	if (device == AllControlDevices || device == KeyboardControl)
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply);
	}
	return false;
}