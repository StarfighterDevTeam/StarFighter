#include "SpatialObject.h"

extern Game* CurrentGame;

using namespace sf;

SpatialObject::SpatialObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	
}

SpatialObject::~SpatialObject()
{
	
}

void SpatialObject::Update(sf::Time deltaTime)
{
	GameObject::Update(deltaTime);
}

void SpatialObject::SetHostility(HostilityLevel hostility)
{
	m_hostility = hostility;
	m_collider = hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject;
	m_marker->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);
}