#ifndef SPATIALOBJECT_H_INCLUDED
#define SPATIALOBJECT_H_INCLUDED

#include "Game.h"

class Marker;

enum HostilityLevel
{
	Hostility_Ally,
	Hostility_HoldFire,
	Hostility_ReturnFire,
	Hostility_FireAtWill,
	NB_HOSTILITY_LEVELS,
};

class SpatialObject : public GameObject
{
public :
	SpatialObject(){};
	SpatialObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~SpatialObject();

	void Update(sf::Time deltaTime) override;
	virtual void SetHostility(HostilityLevel hostility);

	Marker* m_marker;
	HostilityLevel m_hostility;

};

#endif // SPATIALOBJECT_H_INCLUDED
