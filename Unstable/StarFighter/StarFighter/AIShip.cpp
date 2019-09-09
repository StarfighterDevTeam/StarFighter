#include "AIShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
AIShip::AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, bool dontStoreMe) : Ship()
{
	m_ship_type = ship_type;
	SetHostility(hostility);
	SetROE(roe);

	ColliderType weapon_collider = hostility == Hostility_Ally ? PlayerFire : EnemyFire;
	m_collider = hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject;
	m_layer = AIShipLayer;

	string textureName;
	sf::Vector2f textureSize;
	int frameNumber = 1;
	int animationNumber = 1;

	switch (m_ship_type)
	{
		case Ship_Alpha:
		{
			m_speed_max = 1000;
			m_acceleration_max = 2000;
			m_turn_speed = 160;
			m_braking_max = 3000;
			m_idle_decelleration = 1000;

			m_health_max = 10;
			m_shield_max = 10;
			m_shield_range = 50;
			m_shield_regen = 1.5;

			textureName = "2D/V_Alpha2_red.png";
			textureSize = sf::Vector2f(68, 84);
			frameNumber = 3;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(0, textureSize.y * 0.5)));
			break;
		}
	}

	Init(m_position, m_speed, textureName, textureSize, frameNumber, animationNumber);
	m_heading = heading;
	//setRotation(m_heading);

	(*CurrentGame).SetStarSectorIndex(this, sector_index, dontStoreMe);
	m_move_destination = m_position;
	m_target = NULL;

	InitShip();
}

AIShip::~AIShip()
{

}

void AIShip::Update(sf::Time deltaTime)
{
	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);//x == 1 == right; y == -1 == speed-up
	bool input_fire = false;

	//grouping with allied ships
	m_allied_ships.clear();
	for (GameObject* allied_ship : (*CurrentGame).m_sceneGameObjectsTyped[m_collider])
		if (allied_ship != this)
		{
			const float dx = allied_ship->m_position.x - m_position.x;
			const float dy = allied_ship->m_position.y - m_position.y;
			if (dx*dx + dy*dy < REF_WINDOW_RESOLUTION_X * 0.5 * REF_WINDOW_RESOLUTION_X * 0.5)
				m_allied_ships.push_back((SpatialObject*)allied_ship);
		}

	//AI - move & shoot strategies
	m_move_destination = m_position;

	if (m_target != NULL && m_target->m_garbageMe == true)
		m_target = NULL;

	switch (m_roe)
	{
		case ROE_HoldFire:
		case ROE_ReturnFire:
		{
			m_target = NULL;
			break;
		}
		case ROE_FireAtWill:
		{
			input_fire = true;

			if (m_target != NULL)
				m_target = KeepTarget();

			if (m_target == NULL)
				m_target = GetTargetableEnemyShip(REF_WINDOW_RESOLUTION_X * 2, 360);

			break;
		}
	}

	//Move strategy
	if (m_target != NULL)
		m_move_destination = m_target->m_position;

	//Apply move strategy
	GoTo(m_move_destination, deltaTime, inputs_direction);
	ApplyFlightModel(deltaTime, inputs_direction);

	Ship::Update(deltaTime);

	if (input_fire == true)
		for (Weapon* weapon : m_weapons)
		{
			weapon->Update(deltaTime);
			if (m_target != NULL)
				if (weapon->IsTargetAligned(m_target) == true)
					if (weapon->IsReadyToFire() == true)
						weapon->Fire();
		}
}

void AIShip::SetHostility(Hostility hostility)
{
	SpatialObject::SetHostility(hostility);

	m_collider = hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject;
	
	if (m_marker_target != NULL)
		m_marker_target->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);

	for (Weapon* weapon : m_weapons)
		weapon->m_collider = hostility == Hostility_Ally ? PlayerFire : EnemyFire;
}

bool AIShip::CheckMarkingConditions()
{
	return m_hostility == Hostility_Ally || m_roe == ROE_FireAtWill;
}

bool AIShip::GetHitByAmmo(GameObject* ammo)
{
	if (ammo->m_collider == PlayerFire && m_hostility == Hostility_Ally)
		return false;

	if (ammo->m_collider == EnemyFire && m_hostility == Hostility_Enemy)
		return false;

	if (m_roe == ROE_ReturnFire)
		SetROE(ROE_FireAtWill);

	return Ship::GetHitByAmmo(ammo);
}

void AIShip::GoTo(sf::Vector2f position, sf::Time deltaTime, sf::Vector2f& inputs_direction)
{
	const float dx = m_position.x - position.x;
	const float dy = m_position.y - position.y;
	const float delta_angle = GetAngleDegToTargetPosition(m_position, m_heading, position);
	//const float angle = GetAngleRadFromVector(sf::Vector2f(-dx, -dy));

	//move to desired position and heading
	if (m_position != position)
	{
		if (delta_angle < 0)
			inputs_direction.x = 1;
		else if (delta_angle > 0)
			inputs_direction.x = -1;

		if (dx * dx + dy * dy > m_range_max * m_range_max && abs(delta_angle) < m_angle_coverage_max)
			inputs_direction.y = -1;
		else if (abs(delta_angle) > 90)
			inputs_direction.y = 1;
	}
}

void AIShip::Draw(RenderTarget& screen)
{
	Ship::Draw(screen);

	if (m_position != m_move_destination)
	{
		sf::Vector2f destination = sf::Vector2f(m_move_destination.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_move_destination.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5);
		//DebugDrawSegment(getPosition(), destination, sf::Color::Magenta, (*CurrentGame).m_mainScreen);
	}
}

SpatialObject* AIShip::KeepTarget()
{
	const float a = m_position.x - m_target->m_position.x;
	const float b = m_position.y - m_target->m_position.y;

	float distance_to_ref = (a * a) + (b * b);
	if (distance_to_ref > m_range_max * m_range_max)
		m_target = NULL;

	if (m_target != NULL && abs(GetAngleDegToTargetPosition(m_position, m_heading, m_target->m_position) > 90))
		m_target = NULL;

	return m_target;
}

void AIShip::Death()
{
	m_garbageMe = true;

	if (m_marker_target != NULL)
		(*CurrentGame).m_playerShip->UnmarkThis(this, false);

	if (m_marker_mission != NULL)
		(*CurrentGame).m_playerShip->UnmarkThis(this, true);

	Ship::Death();
}

void AIShip::SetROE(RuleOfEngagement roe)
{
	SpatialObject::SetROE(roe);

	for (SpatialObject* allied_ship : m_allied_ships)
		allied_ship->SpatialObject::SetROE(roe);
}