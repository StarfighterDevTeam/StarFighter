#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//map
	(*CurrentGame).m_map_size = sf::Vector2f(1920, 1080);

	//ship
	Player* player = new Player(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/V_Alpha2.png", sf::Vector2f(68, 84), sf::Vector2f(34, 42), 3, 1);
	(*CurrentGame).m_playerShip = player;
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShipObject);
	(*CurrentGame).UpdateSectorList(true);

	//enemy
	AIShip* enemy = CreateAIShip(Ship_Alpha, sf::Vector2i(2, 0), 0, Hostility_ReturnFire);
	AIShip* enemy2 = CreateAIShip(Ship_Alpha, sf::Vector2i(3, 0), 0, Hostility_HoldFire);
	AIShip* ally = CreateAIShip(Ship_Alpha, sf::Vector2i(1, 1), 0, Hostility_Ally);
	Planet* planet = CreatePlanet(sf::Vector2i(-2, 1), Hostility_Ally);

	CreateMission();
	CreateMission();

	//star
	//StarGenerator* generator = new StarGenerator();
}

Gameloop::~Gameloop()
{
	for (StarGenerator* star_generator : m_star_generator)
	{
		delete star_generator;
	}
	m_star_generator.clear();
}

void Gameloop::Update(sf::Time deltaTime)
{
	//for (StarGenerator* star_generator : m_star_generator)
	//{
	//	star_generator->Update(deltaTime);
	//}
	
	(*CurrentGame).UpdateScene(deltaTime);

	//create procedural content for new sectors
	for (sf::Vector2i sector_index : (*CurrentGame).m_star_sectors_to_create)
	{
		PopulateSector(sector_index);
	}
	(*CurrentGame).m_star_sectors_to_create.clear();
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->m_position.x;
	const float b = (*CurrentGame).m_playerShip->m_position.y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

void Gameloop::PopulateSector(sf::Vector2i sector_index)
{
	//if (sector_index == sf::Vector2i(1, 0))
	Star* new_star = StarGenerator::CreateStar(sector_index);

	//To be done
}

AIShip* Gameloop::CreateAIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, HostilityLevel hostility)
{
	AIShip* ship = new AIShip(ship_type, sector_index, heading, hostility);
	(*CurrentGame).addToScene(ship, AIShipLayer, hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject);
	return ship;
}

Planet* Gameloop::CreatePlanet(sf::Vector2i sector_index, HostilityLevel hostility)
{
	int r = RandomizeIntBetweenValues(0, NB_PLANET_TYPES);
	return CreatePlanet(r, sector_index, hostility);
}

Planet* Gameloop::CreatePlanet(int planet_type, sf::Vector2i sector_index, HostilityLevel hostility)
{
	Planet* planet = new Planet(planet_type, sector_index, hostility);
	(*CurrentGame).addToScene(planet, Planet_Layer, BackgroundObject);
	return planet;
}

Mission* Gameloop::CreateMission()
{
	int m = RandomizeIntBetweenValues(0, (int)(NB_MISSION_TYPES - 1));
	Mission* mission = NULL;

	switch (m)
	{
		case Mission_GoTo_Easy:
		{
			mission = new Mission((MissionType)m);
			
			sf::Vector2i index = sf::Vector2i(RandomizeIntBetweenValues(5, 10), RandomizeIntBetweenValues(5, 10));
			Planet* planet = CreatePlanet(index, Hostility_HoldFire);
			mission->m_marked_objectives.push_back(planet);

			Player* player = (Player*)(*CurrentGame).m_playerShip;
			player->AcceptMission(mission);
		}
	}

	return mission;
}