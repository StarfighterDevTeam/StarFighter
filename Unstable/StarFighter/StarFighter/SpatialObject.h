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
	SpatialObject();
	virtual ~SpatialObject();

	void Update(sf::Time deltaTime) override;
	virtual void SetHostility(Hostility hostility);
	virtual bool CheckMarkingConditions();
	void SetROE(RuleOfEngagement roe);
	bool IsMarked() override;

	Marker* m_marker_target;
	Marker* m_marker_mission;
	Hostility m_hostility;
	RuleOfEngagement m_roe;

	sf::Text m_id_text;
};

#endif // SPATIALOBJECT_H_INCLUDED
