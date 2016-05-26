#include "Location.h"

extern Game* CurrentGame;

using namespace sf;

Location::Location(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

Location::~Location()
{

}

void Location::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}