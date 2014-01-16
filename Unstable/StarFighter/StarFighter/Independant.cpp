#include "Independant.h"


Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin)  : Sprite()
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y);

	this->setOrigin(origin.x, origin.y);
	this->setTexture(*texture);
	this->speed = speed;
	this->setPosition(position.x,position.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Sprite()
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	sf::Texture* texture = loader->loadTexture(textureName, size.x, size.y);

	this->setOrigin(size.x/2, size.y/2);
	this->setTexture(*texture);
	this->speed = speed;
	this->setPosition(position.x,position.y);
}

Independant::Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture) : Sprite()
{
	this->setTexture(*texture);
	this->speed = speed;
	this->setPosition(position.x,position.y);
}

void Independant::update(sf::Time deltaTime)
{
	float x = this->getPosition().x + (this->speed.x)*deltaTime.asSeconds();
	float y = this->getPosition().y + (this->speed.y)*deltaTime.asSeconds();
	this->setPosition(x,y);
}