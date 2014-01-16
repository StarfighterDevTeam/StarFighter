#include "Ammo.h"

Ammo::Ammo()
{
	speed.x = 0;
	speed.y = -500;
}

void Ammo::Init(int x, int y)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(LASER_FILENAME, LASER_WIDTH, LASER_HEIGHT);

	this->setTexture(*texture);
	setOrigin(LASER_WIDTH/2, LASER_HEIGHT/2);
}


void Ammo::Update(sf::Time deltaTime)
{	
	this->setPosition(this->getPosition().x + (speed.x)*deltaTime.asSeconds(), this->getPosition().y + (speed.y)*deltaTime.asSeconds());
}
