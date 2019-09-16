#ifndef SPATIALOBJECT_H_INCLUDED
#define SPATIALOBJECT_H_INCLUDED

#include "FX.h"

class Marker;

enum Hostility
{
	Hostility_Ally,
	Hostility_Enemy,
	NB_HOSTILITY_TYPES,
};

enum RuleOfEngagement
{
	ROE_Freeze,
	ROE_HoldFire,
	ROE_ReturnFire,
	ROE_Ambush,
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
	bool IsMarked() override;
	SpatialObject* GetTargetableEnemyShip(const float dist_max, const float angle_delta_max);
	virtual void SetROE(RuleOfEngagement roe);
	virtual AIShip* GetEscortedShip();

	Marker* m_marker_target;
	Marker* m_marker_mission;
	Hostility m_hostility;
	RuleOfEngagement m_roe;

	sf::Text m_id_text;
};

#endif // SPATIALOBJECT_H_INCLUDED
