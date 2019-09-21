#include "Beacon.h"

extern Game* CurrentGame;

using namespace sf;

Beacon::Beacon(sf::Vector2i sector_index, SpatialObject* trigger, bool isMissionObjective) : SpatialObject()
{
	(*CurrentGame).SetStarSectorIndex(this, sector_index);
	m_visible = false;
	m_trigger = trigger;
	m_isMissionObjective = isMissionObjective;
	m_collider = BeaconObject;
	m_layer = BackgroundLayer;
	m_size = sf::Vector2f(138, 138);
}

Beacon::~Beacon()
{
	for (AIShip* ship : m_ships_to_create)
		delete ship;
}

void Beacon::Draw(RenderTarget& screen)
{
	//Debug draw
	CircleShape circle;
	circle.setRadius(8);
	circle.setOrigin(sf::Vector2f(4, 4));
	circle.setFillColor(sf::Color::Green);
	circle.setPosition(getPosition());
	screen.draw(circle);
}

bool Beacon::TryTrigger(GameObject* trigger)
{
	if (trigger == m_trigger && m_isMissionObjective == false)//beacons that are tagged "mission" are triggerd in Player::UpdateMission()
	{
		Activate();
		return true;
	}
	else
		return false;
}

void Beacon::Activate()
{
	for (AIShip* ship : m_ships_to_create)
	{
		(*CurrentGame).addToScene(ship, AIShipLayer, ship->m_hostility == Hostility_Ally ? AllyShipObject : EnemyShipObject, true);
		ship->m_visible = true;
		ship->SetROE(ROE_FireAtWill);
	}

	m_ships_to_create.clear();
	m_garbageMe = true;
}