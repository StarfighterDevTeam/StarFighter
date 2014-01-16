#include "Ship.h"

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
    this->animatedSprite.setPosition(x,y);
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
	if(abs(speed.x) > SHIP_MAX_SPEED_X)
	{
		speed.x = speed.x > 0 ? SHIP_MAX_SPEED_X : -SHIP_MAX_SPEED_X;
	}
	if(abs(speed.y) > SHIP_MAX_SPEED_Y)
	{
		speed.y = speed.y > 0 ? SHIP_MAX_SPEED_Y : -SHIP_MAX_SPEED_Y;
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

	for (int i=0; i<LASER_MAX_AMMO_PER_STAGE; i++)
	{
		weapon.ammo[i].Update(deltaTime);
	}

	//printf("%f %f / %f \n", animatedSprite.getPosition().x, animatedSprite.getPosition().y, deltaTime.asSeconds() );	
}
