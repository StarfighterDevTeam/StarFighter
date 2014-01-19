#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

ShipConfig::ShipConfig()
{
	this->max_speed.x = 10.0f;
	this->max_speed.y = 10.0f;
	this->texturename = SHIP_FILENAME;
}


void ShipConfig::Init(sf::Vector2f m_max_speed, std::string m_texturename)
{
	this->max_speed.x = m_max_speed.x;
	this->max_speed.y = m_max_speed.y;
	this->texturename = m_texturename;
}


Ship::Ship(Vector2f position, ShipConfig m_ship_config) : Independant(position, Vector2f(0,0), m_ship_config.texturename, Vector2f(192,64),Vector2f(32,32),3)
{
	this->collider_type = 1;
	this->ship_config = m_ship_config;
	moving = false;	
	this->visible = true;
	//this->animatedSprite = AnimatedSprite(sf::seconds(SHIP_SPRITE_RATE_SEC), false, true);
	//this->animatedSprite.setOrigin(SHIP_WIDTH/2, SHIP_HEIGHT/2);
}

void Ship::setShipConfig(ShipConfig m_ship_config)
{
	this->ship_config = m_ship_config;
}

void Ship::Init(int x, int y)
{
	//this->shipSprite = new Independant(Vector2f(x,y), Vector2f(speed.x,speed.y), SHIP_FILENAME, Vector2f(192,64),Vector2f(32,32),3);
	//(*CurrentGame).addToScene(shipSprite,7);

	/*
	//Loading ship animation
	static Animation shipDefaultAnimation;
	shipDefaultAnimation.setSpriteSheet(*texture);
	shipDefaultAnimation.addFrame(sf::IntRect(0, 0, 64, 64));
	shipDefaultAnimation.addFrame(sf::IntRect(64, 0, 64, 64));
	shipDefaultAnimation.addFrame(sf::IntRect(128, 0, 64, 64));

	Animation* currentAnimation = &shipDefaultAnimation;
	this->animatedSprite.setPosition((float)x,(float)y);
	this->animatedSprite.play(*currentAnimation);*/
}

void Ship::update(sf::Time deltaTime)
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

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		weapon.setPosition(this->getPosition().x, (this->getPosition().y - (SHIP_HEIGHT/2)) );
		//weapon.setPosition((animatedSprite.getPosition().x, animatedSprite.getPosition().y) - (SHIP_HEIGHT/2);
		weapon.Fire();

	}

	//max speed constraints
	if(abs(speed.x) > this->ship_config.max_speed.x)
	{
		speed.x = speed.x > 0 ?  this->ship_config.max_speed.x : - this->ship_config.max_speed.x;
	}
	if(abs(speed.y) >  this->ship_config.max_speed.y)
	{
		speed.y = speed.y > 0 ?  this->ship_config.max_speed.y : - this->ship_config.max_speed.y;
	}

	//screen borders contraints	
	if (this->getPosition().x < SHIP_WIDTH/2)
	{
		this->setPosition(SHIP_WIDTH/2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > WINDOW_RESOLUTION_X - (SHIP_WIDTH/2))
	{
		this->setPosition(WINDOW_RESOLUTION_X-(SHIP_WIDTH/2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < SHIP_HEIGHT/2)
	{
		this->setPosition(this->getPosition().x, SHIP_HEIGHT/2);
		speed.y = 0;
	}

	if (this->getPosition().y > WINDOW_RESOLUTION_Y-(SHIP_HEIGHT/2))
	{
		this->setPosition(this->getPosition().x, WINDOW_RESOLUTION_Y-(SHIP_HEIGHT/2));
		speed.y = 0;
	}

	//moving stuff
	//this->setPosition(this->getPosition().x + (speed.x)*deltaTime.asSeconds(), this->getPosition().y + (speed.y)*deltaTime.asSeconds());

	//idle decceleration
	if(!moving)
	{
		speed.x -= (speed.x)*deltaTime.asSeconds()*(SHIP_DECCELERATION_COEF/100);
		speed.y -= (speed.y)*deltaTime.asSeconds()*(SHIP_DECCELERATION_COEF/100);

		if(abs(speed.x) < SHIP_MIN_SPEED_X)
			speed.x = 0;

		if(abs(speed.y) < SHIP_MIN_SPEED_Y)
			speed.y = 0;
	}

	Independant::update(deltaTime);
}

