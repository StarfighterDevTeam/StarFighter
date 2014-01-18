#include "Ship.h"

extern Game* CurrentGame;


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

void Ship::getShipConfig(ShipConfig m_ship_config)
{
	ship_config = m_ship_config;
	max_speed.x = ship_config.max_speed.x;
	max_speed.y = ship_config.max_speed.y;
}

Ship::Ship()
{
	speed.x = 0;
	speed.y = 0;
	moving = false;	
	this->animatedSprite = AnimatedSprite(sf::seconds(SHIP_SPRITE_RATE_SEC), false, true);
	this->animatedSprite.setOrigin(SHIP_WIDTH/2, SHIP_HEIGHT/2);
}

void Ship::Init(int x, int y)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(SHIP_FILENAME,192,64);

	//Loading ship animation
	static Animation shipDefaultAnimation;
	shipDefaultAnimation.setSpriteSheet(*texture);
	shipDefaultAnimation.addFrame(sf::IntRect(0, 0, 64, 64));
    shipDefaultAnimation.addFrame(sf::IntRect(64, 0, 64, 64));
    shipDefaultAnimation.addFrame(sf::IntRect(128, 0, 64, 64));

	Animation* currentAnimation = &shipDefaultAnimation;
    this->animatedSprite.setPosition((float)x,(float)y);
	this->animatedSprite.play(*currentAnimation);
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

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		weapon.setPosition(animatedSprite.getPosition().x, (animatedSprite.getPosition().y - (SHIP_HEIGHT/2)) );
		//weapon.setPosition((animatedSprite.getPosition().x, animatedSprite.getPosition().y) - (SHIP_HEIGHT/2);
		weapon.Fire();
		
	}

	//max speed constraints
	if(abs(speed.x) > max_speed.x)
	{
		speed.x = speed.x > 0 ? max_speed.x : -max_speed.x;
	}
	if(abs(speed.y) > max_speed.y)
	{
		speed.y = speed.y > 0 ? max_speed.y : -max_speed.y;
	}

	//screen borders contraints	
	if (animatedSprite.getPosition().x < SHIP_WIDTH/2)
		{
			animatedSprite.setPosition(SHIP_WIDTH/2, animatedSprite.getPosition().y);
			speed.x = 0;
		}

	if (animatedSprite.getPosition().x > WINDOW_RESOLUTION_X - (SHIP_WIDTH/2))
		{
			animatedSprite.setPosition(WINDOW_RESOLUTION_X-(SHIP_WIDTH/2), animatedSprite.getPosition().y);
			speed.x = 0;
		}

	if (animatedSprite.getPosition().y < SHIP_HEIGHT/2)
		{
			animatedSprite.setPosition(animatedSprite.getPosition().x, SHIP_HEIGHT/2);
			speed.y = 0;
		}

	if (animatedSprite.getPosition().y > WINDOW_RESOLUTION_Y-(SHIP_HEIGHT/2))
		{
			animatedSprite.setPosition(animatedSprite.getPosition().x, WINDOW_RESOLUTION_Y-(SHIP_HEIGHT/2));
			speed.y = 0;
		}
	
	//moving stuff
	animatedSprite.setPosition(animatedSprite.getPosition().x + (speed.x)*deltaTime.asSeconds(), animatedSprite.getPosition().y + (speed.y)*deltaTime.asSeconds());

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

	animatedSprite.update(deltaTime);

	//printf("%f %f / %f \n", animatedSprite.getPosition().x, animatedSprite.getPosition().y, deltaTime.asSeconds() );	
}

