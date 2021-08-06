#include "SpatialObject.h"

extern Game* CurrentGame;

using namespace sf;

SpatialObject::SpatialObject()
{
	m_marker_target = NULL;
	m_marker_mission = NULL;
	m_removeMe = false;
	m_garbageMe = false;
	m_visible = true;
	m_hostility = Hostility_Ally;
}

SpatialObject::~SpatialObject()
{
	delete m_marker_target;
	delete m_marker_mission;
}

void SpatialObject::Update(sf::Time deltaTime)
{
	//update target markers
	if (m_marker_target == NULL && CheckMarkingConditions() == true)
		(*CurrentGame).m_playerShip->MarkThis(this, false);
	else if (m_marker_target != NULL && CheckMarkingConditions() == false)
		(*CurrentGame).m_playerShip->UnmarkThis(this, false);

	GameObject::Update(deltaTime);
}

void SpatialObject::SetHostility(Hostility hostility)
{
	m_hostility = hostility;
}

void SpatialObject::SetROE(RuleOfEngagement roe)
{
	m_roe = roe;
}

bool SpatialObject::IsMarked()
{
	return m_marker_mission != NULL || m_marker_target != NULL;
}

bool SpatialObject::CheckMarkingConditions()
{
	return false;
	//see override function in class Planet and AIShip and HumanShip
}

AIShip* SpatialObject::GetEscortedShip()
{
	return NULL;
	//see override function in class EscortShip
}

bool SpatialObject::HasWeapons()
{
	return false;
	//see override function in class Ship
}

void SpatialObject::AddAmmoToShotsFired(Ammo* ammo)
{
	//see override function in class Ship
}

void SpatialObject::GetHitByGravitation()
{
	//see override function in class AIShip
}