#include "Weapon.h"


Weapon::Weapon()
{
	speed.x = 0;
	speed.y = 0;
}

void Weapon::Init(int x, int y)
{
	static sf::Texture texture;
	if (!texture.loadFromFile(LASER_FILENAME, sf::IntRect(x, y, LASER_WIDTH, LASER_HEIGHT)))
	{
		printf("error loading ship");
	}
	this->setTexture(texture);
	setOrigin(LASER_WIDTH/2, LASER_HEIGHT/2);
}

void Weapon::Update(float seconds)
{
	speed.y = -500;
	this->setPosition(this->getPosition().x + (speed.x)*seconds, this->getPosition().y + (speed.y)*seconds);
}