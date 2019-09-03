#ifndef SPATIALOBJECT_H_INCLUDED
#define SPATIALOBJECT_H_INCLUDED

#include "Game.h"

class Marker;

enum Hostility
{
	Hostility_Ally,
	Hostility_Enemy,
	NB_HOSTILITY_TYPES,
};

enum RuleOfEngagement
{
	ROE_HoldFire,
	ROE_ReturnFire,
	ROE_FireAtWill,
	NB_ROE_TYPES,
};

class SpatialObject : public GameObject
{
public :
	SpatialObject(){};
	SpatialObject(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~SpatialObject();

	void Update(sf::Time deltaTime) override;
	virtual void SetHostility(Hostility hostility);

	Marker* m_marker;
	Hostility m_hostility;
	RuleOfEngagement m_roe;

	sf::Text m_id_text;
};

#endif // SPATIALOBJECT_H_INCLUDED
