#include "AIShip.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
AIShip::AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe) : Ship()
{
	m_ship_type = ship_type;
	m_hostility = hostility;
	m_roe = roe;

	string textureName;
	sf::Vector2f textureSize;
	int frameNumber = 1;
	int animationNumber = 1;
	ColliderType weapon_collider = hostility == Hostility_Ally ? PlayerFire : EnemyFire;

	m_collider = hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject;
	m_layer = AIShipLayer;

	switch (m_ship_type)
	{
		case Ship_Alpha:
		{
			m_speed_max = 100;
			m_acceleration_max = 2000;
			m_turn_speed = 160;
			m_max_braking = 3000;
			m_idle_decelleration = 1000;

			textureName = "2D/V_Alpha2_red.png";
			textureSize = sf::Vector2f(68, 84);
			frameNumber = 3;

			m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserRed, weapon_collider, AIShipFireLayer, sf::Vector2f(0, textureSize.y * 0.5)));
			break;
		}
	}

	Init(m_position, m_speed, textureName, textureSize, frameNumber, animationNumber);

	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	m_heading = heading;

	//update position and rotation "manually" because they won't be updated during the frame of their creation
	setPosition(sf::Vector2f(m_position.x - (*CurrentGame).m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(m_position.y - (*CurrentGame).m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	setRotation(m_heading);

	m_marker = new Marker(hostility != Hostility_Ally ? Marker_Enemy : Marker_Ally, this);
	(*CurrentGame).m_playerShip->MarkThis(this);
}

AIShip::~AIShip()
{

}

void AIShip::Update(sf::Time deltaTime)
{
	ApplyFlightModel(deltaTime, sf::Vector2f(1, -1));

	Ship::Update(deltaTime);

	//weapons
	switch (m_roe)
	{
	case ROE_HoldFire:
	case ROE_ReturnFire:
		break;
	case ROE_FireAtWill:
		for (Weapon* weapon : m_weapons)
		{
			weapon->Update(deltaTime);
			GameObject* target = m_hostility == Hostility_Ally ? (*CurrentGame).m_playerShip : (*CurrentGame).m_playerShip;//todo: find appropriate target
			if (weapon->IsTargetAligned(target) == true)
				if (weapon->IsReadyToFire() == true)
					weapon->Fire();
		}
	}
}

void AIShip::SetHostility(Hostility hostility)
{
	SpatialObject::SetHostility(hostility);

	m_collider = hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject;
	m_marker->SetMarkerType(hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy);

	for (Weapon* weapon : m_weapons)
		weapon->m_collider = m_collider;
}

bool AIShip::GetHitByAmmo(GameObject* ammo)
{
	if (ammo->m_collider == PlayerFire && m_hostility == Hostility_Ally)
		return false;

	if (ammo->m_collider == EnemyFire && m_hostility == Hostility_Enemy)
		return false;

	if (m_roe == ROE_ReturnFire)
		m_roe = ROE_FireAtWill;

	return Ship::GetHitByAmmo(ammo);
}