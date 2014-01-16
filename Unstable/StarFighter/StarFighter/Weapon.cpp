#include "Weapon.h"


Weapon::Weapon()
{
	speed.x = 0;
	speed.y = 0;
	firing_ready = true;
	index = 0;
}
/*
void Weapon::Init(int x, int y)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(LASER_FILENAME,LASER_WIDTH,LASER_HEIGHT);

	this->setTexture(*texture);
	setOrigin(LASER_WIDTH/2, LASER_HEIGHT/2);
}
*/
void Weapon::Fire()
{
	if (deltaClock.getElapsedTime() > sf::seconds(LASER_RATE_OF_FIRE))
		{
			firing_ready = true;
		}

		if (firing_ready)
		{
			if (index > (LASER_MAX_AMMO_PER_STAGE-1))
			{
				index = 0;
			}
			printf("index tir/10 : %d \n", index);
			ammo[index].setPosition(getPosition().x, getPosition().y - (LASER_HEIGHT/2));
			index++;
			deltaClock.restart();
			firing_ready = false;		
		}
}

void Weapon::Update(sf::Time deltaTime)
{
	for (int i=0; i<LASER_MAX_AMMO_PER_STAGE; i++)
	{
		ammo[i].Update(deltaTime);
	}
}


