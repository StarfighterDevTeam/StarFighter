#include "SpatialObject.h"

extern Game* CurrentGame;

using namespace sf;

SpatialObject::SpatialObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

SpatialObject::~SpatialObject()
{
	delete m_marker;
}

void SpatialObject::Update(sf::Time deltaTime)
{
	GameObject::Update(deltaTime);
}

void SpatialObject::SetHostility(Hostility hostility)
{
	m_hostility = hostility;
}