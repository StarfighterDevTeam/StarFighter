#include "AIShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
AIShip::AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, HostilityLevel hostility)
{
	m_ship_type = ship_type;
	m_hostility = hostility;
	m_marker = new Marker(hostility != Hostility_Ally ? Marker_Enemy : Marker_Ally, this);

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

			m_weapons.push_back(new Weapon(this, Weapon_Laser_Enemy, Ammo_LaserRed, collider, AIShipFireLayer, sf::Vector2f(0, textureSize.y * 0.5)));
			break;
		}
	}

	SetStarSectorIndex(sector_index);
	m_speed = GetSpeedVectorFromAbsoluteSpeedAndAngle(m_speed_max, (heading + 180) * M_PI / 180);

	Init(m_position, m_speed, textureName, textureSize, frameNumber, animationNumber);

	m_heading = heading;
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
	m_hostility = hostility;

	m_collider = hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject;
	for (Weapon* weapon : m_weapons)
		weapon->m_collider = m_collider;

	m_marker->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);
}