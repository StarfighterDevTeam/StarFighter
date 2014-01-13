#include "Ship.h"

Ship::Ship()
{
	speed.x = 0;
	speed.y = 0;
	moving = false;

	super::AnimatedSprite(sf::seconds(SHIP_SPRITE_RATE_SEC), false, true);
}

void Ship::Init(int x, int y)
{
	static sf::Texture texture;
	if (!texture.loadFromFile(SHIP_FILENAME, sf::IntRect(x, y, 192, 64)))
	{
		printf("error loading ship");
	}
	//Loading ship animation
	animation.setSpriteSheet(texture);
	animation.addFrame(sf::IntRect(0, 0, 64, 64));
    animation.addFrame(sf::IntRect(64, 0, 64, 64));
    animation.addFrame(sf::IntRect(128, 0, 64, 64));

    this->setPosition(x,y);
	this->play(animation);
}

void Ship::Update(sf::Time deltaTime)
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

	if(abs(speed.x) > SHIP_MAX_SPEED_X)
	{
		speed.x = speed.x > 0 ? SHIP_MAX_SPEED_X : -SHIP_MAX_SPEED_X;
	}
	if(abs(speed.y) > SHIP_MAX_SPEED_Y)
	{
		speed.y = speed.y > 0 ? SHIP_MAX_SPEED_Y : -SHIP_MAX_SPEED_Y;
	}

	this->setPosition(this->getPosition().x + (speed.x)*deltaTime.asSeconds(), this->getPosition().y + (speed.y)*deltaTime.asSeconds());

	if(!moving)
	{
		speed.x -= (speed.x)*deltaTime.asSeconds()*(SHIP_DECCELERATION_COEF/100);
		speed.y -= (speed.y)*deltaTime.asSeconds()*(SHIP_DECCELERATION_COEF/100);

		if(abs(speed.x) < SHIP_MIN_SPEED_X)
			speed.x = 0;

		if(abs(speed.y) < SHIP_MIN_SPEED_Y)
			speed.y = 0;
	}

	super::update(deltaTime);

	printf("%f %f / %f \n", this->getPosition().x, this->getPosition().y, deltaTime.asSeconds() );	
}

