#include "AIShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
AIShip::AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, HostilityLevel hostility)
{
	m_ship_type = ship_type;
	m_hostility = hostility;

	string textureName;
	sf::Vector2f textureSize;
	int frameNumber = 1;
	int animationNumber = 1;
	ColliderType collider = hostility == Hostility_Ally ? PlayerFire : EnemyFire;
	switch (m_ship_type)
	{
		case Ship_Alpha:
		{
			m_speed_max = 100;
			m_max_acceleration = 2000;
			m_turn_speed = 160;
			m_max_braking = 3000;
			m_idle_decelleration = 1000;

			textureName = "2D/V_Alpha2_red.png";
			textureSize = sf::Vector2f(68, 84);
			frameNumber = 3;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, collider, AIShipFireLayer, sf::Vector2f(0, textureSize.y * 0.5)));
			break;
		}
	}

	SetStarSectorIndex(sector_index);

	Init(m_position, m_speed, textureName, textureSize, frameNumber, animationNumber);

	m_heading = heading;

	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);

	m_marker = new Marker(hostility != Hostility_Ally ? Marker_Enemy : Marker_Ally, this);
	(*CurrentGame).m_playerShip->MarkThis(this);
}

AIShip::~AIShip()
{
	delete m_marker;
}

void AIShip::Update(sf::Time deltaTime)
{
	ApplyFlightModel(deltaTime, sf::Vector2f(1, -1));

	switch (m_hostility)
	{
		case Hostility_Ally:
		case Hostility_HoldFire:
		case Hostility_ReturnFire:
			break;
		case Hostility_FireAtWill:
			for (Weapon* weapon : m_weapons)
			{
				weapon->Update(deltaTime);

				if (weapon->IsReadyToFire() == true)
					weapon->Fire();
			}
		
	}

	Ship::Update(deltaTime);
}

void AIShip::SetHostility(HostilityLevel hostility)
{
	SpatialObject::SetHostility(hostility);

	for (Weapon* weapon : m_weapons)
		weapon->m_collider = m_collider;
}

bool AIShip::GetHitByAmmo(GameObject* ammo)
{
	if (ammo->m_collider == PlayerFire && m_hostility == Hostility_Ally)
		return false;

	if (m_hostility == Hostility_ReturnFire)
		m_hostility = Hostility_FireAtWill;

	return Ship::GetHitByAmmo(ammo);
}