#include "Starship.h"

extern Game* CurrentGame;

using namespace sf;

Starship::Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

Starship::~Starship()
{

}

void Starship::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}