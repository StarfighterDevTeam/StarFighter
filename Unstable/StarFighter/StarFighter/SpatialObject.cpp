#include "SpatialObject.h"

extern Game* CurrentGame;

using namespace sf;

SpatialObject::SpatialObject()
{
	m_marker_target = NULL;
	m_marker_mission = NULL;
}

SpatialObject::~SpatialObject()
{
	delete m_marker_target;
	delete m_marker_mission;
}

void SpatialObject::Update(sf::Time deltaTime)
{
	GameObject::Update(deltaTime);
}

void SpatialObject::SetHostility(Hostility hostility)
{
	m_hostility = hostility;

	if (m_marker_target == NULL && CheckMarkingConditions() == true)
		(*CurrentGame).m_playerShip->MarkThis(this, false);
	else if (m_marker_target != NULL && CheckMarkingConditions() == false)
		(*CurrentGame).m_playerShip->UnmarkThis(this, false);
}

void SpatialObject::SetROE(RuleOfEngagement roe)
{
	m_roe = roe;

	if (m_marker_target == NULL && CheckMarkingConditions() == true)
		(*CurrentGame).m_playerShip->MarkThis(this, false);
	else if (m_marker_target != NULL && CheckMarkingConditions() == false)
		(*CurrentGame).m_playerShip->UnmarkThis(this, false);
}

bool SpatialObject::IsMarked()
{
	return m_marker_mission != NULL || m_marker_target != NULL;
}

bool SpatialObject::CheckMarkingConditions()
{
	return false;
	//see override function in class Planet and AIShip
}