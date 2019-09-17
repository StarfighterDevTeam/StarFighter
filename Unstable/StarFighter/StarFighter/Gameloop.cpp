#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//map
	(*CurrentGame).m_map_size = sf::Vector2f(1920, 1080);

	//ship
	Player* player = new Player(sf::Vector2i(0, 0));
	//(*CurrentGame).m_playerShip = player;
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, AllyShipObject, false);
	(*CurrentGame).UpdateSectorList(true);

	//Init first mission
	//sf::Vector2i index = sf::Vector2i(player->m_sector_index.x + RandomizeSign() * RandomizeIntBetweenValues(5, 10), player->m_sector_index.y + RandomizeIntBetweenValues(10, 15));
	sf::Vector2i index = sf::Vector2i(RandomizeSign() * RandomizeIntBetweenValues(0, 0), RandomizeIntBetweenValues(2, 2));
	Planet* planet = CreatePlanet(index, Hostility_Ally, 1, 1);
	Mission* mission = new Mission(Mission_GoTo, planet, planet);
	player->AcceptMission(mission);

	//AIShip* cruiser = CreateAIShip(Ship_Cruiser, sf::Vector2i(10, 0), 0, Hostility_Enemy, ROE_Ambush);
	//AIShip* enemy = CreateAIShip(Ship_Alpha, sf::Vector2i(2, 0), 0, Hostility_Enemy, ROE_FireAtWill);
	//AIShip* enemy2 = CreateAIShip(Ship_Alpha, sf::Vector2i(3, 0), 0, Hostility_Enemy, ROE_FireAtWill);
	//AIShip* ally = CreateAIShip(Ship_Alpha, sf::Vector2i(1, 1), 0, Hostility_Ally, ROE_FireAtWill);
	//Planet* planet = CreatePlanet(sf::Vector2i(-2, 1), Hostility_Ally);

	//star
	//StarGenerator* generator = new StarGenerator();
}

Gameloop::~Gameloop()
{
	for (StarGenerator* star_generator : m_star_generator)
		delete star_generator;

	m_star_generator.clear();
}

void Gameloop::Update(sf::Time deltaTime)
{
	Player* player = (Player*)(*CurrentGame).m_playerShip;

	//create procedural content for new sectors
	for (sf::Vector2i sector_index : (*CurrentGame).m_star_sectors_to_create)
		PopulateSector(sector_index);
	(*CurrentGame).m_star_sectors_to_create.clear();

	//create procedural stars for sectors
	for (sf::Vector2i sector_index : (*CurrentGame).m_sectors_to_add_star)
		StarGenerator::CreateStar(sector_index);
	(*CurrentGame).m_sectors_to_add_star.clear();

	//Get new missions from planet where're orbiting around
	while (player->m_missions.size() < NB_MISSIONS_MAX && player->m_isOrbiting != NULL && player->m_isOrbiting->m_nb_missions_to_create > 0)
	{
		Mission* new_mission = CreateMission(player->m_isOrbiting);
		player->m_isOrbiting->m_nb_missions_to_create--;
		player->AcceptMission(new_mission);
	}
	
	//update objects
	(*CurrentGame).UpdateScene(deltaTime);
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
		(*CurrentGame).addToScene(ship, AIShipLayer, hostility == Hostility_Ally ? AllyShipObject : EnemyShipObject, false);
	return ship;
}

EscortShip* Gameloop::CreateEscortShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, AIShip* escorted_ship, sf::Vector2f escort_offset)
{
	EscortShip* ship = new EscortShip(ship_type, sector_index, heading, hostility, roe, escorted_ship, escort_offset);
	if (ship->m_removeMe == false)
		(*CurrentGame).addToScene(ship, AIShipLayer, hostility == Hostility_Ally ? AllyShipObject : EnemyShipObject, false);

	return ship;
}

Planet* Gameloop::CreatePlanet(sf::Vector2i sector_index, Hostility hostility, int nb_missions_to_create_min, int nb_missions_to_create_max)
{
	int nb_missions_to_create = RandomizeIntBetweenValues(nb_missions_to_create_min, nb_missions_to_create_max);

	Planet* planet = new Planet(sector_index, hostility, nb_missions_to_create);
	if (planet->m_removeMe == false)
		(*CurrentGame).addToScene(planet, Planet_Layer, PlanetObject, false);

	return planet;
}

pair<Planet*, sf::Vector2i> Gameloop::SnailSearchSectorForMission(sf::Vector2i starting_index, MissionType mission_type)
{
	sf::Vector2i found_index = starting_index;

	//find sector where to create the mission
	Planet* planet = NULL;
	int i = 0;
	int s = 1;
	int id = 0;
	while (planet == NULL && id >= 0)
	{
		//"snail" search for an existing planet or an unknown sector
		s *= (i == 0 || i % 2 == 1) ? 1 : -1;
		i++;

		if (i % 2 == 0)
			for (int y = 0; y < 2; y++)//y < (i / 2) * abs(s)
			{
				found_index.y += y * s * (i / 2) * abs(s);//found_index.y += s
				id = (*CurrentGame).GetSectorId(found_index);

				if (id == -1)//unkown sector found
				{
					//check if we don't already have a mission in the surrounding to avoid objectives overlap
					if (IsSectorNearAnExistingMission(found_index) == true)
						id = 0;

					if (id == -1)
						break;
				}
				else if (mission_type == Mission_GoTo)
				{
					planet = GetPlanetAtSectorId(id);//re-use an existing planet?
					if (planet != NULL)
						break;
				}
			}
		else
			for (int x = 0; x < 2; x++)//x < (1 + ((i + 1) / 2) - 1) * abs(s)
			{
				found_index.x += x * s * (1 + ((i + 1) / 2) - 1) * abs(s);//found_index.x += s; 
				id = (*CurrentGame).GetSectorId(found_index);

				if (id == -1)//unkown sector found
				{
					//check if we don't already have a mission in the surrounding
					if (IsSectorNearAnExistingMission(found_index) == true)
						id = 0;

					if (id == -1)
						break;
				}
				else if (mission_type == Mission_GoTo)
				{
					planet = GetPlanetAtSectorId(id);//re-use an existing planet?
					if (planet != NULL)
						break;
				}
			}
	}

	return pair<Planet*, sf::Vector2i>(planet, found_index);
}

Mission* Gameloop::CreateMission(Planet* owner)
{
	MissionType mission_type = Mission_Convoy;// (MissionType)RandomizeIntBetweenValues(0, NB_MISSION_TYPES - 1);
	sf::Vector2i starting_index = sf::Vector2i(owner->m_sector_index.x + RandomizeSign() * RandomizeIntBetweenValues(5, 10), owner->m_sector_index.y + RandomizeSign() * RandomizeIntBetweenValues(5, 10));

	//find sector where to create the mission
	pair<Planet*, sf::Vector2i> found_sector = SnailSearchSectorForMission(starting_index, mission_type);
	Planet* planet = found_sector.first;
	sf::Vector2i found_index = found_sector.second;

	//create mission
	float distance = RandomizeFloatBetweenValues(REF_WINDOW_RESOLUTION_X * 1.5, REF_WINDOW_RESOLUTION_X * 1.8);
	float angle = GetAngleRadFromVector(sf::Vector2f(1.f * (found_index.x - owner->m_sector_index.x), 1.f * (found_index.y - owner->m_sector_index.y)));
	angle += RandomizeFloatBetweenValues(-0.3, 0.3);
	
	switch (mission_type)
	{
		case Mission_GoTo:
		{
			//re-use an existing planet and add a mission to it
			if (planet != NULL)
				planet->m_nb_missions_to_create++;
			//sector unknown => create a planet
			else
				planet = CreatePlanet(found_index, Hostility_Ally);

			return new Mission(mission_type, planet, planet);
		}
		case Mission_Convoy:
		{
			AIShip* ship = CreateAIShip(Ship_Convoy, found_index, 0, Hostility_Ally, ROE_MoveCautiously);

			//convoy destination
			sf::Vector2i destination_index = sf::Vector2i(found_index - owner->m_sector_index);
			ScaleVectorInt(&destination_index, RandomizeIntBetweenValues(8, 12));

			pair<Planet*, sf::Vector2i> destination_sector = SnailSearchSectorForMission(found_index + destination_index, mission_type);

			if (destination_sector.first == NULL)
				destination_sector.first = CreatePlanet(destination_sector.second, Hostility_Ally);

			ship->m_scripted_destination = new sf::Vector2f(destination_sector.first->m_position);
			sf::Vector2f destination_vector = sf::Vector2f(destination_sector.first->m_position.x - ship->m_position.x, destination_sector.first->m_position.y - ship->m_position.y);
			ship->m_heading = GetAngleRadFromVector(destination_vector) * 180 / M_PI;

			//convoy escorts
			ship->m_scripted_allied_ships.push_back(CreateEscortShip(Ship_Alpha, found_index, ship->m_heading, Hostility_Ally, ROE_Ambush, ship, sf::Vector2f(200, -400)));
			ship->m_scripted_allied_ships.push_back(CreateEscortShip(Ship_Alpha, found_index, ship->m_heading, Hostility_Ally, ROE_Ambush, ship, sf::Vector2f(0, -500)));

			AIShip* enemy = CreateAIShip(Ship_Alpha, destination_sector.second + sf::Vector2i(2, 0), ship->m_heading + 180, Hostility_Enemy, ROE_Ambush);
			
			return new Mission(mission_type, ship, destination_sector.first);
		}
		case Mission_Bounty:
		{
			Beacon* beacon = new Beacon(found_index);
			for (int e = 0; e < 3; e++)
			{
				sf::Vector2f vector = GetVectorFromLengthAndAngle(distance, angle);
				sf::Vector2i offset = sf::Vector2i((int)(-vector.x / STAR_SECTOR_SIZE), (int)(-vector.y / STAR_SECTOR_SIZE));
				angle += 0.08;

				AIShip* ship = new AIShip(Ship_Alpha, found_index + offset, (angle * 180 / M_PI) + 180, Hostility_Enemy, ROE_HoldFire);
				ship->m_visible = false;
				beacon->m_ships_to_create.push_back(ship);
			}
				
			return new Mission(mission_type, beacon, owner);
		}
		case Mission_EliminateBoss:
		{
			AIShip* ship;
			for (int e = 0; e < 3; e++)
			{
				sf::Vector2f vector = GetVectorFromLengthAndAngle(distance, angle);
				sf::Vector2i offset = sf::Vector2i((int)(-vector.x / STAR_SECTOR_SIZE), (int)(-vector.y / STAR_SECTOR_SIZE));
				angle += 0.08;

				if (e == 0)
					ship = CreateAIShip(Ship_Cruiser, found_index + offset, (angle * 180 / M_PI) + 180, Hostility_Enemy, ROE_Ambush);
				else
					ship->m_scripted_allied_ships.push_back(CreateAIShip(Ship_Alpha, found_index + offset, (angle * 180 / M_PI) + 180, Hostility_Enemy, ROE_Ambush));
			}

			return new Mission(mission_type, ship, owner);
		}
		case Mission_EliminateSquad:
		{
			AIShip* ship;
			for (int e = 0; e < 5; e++)
			{
				sf::Vector2f vector = GetVectorFromLengthAndAngle(distance, angle);
				sf::Vector2i offset = sf::Vector2i((int)(-vector.x / STAR_SECTOR_SIZE), (int)(-vector.y / STAR_SECTOR_SIZE));
				angle += 0.08;

				if (e == 0)
					ship = CreateAIShip(Ship_Alpha, found_index + offset, (angle * 180 / M_PI) + 180, Hostility_Enemy, ROE_Ambush);
				else
					ship->m_scripted_allied_ships.push_back(CreateAIShip(Ship_Alpha, found_index + offset, (angle * 180 / M_PI) + 180, Hostility_Enemy, ROE_Ambush));
			}

			return new Mission(mission_type, ship, owner);
		}
	}
}

bool Gameloop::IsSectorNearAnExistingMission(sf::Vector2i sector_index)
{
	Player* player = (Player*)(*CurrentGame).m_playerShip;
	for (Mission* mission : player->m_missions)
		if (mission->m_marked_objectives.empty() == false)
		{
			sf::Vector2i objective_index = mission->m_marked_objectives.front()->m_sector_index;
			if (abs(sector_index.x - objective_index.x) < 3.f * REF_WINDOW_RESOLUTION_X / STAR_SECTOR_SIZE && abs(sector_index.y - objective_index.y) < 3.f * REF_WINDOW_RESOLUTION_Y / STAR_SECTOR_SIZE)
				return true;
		}

	return false;
}

Planet* Gameloop::GetPlanetAtSectorId(int id)
{
	Planet* planet = NULL;

	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsStored[id])
		if (object->m_collider == PlanetObject)
			return (Planet*)object;

	if ((*CurrentGame).m_sceneGameObjectsStored[id].empty() == true)
		(*CurrentGame).m_sceneGameObjectsStored.erase(id);

	return NULL;
}