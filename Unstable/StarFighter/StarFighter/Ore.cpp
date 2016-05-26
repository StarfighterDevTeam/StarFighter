#include "Ore.h"

extern Game* CurrentGame;

using namespace sf;

Ore::Ore(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

Ore::~Ore()
{

}

void Ore::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}