#include "Ship.h"

Ship::Ship()
{
	speed.x = 0;
	speed.y = 0;
	moving = false;
}

void Ship::Init(int x, int y)
{
	static sf::Texture texture;
	if (!texture.loadFromFile(SHIP_FILENAME, sf::IntRect(x, y, SHIP_WIDTH, SHIP_HEIGHT)))
	{
		printf("error loading ship");
	}
	this->setTexture(texture);
	setOrigin(SHIP_WIDTH/2,SHIP_HEIGHT/2);
	setPosition(400,500);
}

void Ship::Update(float seconds)
{
	moving = false;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{	
		moving = true;
		speed.x += SHIP_ACCELERATION_X;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		moving = true;
		speed.y -= SHIP_ACCELERATION_Y;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		moving = true;
		speed.x -= SHIP_ACCELERATION_X;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		moving = true;
		speed.y += SHIP_ACCELERATION_Y;
	}

	//max speed constraints
	if(abs(speed.x) > SHIP_MAX_SPEED_X)
	{
		speed.x = speed.x > 0 ? SHIP_MAX_SPEED_X : -SHIP_MAX_SPEED_X;
	}
	if(abs(speed.y) > SHIP_MAX_SPEED_Y)
	{
		speed.y = speed.y > 0 ? SHIP_MAX_SPEED_Y : -SHIP_MAX_SPEED_Y;
	}

	//screen borders contraints	
	if (getPosition().x < SHIP_WIDTH/2)
		{
			setPosition(SHIP_WIDTH/2, getPosition().y);
			speed.x = 0;
		}

	if (getPosition().x > 800-(SHIP_WIDTH/2))
		{
			setPosition(800-(SHIP_WIDTH/2), getPosition().y);
			speed.x = 0;
		}

	if (getPosition().y < SHIP_HEIGHT/2)
		{
			setPosition(getPosition().x, SHIP_HEIGHT/2);
			speed.y = 0;
		}

	if (getPosition().y > 600-(SHIP_HEIGHT/2))
		{
			setPosition(getPosition().x, 600-(SHIP_HEIGHT/2));
			speed.y = 0;
		}
	
	//moving stuff
	this->setPosition(this->getPosition().x + (speed.x)*seconds, this->getPosition().y + (speed.y)*seconds);

	//idle decceleration
	if(!moving)
	{
		speed.x -= (speed.x)*seconds*(SHIP_DECCELERATION_COEF/100);
		speed.y -= (speed.y)*seconds*(SHIP_DECCELERATION_COEF/100);

		if(abs(speed.x) < SHIP_MIN_SPEED_X)
			speed.x = 0;

		if(abs(speed.y) < SHIP_MIN_SPEED_Y)
			speed.y = 0;

	

		
	}

	//printf("%f %f / %f \n", getPosition().x, getPosition().y, seconds );	
}

