#include "AIShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
AIShip::AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe) : Ship()
{
	ColliderType weapon_collider = hostility == Hostility_Ally ? AllyFire : EnemyFire;
	m_layer = AIShipLayer;
	m_collider = hostility == Hostility_Ally ? AllyShipObject : EnemyShipObject;
	m_scripted_destination = NULL;

	m_ship_type = ship_type;
	SetHostility(hostility);
	SetROE(roe);
	m_native_ROE = roe;
	m_move_clockwise = RandomizeSign() == 1 ? true : false;

	string textureName;
	sf::Vector2f textureSize;
	int frameNumber = 1;
	int animationNumber = 1;

	switch (m_ship_type)
	{
		case Ship_Alpha:
		{
			m_speed_min = 0;
			m_speed_max = 500;
			m_acceleration_max = 1000;
			m_turn_speed = 160;
			m_braking_max = 3000;
			m_idle_decelleration = 1000;

			m_health_max = 10;
			m_shield_max = 10;
			m_shield_range = 50;
			m_shield_regen = 1.5;

			textureName = hostility == Hostility_Ally ? "2D/ship_alpha_blue.png" : "2D/ship_alpha_red.png";
			textureSize = sf::Vector2f(68, 84);
			frameNumber = 3;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(0, textureSize.y * 0.5)));
			break;
		}
		case Ship_AlphaBlack:
		{
			m_speed_min = 300;
			m_speed_max = 700;
			m_acceleration_max = 1000;
			m_turn_speed = 240;
			m_braking_max = 3000;
			m_idle_decelleration = 1000;

			m_health_max = 50;
			m_shield_max = 20;
			m_shield_range = 50;
			m_shield_regen = 3;

			textureName = hostility == Hostility_Ally ? "2D/ship_alpha_blue.png" : "2D/ship_alpha_black.png";
			textureSize = sf::Vector2f(68, 84);
			frameNumber = 3;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(0, textureSize.y * 0.5)));
			break;
		}
		case Ship_Sigma:
		{
			m_speed_min = 500;
			m_speed_max = 1000;
			m_acceleration_max = 1500;
			m_turn_speed = 200;
			m_braking_max = 3000;
			m_idle_decelleration = 1000;

			m_health_max = 50;
			m_shield_max = 0;
			m_shield_range = 0;
			m_shield_regen = 0;

			m_collision_damage = 10;

			textureName = hostility == Hostility_Ally ? "2D/ship_sigma_blue.png" : "2D/ship_sigma_red.png";
			textureSize = sf::Vector2f(39, 52);
			frameNumber = 3;
			break;
		}
		case Ship_Cruiser:
		{
			m_speed_min = 0;
			m_speed_max = 200;
			m_acceleration_max = 2000;
			m_turn_speed = 30;
			m_braking_max = 3000;
			m_idle_decelleration = 1000;

			m_health_max = 200;
			m_shield_max = 100;
			m_shield_range = 250;
			m_shield_regen = 1.5;

			textureName = hostility == Hostility_Ally ? "2D/ship_cruiser_blue.png" : "2D/ship_cruiser_red.png";
			textureSize = sf::Vector2f(288, 390);
			frameNumber = 1;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(8, textureSize.y * 0.5)));
			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(-8, textureSize.y * 0.5)));
			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(textureSize.x * 0.5 - 8, textureSize.y * 0.25)));
			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(-textureSize.x * 0.5 + 8, textureSize.y * 0.25)));

			m_gravitation_range = REF_WINDOW_RESOLUTION_X;
			m_gravitation_strength = 70;//fine_tuned for player m_speed_max = 800; m_acceleration_max = 2000;

			break;
		}
		case Ship_Convoy:
		{
			m_speed_min = 0;
			m_speed_max = 100;
			m_acceleration_max = 500;
			m_turn_speed = 50;
			m_braking_max = 3000;
			m_idle_decelleration = 1000;

			m_health_max = 100;
			m_shield_max = 0;

			textureName = hostility == Hostility_Ally ? "2D/ship_convoy_blue.png" : "2D/ship_convoy_red.png";
			textureSize = sf::Vector2f(75, 126);
			frameNumber = 1;

			break;
		}
	}

	Init(m_position, m_speed, textureName, textureSize, frameNumber, animationNumber);
	m_heading = heading;

	(*CurrentGame).SetStarSectorIndex(this, sector_index);
	m_move_destination = m_position;
	m_target = NULL;

	InitShip();
}

AIShip::~AIShip()
{
	delete m_scripted_destination;
}

void AIShip::Update(sf::Time deltaTime)
{
	bool input_fire = false;

	//AI - move & shoot strategies
	//m_move_destination = (m_scripted_destination == NULL || m_roe == ROE_Freeze) ? m_position : *m_scripted_destination;

	if (m_target != NULL && m_target->m_garbageMe == true)
		m_target = NULL;

	switch (m_roe)
	{
		case ROE_Freeze:
		case ROE_HoldFire:
		case ROE_ReturnFire:
		{
			m_target = NULL;
			break;
		}
		case ROE_Ambush:
		case ROE_FireAtWill:
		{
			input_fire = true;

			if (m_target != NULL)
				m_target = KeepTarget(MaxBetweenValues(m_range_max, AMBUSH_ENGAGEMENT_DISTANCE));//distance max to keep track of the current target

			if (m_target == NULL)
				m_target = (*CurrentGame).GetTargetableEnemyShip(this, m_roe == ROE_Ambush ? AMBUSH_ENGAGEMENT_DISTANCE : FIREATWILL_ENGAGEMENT_DISTANCE, 360);

			if (m_target == NULL && m_roe != m_native_ROE)//no target is sight => go back to native Rule of engagement
				SetROE(m_native_ROE);

			break;
		}
	}

	//Move to forced destination
	/*if (m_position != m_move_destination && m_inputs_direction == sf::Vector2f(0, 0))
	{
		GoTo(m_move_destination, deltaTime, m_inputs_direction);
		TurnTo(m_move_destination, deltaTime, m_inputs_direction);
	}
	*/

	//Enemy nearby? Get tactical movements
	if (m_target != NULL)
		GetTacticalMoveAroundTarget(m_move_destination, m_target);
	else if (m_scripted_destination != NULL)
		m_move_destination = *m_scripted_destination;
	else
		m_move_destination = m_position;

	//move and turn towards destination
	if (m_position != m_move_destination)
	{
		GoTo(m_move_destination, deltaTime, m_inputs_direction);
		TurnTo(m_move_destination, deltaTime, m_inputs_direction);
	}
	else if (m_target != NULL)
	{
		m_move_clockwise = !m_move_clockwise;//randomize -1 : 1 for the next time we'll have to decide between clockwise or counter-clockwise movement
		TurnTo(m_target->m_position, deltaTime, m_inputs_direction);
	}

	//apply inputs
	ApplyFlightModel(deltaTime, m_inputs_direction);

	//reset inputs for next frame
	m_inputs_direction = sf::Vector2f(0, 0);//x == 1 == right; y == -1 == speed-up
	
	//Gravity circle to be drawn
	if (m_gravitation_range > 0 && m_roe == ROE_FireAtWill)
		(*CurrentGame).m_gravity_circles.push_back(m_gravitation_circle);

	Ship::Update(deltaTime);

	if (input_fire == true)
		for (Weapon* weapon : m_weapons)
		{
			weapon->Update(deltaTime, m_heading);
			if (m_target != NULL)
				if (weapon->IsTargetAligned(m_target, 30) == true)
				{
					if (weapon->IsReadyToFire() == true)
						weapon->Fire();

					if (m_roe == ROE_Ambush)
						SetROE(ROE_FireAtWill);
				}
		}
}

void AIShip::SetHostility(Hostility hostility)
{
	SpatialObject::SetHostility(hostility);

	m_collider = hostility == Hostility_Ally ? AllyShipObject : EnemyShipObject;
	
	if (m_marker_target != NULL)
		m_marker_target->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);

	for (Weapon* weapon : m_weapons)
		weapon->m_collider = hostility == Hostility_Ally ? AllyFire : EnemyFire;
}

bool AIShip::CheckMarkingConditions()
{
	return m_roe == ROE_FireAtWill;
}

void AIShip::GetHitByAmmo(GameObject* ammo)
{
	if (m_roe == ROE_ReturnFire || m_roe == ROE_Ambush)
		SetROE(ROE_FireAtWill);
	else
	{
		if (m_roe != ROE_FireAtWill && m_roe != ROE_Freeze)
			SetROE(ROE_Freeze);

		//allies can replicate if this ship cannot
		for (SpatialObject* allied_ship : m_dynamic_allied_ships)
		{
			Ship* ship = (Ship*)allied_ship;
			if (ship->m_roe == ROE_ReturnFire || ship->m_roe == ROE_Ambush)
				ship->Ship::SetROE(ROE_FireAtWill);
			else if (m_roe != ROE_FireAtWill && m_roe != ROE_Freeze)
				SetROE(ROE_Freeze);
		}

		for (SpatialObject* allied_ship : m_scripted_allied_ships)
		{
			Ship* ship = (Ship*)allied_ship;
			if (ship->m_roe == ROE_ReturnFire || ship->m_roe == ROE_Ambush)
				ship->Ship::SetROE(ROE_FireAtWill);
			else if (m_roe != ROE_FireAtWill && m_roe != ROE_Freeze)
				SetROE(ROE_Freeze);
		}
	}

	//switch target in case we're taking fire while we're aiming at an unarmed target
	Ammo* ammo_ = (Ammo*)ammo;
	if ((m_roe == ROE_Ambush || m_roe == ROE_FireAtWill) && m_target != NULL && m_target->HasWeapons() == false && ammo_->m_owner != NULL)
		m_target = ammo_->m_owner;

	Ship::GetHitByAmmo(ammo);
}

void AIShip::GetHitByObject(GameObject* object)
{
	if (m_roe == ROE_ReturnFire || m_roe == ROE_Ambush)
		SetROE(ROE_FireAtWill);
	else
	{
		if (m_roe != ROE_FireAtWill && m_roe != ROE_Freeze)
			SetROE(ROE_Freeze);

		//allies can replicate if this ship cannot
		for (SpatialObject* allied_ship : m_dynamic_allied_ships)
		{
			Ship* ship = (Ship*)allied_ship;
			if (ship->m_roe == ROE_ReturnFire || ship->m_roe == ROE_Ambush)
				ship->Ship::SetROE(ROE_FireAtWill);
			else if (m_roe != ROE_FireAtWill && m_roe != ROE_Freeze)
				SetROE(ROE_Freeze);
		}

		for (SpatialObject* allied_ship : m_scripted_allied_ships)
		{
			Ship* ship = (Ship*)allied_ship;
			if (ship->m_roe == ROE_ReturnFire || ship->m_roe == ROE_Ambush)
				ship->Ship::SetROE(ROE_FireAtWill);
			else if (m_roe != ROE_FireAtWill && m_roe != ROE_Freeze)
				SetROE(ROE_Freeze);
		}
	}

	//switch target in case we're taking fire while we're aiming at an unarmed target
	if ((m_roe == ROE_Ambush || m_roe == ROE_FireAtWill) && m_target != NULL && m_target->HasWeapons() == false)
		m_target = (SpatialObject*)object;

	Ship::GetHitByObject(object);
}

void AIShip::GetHitByGravitation()
{
	if (m_roe == ROE_Ambush)
		SetROE(ROE_FireAtWill);
}

void AIShip::GoTo(sf::Vector2f position, sf::Time deltaTime, sf::Vector2f& inputs_direction)
{
	const float dx = m_position.x - position.x;
	const float dy = m_position.y - position.y;
	const float delta_angle = GetAngleDegToTargetPosition(m_position, m_heading, position);

	bool speed_up_authorized;
	//if (m_collision_damage == 0 || m_weapons.empty() == false)//shooter profile
		//speed_up_authorized = dx*dx + dy*dy > m_speed.x*m_speed.x + m_speed.y*m_speed.y;//authorize to speed if destination is very far relative to the current speed
	//else//melee profile
		speed_up_authorized = true;

	if (speed_up_authorized == true && abs(delta_angle) < 15)
		inputs_direction.y = -1;
	else if (abs(delta_angle) > 90)
		inputs_direction.y = 1;
}

void AIShip::TurnTo(sf::Vector2f position, sf::Time deltaTime, sf::Vector2f& inputs_direction)
{
	const float dx = m_position.x - position.x;
	const float dy = m_position.y - position.y;
	const float delta_angle = GetAngleDegToTargetPosition(m_position, m_heading, position);

	if (delta_angle < -2)//let's not bother moving, this would only create micro movements
		inputs_direction.x = 1;
	else if (delta_angle > 2)
		inputs_direction.x = -1;
}

void AIShip::Draw(RenderTarget& screen)
{
	Ship::Draw(screen);

	if (m_position != m_move_destination)
	{
		sf::Vector2f destination = sf::Vector2f(m_move_destination.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_move_destination.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5);
		DebugDrawSegment(getPosition(), destination, sf::Color::Magenta, (*CurrentGame).m_mainScreen);
	}
}

SpatialObject* AIShip::KeepTarget(const float dist_max)
{
	const float a = m_position.x - m_target->m_position.x;
	const float b = m_position.y - m_target->m_position.y;

	const float distance_squared = a*a + b*b;
	if (distance_squared > dist_max * dist_max)
		m_target = NULL;

	if (m_target != NULL && abs(GetAngleDegToTargetPosition(m_position, m_heading, m_target->m_position) > 90))
		m_target = NULL;

	return m_target;
}

void AIShip::Death()
{
	//create loot
	if (m_hostility == Hostility_Enemy && RandomizeFloatBetweenValues(0, 1) < 0.70)
		CreateLoot(RandomizeIntBetweenValues(8, 20));

	Destructible::Death();
}

void AIShip::SetROE(RuleOfEngagement roe)
{
	SpatialObject::SetROE(roe);

	if (roe == ROE_FireAtWill)
	{
		for (SpatialObject* allied_ship : m_dynamic_allied_ships)
		{
			Ship* ship = (Ship*)allied_ship;
			ship->Ship::SetROE(roe);
		}
			
		for (SpatialObject* allied_ship : m_scripted_allied_ships)
		{
			Ship* ship = (Ship*)allied_ship;
			ship->Ship::SetROE(roe);
		}
	}
}

void AIShip::GetTacticalMoveAroundTarget(sf::Vector2f &move_destination, SpatialObject* target)
{
	switch (m_ship_type)
	{
		default:
		{
			move_destination = target->m_position;
		}
	}

	/*
	const float dx = m_position.x - m_target->m_position.x;
	const float dy = m_position.y - m_target->m_position.y;
	if (dx*dx + dy*dy > m_range_max * m_range_max)
	{
	//move to range position with same approch angle as current angle to target
	const float angle = GetAngleRadFromVector(sf::Vector2f(dx, dy));
	sf::Vector2f vector = sf::Vector2f(dx, dy);
	ScaleVector(&vector, m_range_max);
	m_move_destination = m_target->m_position + vector;
	}
	else
	{
	m_move_destination = m_position;
	}

	//check if not overlapping with friends
	OffsetMoveDestinationToAvoidAlliedShips(dx, dy);
	*/
}

void AIShip::UpdateAlliedShips()
{
	//updating forced allied ships (removing the dead)
	vector<SpatialObject*> tmp_forced_allied_ships;
	for (SpatialObject* ally : m_scripted_allied_ships)
		if (ally->m_garbageMe == false)
			tmp_forced_allied_ships.push_back(ally);
	m_scripted_allied_ships.clear();
	for (SpatialObject* ally : tmp_forced_allied_ships)
		m_scripted_allied_ships.push_back(ally);

	//grouping with nearby allied ships
	m_dynamic_allied_ships.clear();
	for (GameObject* allied_ship : (*CurrentGame).m_sceneGameObjectsTyped[m_collider])
		if (allied_ship != this && allied_ship->m_garbageMe == false && allied_ship != (*CurrentGame).m_playerShip)
		{
			const float dx = allied_ship->m_position.x - m_position.x;
			const float dy = allied_ship->m_position.y - m_position.y;
			if (dx*dx + dy*dy < REF_WINDOW_RESOLUTION_X * 0.5 * REF_WINDOW_RESOLUTION_X * 0.5)
			{
				//already a forced ally?
				bool found = false;
				for (SpatialObject* ally : m_scripted_allied_ships)
					if (ally == allied_ship)
					{
						found = true;
						break;
					}

				if (found == false)
					m_dynamic_allied_ships.push_back((SpatialObject*)allied_ship);
			}
		}
}

void AIShip::OffsetMoveDestinationToAvoidAlliedShips(const float dx, const float dy)
{
	bool check_ok = false;
	while (check_ok == false)
	{
		check_ok = true;
		for (SpatialObject* ally : m_scripted_allied_ships)
		{
			AIShip* ally_ship = (AIShip*)ally;
			float size = MaxBetweenValues(m_radius, ally_ship->m_radius);
			//optim: make the "lighter" ship turn (checking turn speed)
			if (m_turn_speed >= ally_ship->m_turn_speed && abs(m_move_destination.x - ally_ship->m_move_destination.x) < size * 2 && abs(m_move_destination.y - ally_ship->m_move_destination.y) < size * 2)
			{
				sf::Vector2f perp_vector = m_move_clockwise == true ? sf::Vector2f(dy, -dx) : sf::Vector2f(-dy, dx);
				ScaleVector(&perp_vector, size * 3);
				m_move_destination += perp_vector;
				check_ok = false;
			}

			if (check_ok == false)
				break;
		}

		if (check_ok == true)//second part
			for (SpatialObject* ally : m_dynamic_allied_ships)
			{
				AIShip* ally_ship = (AIShip*)ally;
				float size = MaxBetweenValues(m_radius, ally_ship->m_radius);
				//optim: make the "lighter" ship turn (checking turn speed)
				if (m_turn_speed >= ally_ship->m_turn_speed && abs(m_move_destination.x - ally_ship->m_move_destination.x) < size * 2 && abs(m_move_destination.y - ally_ship->m_move_destination.y) < size * 2)
				{
					sf::Vector2f perp_vector = m_move_clockwise == true ? sf::Vector2f(dy, -dx) : sf::Vector2f(-dy, dx);
					ScaleVector(&perp_vector, size * 3);
					m_move_destination += perp_vector;
					check_ok = false;
				}

				if (check_ok == false)
					break;
			}
	}
}