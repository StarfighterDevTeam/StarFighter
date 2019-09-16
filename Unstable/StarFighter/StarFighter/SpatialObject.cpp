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
	//see override function in class Planet and AIShip
}

SpatialObject* SpatialObject::GetTargetableEnemyShip(const float dist_max, const float angle_delta_max)
{
	float shortest_distance = -1;
	SpatialObject* target = NULL;

	ColliderType target_collider = m_collider == AllyShipObject ? EnemyShipObject : AllyShipObject;
	
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[target_collider])
	{
		const float a = m_position.x - object->m_position.x;
		const float b = m_position.y - object->m_position.y;

		float distance_to_ref = (a * a) + (b * b);
		if (distance_to_ref < shortest_distance || shortest_distance < 0)
		{
			if (distance_to_ref <= dist_max * dist_max)
			{
				float angle_delta = GetAngleDegToTargetPosition(m_position, m_heading, object->m_position);

				if (abs(angle_delta) <= angle_delta_max)
				{
					shortest_distance = distance_to_ref;
					target = (SpatialObject*)object;
				}
			}
		}
	}

	return target;
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