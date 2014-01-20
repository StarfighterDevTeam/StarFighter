#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

ShipConfig::ShipConfig()
{
	this->max_speed.x = 10.0f;
	this->max_speed.y = 10.0f;
	this->decceleration = 200.0f;
	this->size.x = SHIP_WIDTH;
	this->size.y = SHIP_HEIGHT;
	this->textureName = SHIP_FILENAME;
	this->frameNumber = SHIP_NB_FRAMES;
}


void ShipConfig::Init(sf::Vector2f m_max_speed, float m_decceleration, std::string m_textureName, sf::Vector2f m_size, int m_frameNumber)
{
	this->max_speed.x = m_max_speed.x;
	this->max_speed.y = m_max_speed.y;
	this->decceleration = m_decceleration;
	this->size.x = m_size.x;
	this->size.y = m_size.y;
	this->textureName = m_textureName;
	this->frameNumber = m_frameNumber;
}


Ship::Ship(Vector2f position, ShipConfig m_ship_config) : Independant(position, Vector2f(0,0), m_ship_config.textureName, Vector2f(m_ship_config.size.x, m_ship_config.size.y), Vector2f((m_ship_config.size.x/2),(m_ship_config.size.y/2)), m_ship_config.frameNumber)
{
	this->collider_type =  IndependantType::PlayerShip;
	this->ship_config = m_ship_config;
	moving = false;	
	this->visible = true;
}

void Ship::setShipConfig(ShipConfig m_ship_config)
{
	this->ship_config = m_ship_config;
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
		weapon.setPosition(this->getPosition().x, (this->getPosition().y - (ship_config.size.y/2)) );
		//weapon.setPosition((animatedSprite.getPosition().x, animatedSprite.getPosition().y) - (SHIP_HEIGHT/2);
		weapon.Fire(FriendlyFire);

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
	if (this->getPosition().x < ship_config.size.x/2)
	{
		this->setPosition(ship_config.size.x/2, this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().x > WINDOW_RESOLUTION_X - (ship_config.size.x/2))
	{
		this->setPosition(WINDOW_RESOLUTION_X-(ship_config.size.x/2), this->getPosition().y);
		speed.x = 0;
	}

	if (this->getPosition().y < ship_config.size.y/2)
	{
		this->setPosition(this->getPosition().x, ship_config.size.y/2);
		speed.y = 0;
	}

	if (this->getPosition().y > WINDOW_RESOLUTION_Y-(ship_config.size.y/2))
	{
		this->setPosition(this->getPosition().x, WINDOW_RESOLUTION_Y-(ship_config.size.y/2));
		speed.y = 0;
	}

	//moving stuff
	//this->setPosition(this->getPosition().x + (speed.x)*deltaTime.asSeconds(), this->getPosition().y + (speed.y)*deltaTime.asSeconds());

	//idle decceleration
	if(!moving)
	{
		speed.x -= (speed.x)*deltaTime.asSeconds()*(ship_config.decceleration/100);
		speed.y -= (speed.y)*deltaTime.asSeconds()*(ship_config.decceleration/100);

		if(abs(speed.x) < SHIP_MIN_SPEED_X)
			speed.x = 0;

		if(abs(speed.y) < SHIP_MIN_SPEED_Y)
			speed.y = 0;
	}

	Independant::update(deltaTime);
}

