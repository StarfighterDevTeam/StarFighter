#include "Warship.h"

extern Game* CurrentGame;

Warship::Warship(DMS_Coord coord) : Ship(coord, Ship_Warship, Alliance_Player, "Hispanola")
{
	m_angle = 90.f;
	m_desired_angle = m_angle;
	m_angle_speed = ANGLE_SPEED;
	m_destination = NULL;
	m_seaport = NULL;
	m_position.x = WATERTILE_OFFSET_X - WATERTILE_SIZE * (0.5f - NB_WATERTILE_VIEW_RANGE - 1);
	m_position.y = WATERTILE_OFFSET_Y - WATERTILE_SIZE * (0.5f - NB_WATERTILE_VIEW_RANGE - 1);
	m_can_be_seen = true;
	m_can_open_new_menu = false;
	m_sonar = 300;
	m_holds_capacity = 4;

	m_flee_timer = 0.f;
	m_flee_count = 0.f;
	m_is_charging_flee_count = true;

	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		m_upkeep_costs[i] = 0;
	}

	//shape for water tiles
	m_textureName = "2D/warship_icon.png";
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(m_textureName, (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);
	setAnimation(texture, 1, 2);

	//water tile outline
	m_default_color = sf::Color(0, 0, 0, 0);

	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(sf::Vector2f(WATERTILE_SIZE, WATERTILE_SIZE));
	m_shape_container.setOrigin(sf::Vector2f(WATERTILE_SIZE * 0.5f, WATERTILE_SIZE * 0.5f));
	m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));
	m_shape_container.setOutlineThickness(2.f);
	m_shape_container.setOutlineColor(m_default_color);

	//DMS coordinates display
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(24);
	m_text.setColor(sf::Color::Black);

	//ROOMS
	//left
	AddRoom(0, 1, 3, 4, Room_Diving);
	Room* prison_room = AddRoom(1, 5, 2, 4, Room_Prison);
	Room* prison_cell = NULL;
	for (int i = 0; i < 4; i++)
	{
		Room* cell = AddRoom(0, 5 + i, 1, 1, Room_PrisonCell);
		if (i == 0)
		{
			prison_cell = cell;
		}
		cell->m_tiles.front()->m_is_prison = true;
		m_prison_cells.push_back(cell->m_tiles.front());
	}

	//mid
	Room* nav_room = AddRoom(3, 0, 5, 2, Room_Navigation);
	AddRoom(3, 2, 5, 5, Room_Crewquarter);
	Room* engine_room = AddRoom(3, 7, 5, 2, Room_Engine);
	Room* lifeboats = AddRoom(3, 9, 5, 1, Room_Lifeboat);

	//right
	AddRoom(8, 1, 3, 4, Room_Weapon);
	Room* weapon_room = AddRoom(8, 5, 3, 4, Room_Weapon);

	m_health = m_health_max;
	m_lifeboats += 4;
	m_nb_crew_max = 8;

	//center position of each room & room tiles
	CenterRoomPositions(false);

	//doors
	ConnectRooms();

	//hardcode to add and lock doors of prison cells
	AddConnexion(prison_cell->m_tiles[0]->m_coord_x, prison_cell->m_tiles[0]->m_coord_y, prison_room->m_tiles[0]->m_coord_x, prison_room->m_tiles[0]->m_coord_y);
	for (vector<RoomConnexion*>::iterator it = m_connexions.begin(); it != m_connexions.end(); it++)
	{
		if ((*it)->m_tiles.second->m_room->m_type == Room_PrisonCell || (*it)->m_tiles.first->m_room->m_type == Room_PrisonCell)
		{
			(*it)->SetLock(true);
		}
	}

	//hull
	FlagHullRoomTiles();

	//WEAPONS
	//gunner tiles
	for (int i = 0; i < 3; i++)
	{
		int x = i * weapon_room->m_width + weapon_room->m_width - 1;
		weapon_room->m_tiles[x]->m_operator_tile = weapon_room->m_tiles[x - 1];
		weapon_room->m_tiles[x - 1]->m_system_tile = weapon_room->m_tiles[x];

		weapon_room->m_tiles[x]->m_system = System_Weapon;

		Weapon* weapon = NULL;
		if (i == 0)
		{
			weapon = new Weapon(Weapon_Cannon, false);
		}
		else if (i == 1)
		{
			weapon = new Weapon(Weapon_Cannon, false);
		}
		else if (i == 2)
		{
			weapon = new Weapon(Weapon_Torpedo, false);
		}

		AddWeaponToTile(weapon, weapon_room->m_tiles[x]);
	}

	//navigation tile
	nav_room->m_tiles[2]->m_operator_tile = nav_room->m_tiles[2 + nav_room->m_width];
	nav_room->m_tiles[2 + nav_room->m_width]->m_system_tile = nav_room->m_tiles[2];
	nav_room->m_tiles[2]->m_system = System_Navigation;
	AddRudderToTile(nav_room->m_tiles[2]);
	
	//engine tiles
	for (int i = 0; i < 2; i++)
	{
		i *= engine_room->m_width - 1;
		engine_room->m_tiles[i + engine_room->m_width]->m_operator_tile = engine_room->m_tiles[i];
		engine_room->m_tiles[i]->m_system_tile = engine_room->m_tiles[i + engine_room->m_width];
		engine_room->m_tiles[i + engine_room->m_width]->m_system = System_Engine;
		AddEngineToTile(engine_room->m_tiles[i + engine_room->m_width]);
	}

	//combat interface (displaying hp, etc.)
	m_combat_interface.Init(this, m_alliance, m_display_name, m_type);
}

void Warship::Init()
{
	//resources
	m_resources[Resource_Gold] = 1000;
	m_resources[Resource_Fish] = 1000;
	m_resources[Resource_Mech] = 100;
	m_resources[Resource_Fidelity] = 50;
	m_resources[Resource_Days] = 10;

	//crew
	m_nb_crew = 0;

	for (int i = 0; i < 1; i++)
	{
		CrewMember* crew = new CrewMember(Crew_Pirate, m_alliance);
		AddCrewMember(crew, m_rooms.front());
	}
}

Warship::~Warship()
{
	for (vector<Commodity*>::iterator it = m_holds.begin(); it != m_holds.end(); it++)
	{
		delete *it;
	}
}

void Warship::UpdateOLD(Time deltaTime, bool tactical_combat)
{
	//Compass input
	m_compass.GetInput(m_angle, m_desired_angle);
	
	//Turn boat to desired angle
	if (COMPASS_MODE == true)//mode compass
	{
		if (m_angle != m_desired_angle && sf::Mouse::isButtonPressed(sf::Mouse::Left) == false)
		{
			float delta = m_angle - m_desired_angle;
			if (delta > 180)
				delta -= 360;
			else if (delta < -180)
				delta += 360;

			if (abs(delta) > abs(m_angle_speed)*deltaTime.asSeconds())
			{
				m_angle -= delta >= 0 ? m_angle_speed * deltaTime.asSeconds() : -m_angle_speed * deltaTime.asSeconds();
			}
			else
			{
				m_angle = m_desired_angle;
			}
		}

		//flip the sprite according to the direction
		UpdateAnimation();

		m_speed.x = CRUISE_SPEED * sin(m_angle * M_PI / 180.f);
		m_speed.y = -CRUISE_SPEED * cos(m_angle * M_PI / 180.f);
	}

	//travel management (needs to be refreshed? arrived?)
	if (m_current_path.empty() == false)
	{
		WaterTile* waypoint = m_current_path.back();

		//arrived at waypoint?
		if (GetDistanceSquaredInSecondsDMS(waypoint) < 1.f)
		{
			m_DMS = waypoint->m_DMS;//snap boat to position
			waypoint->UpdatePosition(m_DMS);//snap tile to boat 
			m_current_path.pop_back();

			//arrived at final destination
			if (m_current_path.empty() == true)
			{
				m_speed = sf::Vector2f(0, 0);
				m_destination = NULL;
			}
			//go to next waypoint
			else
			{
				waypoint = m_current_path.back();
				waypoint->UpdatePosition(m_DMS);//update waypoint position (because of the previous snap)
				sf::Vector2f vec = waypoint->m_position - m_position;
				ScaleVector(&vec, CRUISE_SPEED);
				m_speed = vec;
			}
		}
	}

	//apply movement on water tiles
	if (tactical_combat == false)
	{
		m_DMS.m_second_x += m_speed.x * deltaTime.asSeconds();
		m_DMS.m_second_y -= m_speed.y * deltaTime.asSeconds();

		//rotation
		if (COMPASS_MODE == false)
		{
			GetAngleForSpeed(m_angle);
			m_desired_angle = m_angle;
			UpdateAnimation();
		}

		//sexadecimal position system
		if (m_DMS.m_second_x >= NB_WATERTILE_SUBDIVISION)
		{
			int minutes = m_DMS.m_second_x / NB_WATERTILE_SUBDIVISION;
			m_DMS.m_minute_x += minutes;
			m_DMS.m_second_x -= minutes * NB_WATERTILE_SUBDIVISION;
		}
		else if (m_DMS.m_second_x < 0)
		{
			int minutes = (-m_DMS.m_second_x) / NB_WATERTILE_SUBDIVISION + 1;
			m_DMS.m_minute_x -= minutes;
			m_DMS.m_second_x += minutes * NB_WATERTILE_SUBDIVISION;
		}

		if (m_DMS.m_second_y >= NB_WATERTILE_SUBDIVISION)
		{
			int minutes = m_DMS.m_second_y / NB_WATERTILE_SUBDIVISION;
			m_DMS.m_minute_y += minutes;
			m_DMS.m_second_y -= minutes * NB_WATERTILE_SUBDIVISION;
		}
		else if (m_DMS.m_second_y < 0)
		{
			int minutes = (-m_DMS.m_second_y) / NB_WATERTILE_SUBDIVISION + 1;
			m_DMS.m_minute_y -= minutes;
			m_DMS.m_second_y += minutes * NB_WATERTILE_SUBDIVISION;
		}

		//update tile information
		m_tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x][m_DMS.m_minute_y];

		//UI
		ostringstream ss;
		ss << "\n\n\n";
		ss << m_DMS.m_degree_y << "°" << m_DMS.m_minute_y << "' " << (int)m_DMS.m_second_y << "\"\N";
		ss << "\n";
		ss << m_DMS.m_degree_x << "°" << m_DMS.m_minute_x << "' " << (int)m_DMS.m_second_x << "\"\E";
		m_text.setString(ss.str());

		//Compass UI update
		m_compass.Update(deltaTime, m_angle, m_desired_angle);
	}
	//Tactical combat
	else
	{
		UpdateFleeing(deltaTime);

		if (IsFlooded() == true)
		{
			UpdateSinking(deltaTime);
		}
	}

	UpdateFlooding(deltaTime);

	UpdateShipOffset();

	GameEntity::Update(deltaTime);
}

void Warship::UpdateUpkeepCosts()
{
	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		m_upkeep_costs[i] = 0;
		for (int j = 0; j < 2; j++)
		{
			for (vector<CrewMember*>::iterator it = m_crew[j].begin(); it != m_crew[j].end(); it++)
			{
				m_upkeep_costs[i] += (*it)->m_upkeep_cost[i];
			}
		}
	}
}

bool Warship::HasCommodity(CommodityType commodity)
{
	if (commodity == Commodity_None)
	{
		return true;
	}

	for (vector<Commodity*>::iterator it = m_holds.begin(); it != m_holds.end(); it++)
	{
		if ((*it)->m_type == commodity)
		{
			return true;
		}
	}

	return false;
}

bool Warship::ApplyUpgrade(string upgrade_type)
{
	for (vector<vector<string> >::iterator it = (*CurrentGame).m_upgrades_config.begin(); it != (*CurrentGame).m_upgrades_config.end(); it++)
	{
		if ((*it)[Upgrade_ID].compare(upgrade_type) == 0)
		{
			int value = stoi((*it)[Upgrade_DynamicValue]);

			if ((*it)[Upgrade_Stat].compare("sonar") == 0)
			{
				m_sonar = value;
			}
			else if ((*it)[Upgrade_Stat].compare("lifeboat") == 0)
			{
				m_lifeboats = value;
			}

			m_upgrades.push_back(new Upgrade(upgrade_type));

			return true;
		}
	}

	return false;
}