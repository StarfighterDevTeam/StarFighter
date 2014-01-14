#include "Weapon.h"


Weapon::Weapon()
{
	speed.x = 0;
	speed.y = 0;
}

void Weapon::Init(int x, int y)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(LASER_FILENAME,LASER_WIDTH,LASER_HEIGHT);

	this->setTexture(*texture);
	setOrigin(LASER_WIDTH/2, LASER_HEIGHT/2);
}

void Weapon::Update(float seconds)
{
	speed.y = -500;
	this->setPosition(this->getPosition().x + (speed.x)*seconds, this->getPosition().y + (speed.y)*seconds);
}