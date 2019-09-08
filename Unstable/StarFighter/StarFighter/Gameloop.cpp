#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//map
	(*CurrentGame).m_map_size = sf::Vector2f(1920, 1080);

	//ship
	Player* player = new Player(sf::Vector2i(0, 0));
	//(*CurrentGame).m_playerShip = player;
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShipObject, false);
	(*CurrentGame).UpdateSectorList(true);

	//Init first mission
	//sf::Vector2i index = sf::Vector2i(player->m_sector_index.x + RandomizeSign() * RandomizeIntBetweenValues(5, 10), player->m_sector_index.y + RandomizeIntBetweenValues(10, 15));
	sf::Vector2i index = sf::Vector2i(RandomizeSign() * RandomizeIntBetweenValues(0, 0), RandomizeIntBetweenValues(1, 1));
	Planet* planet = CreatePlanet(index, Hostility_Ally, 1, 1);
	Mission* mission = new Mission(Mission_GoTo, planet, planet);
	player->AcceptMission(mission);

	//enemy
	AIShip* enemy = CreateAIShip(Ship_Alpha, sf::Vector2i(2, 0), 0, Hostility_Enemy, ROE_ReturnFire);
	AIShip* enemy2 = CreateAIShip(Ship_Alpha, sf::Vector2i(3, 0), 0, Hostility_Enemy, ROE_ReturnFire);
	//AIShip* ally = CreateAIShip(Ship_Alpha, sf::Vector2i(1, 1), 0, Hostility_Ally, ROE_FireAtWill);
	//Planet* planet = CreatePlanet(sf::Vector2i(-2, 1), Hostility_Ally);

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

	//create procedural missions for planets
	for (Planet* planet : (*CurrentGame).m_planet_missions_to_create)
	{
		Player* player = (Player*)(*CurrentGame).m_playerShip;

		if (player->m_missions.size() >= NB_MISSIONS_MAX)
			break;

		if (planet->m_nb_missions == 0)
			continue;

		Mission* new_mission = CreateMission(planet);
		planet->m_missions.push_back(new_mission);
		planet->m_nb_missions--;

		player->AcceptMission(new_mission);
	}
	(*CurrentGame).m_planet_missions_to_create.clear();
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
	Star* new_star = StarGenerator::CreateStar(sector_index);

	//To be done
}

AIShip* Gameloop::CreateAIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe)
{
	AIShip* ship = new AIShip(ship_type, sector_index, heading, hostility, roe);
	if (ship->m_removeMe == false)
		(*CurrentGame).addToScene(ship, AIShipLayer, hostility == Hostility_Ally ? PlayerShipObject : EnemyShipObject, false);
	return ship;
}

Planet* Gameloop::CreatePlanet(sf::Vector2i sector_index, Hostility hostility, int nb_missions_min, int nb_missions_max)
{
	int nb_missions = RandomizeIntBetweenValues(nb_missions_min, nb_missions_max);

	Planet* planet = new Planet(sector_index, hostility, nb_missions);
	if (planet->m_removeMe == false)
		(*CurrentGame).addToScene(planet, Planet_Layer, PlanetObject, false);

	return planet;
}

Mission* Gameloop::CreateMission(Planet* owner)
{
	MissionType mission_type = (MissionType)RandomizeIntBetweenValues(0, NB_MISSION_TYPES - 1);
	sf::Vector2i starting_index = sf::Vector2i(owner->m_sector_index.x + RandomizeSign() * RandomizeIntBetweenValues(5, 10), owner->m_sector_index.y + RandomizeSign() * RandomizeIntBetweenValues(5, 10));
	sf::Vector2i found_index = starting_index;

	Planet* planet = NULL;
	int i = 0;
	int s = 1;
	while (planet == NULL)
	{
		//"snail" search for an existing planet or an unknown sector
		s *= (i == 0 || i % 2 == 1) ? 1 : -1;
		i++;

		if (i % 2 == 0)
			for (int y = 0; y < 2; y++)//y < (i / 2) * abs(s)
			{
				found_index.y += y * s * (i / 2) * abs(s);//found_index.y += s
				planet = GetPlanetForMission(starting_index + found_index);
				if (planet != NULL)
					break;
			}
		else
			for (int x = 0; x < 2; x++)//x < (1 + ((i + 1) / 2) - 1) * abs(s)
			{
				found_index.x += x * s * (1 + ((i + 1) / 2) - 1) * abs(s);//found_index.x += s; 
				planet = GetPlanetForMission(starting_index + found_index);
				if (planet != NULL)
					break;
			}

		//printf("found index: %d, %d\n", found_index.x, found_index.y);
	}

	return new Mission(Mission_GoTo, planet, planet);
}

Planet* Gameloop::GetPlanetForMission(sf::Vector2i sector_index)
{
	Planet* planet = NULL;

	int id = (*CurrentGame).GetSectorId(sector_index);
	if (id == -1)
		planet = CreatePlanet(sector_index, Hostility_Ally, 1, 1);
	else
	{
		for (GameObject* object : (*CurrentGame).m_sceneGameObjectsStored[id])
		{
			if (object->m_collider == PlanetObject)
			{
				planet = (Planet*)object;
				if (planet->m_nb_missions == 0)
					planet->m_nb_missions++;
				break;
			}
		}

		if ((*CurrentGame).m_sceneGameObjectsStored[id].empty() == true)
			(*CurrentGame).m_sceneGameObjectsStored.erase(id);
	}

	return planet;
}