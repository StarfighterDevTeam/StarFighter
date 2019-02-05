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
	AddRoom(0, 1, 3, 4, Room_Prison);
	AddRoom(0, 5, 3, 4, Room_Diving);

	//mid
	Room* nav_room = AddRoom(3, 0, 5, 2, Room_Navigation);
	AddRoom(3, 2, 5, 5, Room_Crewquarter);
	AddRoom(3, 7, 5, 2, Room_Engine);
	AddRoom(3, 9, 5, 1, Room_Lifeboat);

	//right
	AddRoom(8, 1, 3, 4, Room_Weapon);
	Room* weapon_room = AddRoom(8, 5, 3, 4, Room_Weapon);

	m_health = m_health_max;

	//center position of each room & room tiles
	CenterRoomPositions(false);

	//doors
	ConnectRooms();

	//hull
	FlagHullRoomTiles();

	//WEAPONS
	//gunner tiles
	for (int i = 0; i < 3; i++)
	{
		int x = i * weapon_room->m_width + weapon_room->m_width - 1;
		weapon_room->m_tiles[x]->m_operator_tile = weapon_room->m_tiles[x - 1];
		weapon_room->m_tiles[x - 1]->m_system_tile = weapon_room->m_tiles[x];

		weapon_room->m_tiles[x]->m_operator_tile->m_system = System_Weapon;

		Weapon* weapon = new Weapon(Weapon_Cannon, false);
		AddWeaponToTile(weapon, weapon_room->m_tiles[x]);
	}

	//navigation tiles
	for (int j = 0; j < 5; j+=2)
	{
		if (nav_room->m_tiles[j]->m_coord_y == nav_room->m_upcorner_y)
		{
			nav_room->m_tiles[j]->m_operator_tile = nav_room->m_tiles[j + nav_room->m_width];
			nav_room->m_tiles[j + nav_room->m_width]->m_system_tile = nav_room->m_tiles[j];

			nav_room->m_tiles[j]->m_operator_tile->m_system = System_Navigation;
		}
	}

	//CREW
	m_nb_crew_max = 0;
	CrewMember* crew = new CrewMember(Crew_Pirate, m_alliance);
	AddCrewMember(crew, weapon_room);

	CrewMember* crew2 = new CrewMember(Crew_Pirate, m_alliance);
	AddCrewMember(crew2, weapon_room);

	CrewMember* crew3 = new CrewMember(Crew_Pirate, m_alliance);
	AddCrewMember(crew3, weapon_room);

	m_nb_crew = m_nb_crew_max;

	//FLOOD
	m_tiles[8][1]->m_pierced = true;
	m_tiles[8][1]->m_health = 0;
}

Warship::~Warship()
{
	
}

void Warship::Update(Time deltaTime, bool tactical_combat)
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

	//apply movement
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

	//Combat interface
	if (tactical_combat == true)
	{
		m_combat_interface.Update(deltaTime);
	}

	UpdateFlooding(deltaTime, false);

	//Crew interface
	m_crew_interface.Update();

	GameEntity::Update(deltaTime);
}

void Warship::UpdateCrewMembersCountPerRoom(Room* room)
{
	for (int i = 0; i < NB_CREW_TYPES; i++)
	{
		room->m_nb_crew[i] = 0;
		room->m_nb_crew_working[i] = 0;
	}
		
	for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
	{
		if ((*it)->m_destination != NULL && (*it)->m_destination->m_room == room)
		{
			//present
			room->m_nb_crew[(*it)->m_type]++;
			room->m_nb_crew[Crew_All]++;
		}

		if ((*it)->m_tile->m_room == room && (*it)->m_destination == NULL)
		{
			//working
			room->m_nb_crew_working[(*it)->m_type]++;
			room->m_nb_crew_working[Crew_All]++;

			//present
			room->m_nb_crew[(*it)->m_type]++;
			room->m_nb_crew[Crew_All]++;
		}
	}
}

//WATER PART
int Warship::GetDistanceToWaterTile(WaterTile* tile)
{
	int diff_x = tile->m_coord_x - m_DMS.m_minute_x;
	int diff_y = tile->m_coord_y - m_DMS.m_minute_y;
	float distance_f = sqrt(diff_x * diff_x + diff_y * diff_y);

	int distance_i = (int)distance_f;

	if (distance_f - distance_i > 0.10f)//custom round-up rule
	{
		distance_i++;
	}

	return distance_i;
}

float Warship::GetDistanceSquaredInSecondsDMS(WaterTile* tile)
{
	float xA = m_DMS.m_minute_x * NB_WATERTILE_SUBDIVISION + m_DMS.m_second_x;
	float yA = m_DMS.m_minute_y * NB_WATERTILE_SUBDIVISION + m_DMS.m_second_y;

	float xB = tile->m_DMS.m_minute_x * NB_WATERTILE_SUBDIVISION + tile->m_DMS.m_second_x;
	float yB = tile->m_DMS.m_minute_y * NB_WATERTILE_SUBDIVISION + tile->m_DMS.m_second_y;

	float distance_squared = (xA - xB) * (xA - xB) + (yA - yB) * (yA - yB);

	return distance_squared;
}

float Warship::GetDistanceFloatToWaterTile(WaterTile* tile)
{
	int diff_x = NB_WATERTILE_SUBDIVISION * (tile->m_coord_x - m_DMS.m_minute_x) - m_DMS.m_second_x;
	int diff_y = NB_WATERTILE_SUBDIVISION * (tile->m_coord_y - m_DMS.m_minute_y) - m_DMS.m_second_y;
	float distance_f = sqrt(diff_x * diff_x + diff_y * diff_y) * 1.f / NB_WATERTILE_SUBDIVISION;

	return distance_f;
}

bool Warship::CanViewWaterTile(WaterTile* tile)
{
	if (tile->m_coord_x > m_DMS.m_minute_x + NB_WATERTILE_VIEW_RANGE + 1 || tile->m_coord_x < m_DMS.m_minute_x - NB_WATERTILE_VIEW_RANGE - 1)
	{
		return false;
	}
	else if (tile->m_coord_y > m_DMS.m_minute_y + NB_WATERTILE_VIEW_RANGE + 1 || tile->m_coord_y < m_DMS.m_minute_y - NB_WATERTILE_VIEW_RANGE - 1)
	{
		return false;
	}

	//custom round-up system for visibility distance
	float distance = GetDistanceFloatToWaterTile(tile);
	if (distance <= NB_WATERTILE_VIEW_RANGE + 0.15f || (tile->m_can_be_seen == true && m_destination != NULL && distance <= NB_WATERTILE_VIEW_RANGE + 0.99f))
	{
		return true;
		//line of sight blocked by islands
		//if (RayTracingContainsIsland(m_tile, tile) == false)
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}
	}
	else
	{
		return false;
	}
}

bool Warship::SetSailsToWaterTile(WaterTile* tile)
{
	if (tile->m_type != Water_Empty)
	{
		return false;
	}

	sf::Vector2f move_vector = tile->m_position - m_position;
	ScaleVector(&move_vector, CRUISE_SPEED);
	m_speed = move_vector;
	
	m_destination = tile;
	
	//Find path and set speed
	FindShortestPath(m_tile, m_destination);
	if (m_current_path.empty() == false)
	{
		WaterTile* waypoint = m_current_path.back();
		sf::Vector2f vec = waypoint->m_position - m_position;
		ScaleVector(&vec, CRUISE_SPEED);
		m_speed = vec;
	}
	
	//leaving port?
	if (m_seaport != NULL && m_destination != (WaterTile*)m_seaport)
	{
		m_seaport->RemoveShip(this);
		m_seaport = NULL;
	}

	return true;
}

void Warship::FindShortestPath(WaterTile* tileA, WaterTile* tileB)
{
	if (tileA == tileB)
	{
		//left and down moves "fix" (because of the degree/minute/second metric system): we can be on the same tile (same minute) but not the same position (same seconds)
		int patch_x = 0;
		int patch_y = 0;
		if (m_DMS.m_second_x > 0)
		{
			patch_x = 1;
		}
		if (m_DMS.m_second_y > 0)
		{
			patch_y = 1;
		}

		if (patch_x > 0 || patch_y > 0)
		{
			tileA = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x + patch_x][m_DMS.m_minute_y + patch_y];
		}
	}

	//start
	m_open_list_pathfind.push_back(tileA);
	while (m_open_list_pathfind.empty() == false && tileB->m_parent == NULL)
	{
		//choose next best tile to compute
		size_t min_G_value = 0;
		WaterTile* next_tile = NULL;
		for (list<WaterTile*>::iterator it = m_open_list_pathfind.begin(); it != m_open_list_pathfind.end(); it++)
		{
			if ((*it)->m_G_value < min_G_value || min_G_value == 0)
			{
				min_G_value = (*it)->m_G_value;
				next_tile = *it;
			}
		}

		//compute this tile
		IteratePathFinding(next_tile, tileB);
	}

	//path found -> save all waypoints into a member path
	m_current_path.clear();
	vector<WaterTile*> temp_path;
	WaterTile* way_point = tileB;
	while (way_point != tileA)
	{
		temp_path.push_back(way_point);
		way_point = way_point->m_parent;
	}

	//clear data
	for (vector<WaterTile*>::iterator it = m_tiles_can_be_seen.begin(); it != m_tiles_can_be_seen.end(); it++)
	{
		(*it)->m_heuristic = 0;
		(*it)->m_movement_cost = 0;
		(*it)->m_G_value = 0;
		(*it)->m_parent = NULL;
	}

	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();

	//compute the best diagonals and eliminate useless waypoints
	temp_path.push_back(tileA);
	int path_size = temp_path.size();

	//left and down moves "fix" (because of the degree/minute/second metric system)
	int patch_x = 0;
	int patch_y = 0;

	//patch x
	if (m_DMS.m_second_x > 0)
	{
		for (int i = path_size - 1; i >= 0; i--)
		{
			//going up or down?
			if (temp_path[i]->m_DMS.m_minute_y != tileA->m_DMS.m_minute_y)
			{
				patch_x = 1;
				break;
			}
			//going right = stop
			else if (temp_path[i]->m_DMS.m_minute_x > tileA->m_DMS.m_minute_x)
			{
				break;
			}
		}
	}

	//patch y
	if (m_DMS.m_second_y > 0)
	{
		for (int i = path_size - 1; i >= 0; i--)
		{
			//going left or right?
			if (temp_path[i]->m_DMS.m_minute_x != tileA->m_DMS.m_minute_x)
			{
				patch_y = 1;
				break;
			}
			//going up = stop
			else if (temp_path[i]->m_DMS.m_minute_y > tileA->m_DMS.m_minute_y)
			{
				break;
			}
		}
	}

	//apply fix
	if (patch_x > 0 || patch_y > 0)
	{
		WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[m_DMS.m_minute_x + patch_x][m_DMS.m_minute_y + patch_y];
		temp_path.push_back(tile);
	}

	//look for shortcuts
	path_size = temp_path.size();
	int index = 0;
	while (index < path_size)
	{
		if (index == path_size - 1)
		{
			break;
		}
	
		m_current_path.push_back(temp_path[index]);

		int temp_index = index;

		bool straight_line = true;
		for (int i = index + 1; i < path_size; i++)
		{
			if (index == path_size - 2)
			{
				break;
			}

			//waypoints in line = cannot optimize
			if (straight_line == true && (temp_path[index]->m_coord_x == temp_path[i]->m_coord_x || temp_path[index]->m_coord_y == temp_path[i]->m_coord_y))
			{
				continue;
			}
			else
			{
				straight_line = false;
			}

			//shortcut conditions: the rectangle is filled with only water, OR we can cast a ray to the tile without meeting islands
			bool only_water = IsOnlyWaterInsideRectangle(temp_path[index], temp_path[i]);
			if (only_water == false)
			{
				//ray-tracing should not take in account the "patch" on position but it's true position
				//WaterTile* true_tile = new WaterTile(temp_path[i]->m_coord_x, temp_path[i]->m_coord_y, temp_path[i]->m_type, temp_path[i]->m_zone, temp_path[i]->m_zone->m_coord_x, temp_path[i]->m_zone->m_coord_y);
				//if (i == path_size - 1)
				//{
				//	true_tile->m_coord_x -= patch_x;
				//	true_tile->m_coord_y -= patch_y;
				//}
				//only_water = RayTracingContainsIslandForPathfind(true_tile, temp_path[index]) == false;
				//delete true_tile;
			}

			//chance of optimization
			if (only_water == true)
			{
				//skip all other waypoints
				temp_index = i;
			}
		}

		//we found a shortcut? continue from this waypoint on...
		if (index != temp_index)
		{
			index = temp_index;
		}
		//...otherwise, continue with the next waypoint
		else
		{
			index++;
		}
	}
}

void Warship::IteratePathFinding(WaterTile* tileA, WaterTile* tileB)
{
	m_closed_list_pathfind.push_back(tileA);
	m_open_list_pathfind.remove(tileA);

	const int tileA_x = tileA->m_coord_x;
	const int tileA_y = tileA->m_coord_y;
	const int tileB_x = tileB->m_coord_x;
	const int tileB_y = tileB->m_coord_y;

	//are we arrived at destination? (1 tile away from destination)
	if ((abs(tileA_x - tileB_x) == 1 && abs(tileA_y - tileB_y) == 0) || (abs(tileA_x - tileB_x) == 0 && abs(tileA_y - tileB_y) == 1))
	{
		tileB->m_parent = tileA;
		return;
	}

	//looks through all tiles to find the best next waypoint.
	size_t vector_size = m_tiles_can_be_seen.size();
	for (size_t i = 0; i < vector_size; i++)
	{
		if (m_tiles_can_be_seen[i]->m_type == Water_Empty)
		{
			if ((abs(tileA_x - m_tiles_can_be_seen[i]->m_coord_x) == 1 && abs(tileA_y - m_tiles_can_be_seen[i]->m_coord_y) == 0)
				|| (abs(tileA_x - m_tiles_can_be_seen[i]->m_coord_x) == 0 && abs(tileA_y - m_tiles_can_be_seen[i]->m_coord_y) == 1))
			{
				//tiles that are legitimate to compute	
				if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), m_tiles_can_be_seen[i]) == m_closed_list_pathfind.end())//tile unknown until now
				{
					if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), m_tiles_can_be_seen[i]) == m_open_list_pathfind.end())
					{
						//CASE where the tile is not on the closed list nor on the open list
						m_open_list_pathfind.push_back(m_tiles_can_be_seen[i]);

						//compute Heuristic value (distance between the computed tile and the target) - we avoid using square root here
						const int pos2_x = tileB->m_coord_x;
						const int pos2_y = tileB->m_coord_y;
						const int posit_x = tileB->m_coord_x;
						const int posit_y = tileB->m_coord_y;

						int H_value_x = posit_x > pos2_x ? posit_x - pos2_x : pos2_x - posit_x;
						int H_value_y = posit_y > pos2_y ? posit_y - pos2_y : pos2_y - posit_y;
						m_tiles_can_be_seen[i]->m_heuristic = H_value_x + H_value_y;

						//compute Movement cost
						m_tiles_can_be_seen[i]->m_movement_cost = 10;
						m_tiles_can_be_seen[i]->m_movement_cost += tileA->m_movement_cost;

						//G value
						m_tiles_can_be_seen[i]->m_G_value = m_tiles_can_be_seen[i]->m_heuristic + m_tiles_can_be_seen[i]->m_movement_cost;

						//parent node
						m_tiles_can_be_seen[i]->m_parent = tileA;
					}
				}
			}
		}
	}
}

bool Warship::IsOnlyWaterInsideRectangle(WaterTile* tileA, WaterTile* tileB)
{
	int x_min = Min(tileA->m_coord_x, tileB->m_coord_x);
	int x_max = Max(tileA->m_coord_x, tileB->m_coord_x);
	int y_min = Min(tileA->m_coord_y, tileB->m_coord_y);
	int y_max = Max(tileA->m_coord_y, tileB->m_coord_y);

	bool only_water = true;
	for (int j = x_min; j <= x_max; j++)
	{
		for (int k = y_min; k <= y_max; k++)
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[j][k]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[j][k]->m_can_be_seen == false)
			{
				only_water = false;
				break;
			}
		}

		if (only_water == false)
		{
			break;
		}
	}

	return only_water;
}

bool Warship::RayTracingContainsIsland(WaterTile* tileA, WaterTile* tileB)
{
	if (tileA == tileB)
	{
		return false;
	}

	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	bool contains_island = false;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x) + 1;
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y) + 1;
	int sum = X + Y - 2;

	int tx = 1;
	int ty = 1;

	for (int i = 0; i < sum; i++)
	{
		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx/X < 1.f * ty/Y)
		{
			tx++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
		}//going vertically is shorter?
		else if (X == 0 || 1.f * tx / X > 1.f * ty / Y)
		{
			ty++;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
		}
		else//perfect diagonal
		{
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
			tx++;
			ty++;
			i++;
		}

		//island?
		if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty)
		{
			contains_island = true;
		}
		else
		{
			//water-island-water? = water cannot be seen
			if (contains_island == true)
			{
				return true;
			}
		}
	}

	return false;
}

void Warship::RayTracingGetPath(WaterTile* tileA, WaterTile* tileB)
{
	m_current_path.clear();

	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x) + 1;
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y) + 1;
	int sum = X + Y - 2;

	int tx = 1;
	int ty = 1;

	for (int i = 0; i < sum; i++)
	{
		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx / X < 1.f * ty / Y)
		{
			tx++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
		}//going vertically is shorter?
		else if (X == 0 || 1.f * tx / X > 1.f * ty / Y)
		{
			ty++;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
		}
		else//perfect diagonal
		{
			tx++;
			ty++;
			i++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
		}

		WaterTile* tile = (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y];
		m_current_path.push_back(tile);
	}
}

bool Warship::RayTracingContainsIslandForPathfind(WaterTile* tileA, WaterTile* tileB)
{
	int coord_x = tileA->m_coord_x;
	int coord_y = tileA->m_coord_y;

	//Bresenham line-drawing algorithm
	int X = abs(tileA->m_coord_x - tileB->m_coord_x) + 1;
	int Y = abs(tileA->m_coord_y - tileB->m_coord_y) + 1;
	int sum = X + Y - 2;

	int tx = 1;
	int ty = 1;

	//RAY TRACING
	for (int i = 0; i < sum; i++)
	{
		//PRELIMINARY CHECKS
		if (tileA->m_coord_x < tileB->m_coord_x)//going right, checking right
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x + 1][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x + 1][coord_y]->m_can_be_seen == false)
			{
				printf("prelim check right: NOK %d, %d\n", coord_x + 1, coord_y);
				return true;
			}
		}
		if (tileA->m_coord_x > tileB->m_coord_x)//going left, checking left
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x - 1][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x - 1][coord_y]->m_can_be_seen == false)
			{
				printf("prelim check left: NOK %d, %d\n", coord_x - 1, coord_y);
				return true;
			}
		}
		if (tileA->m_coord_y < tileB->m_coord_y)//going up, checking up
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y + 1]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y + 1]->m_can_be_seen == false)
			{
				printf("prelim check up: NOK %d, %d\n", coord_x, coord_y + 1);
				return true;
			}
		}
		if (tileA->m_coord_y > tileB->m_coord_y)//going down, checking down
		{
			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y - 1]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y - 1]->m_can_be_seen == false)
			{
				printf("prelim check down: NOK %d, %d\n", coord_x, coord_y - 1);
				return true;
			}
		}

		//going horizontally is shorter?
		if (Y == 0 || 1.f * tx / X < 1.f * ty / Y)
		{
			tx++;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;
			printf("going horizontal: %d, %d\n", coord_x, coord_y);

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED\n", coord_x, coord_y);
				return true;
			}

		}//going vertically is shorter?
		else if (X == 0 || 1.f * tx / X > 1.f * ty / Y)
		{
			ty++;
			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;
			printf("going vertical: %d, %d\n", coord_x, coord_y);

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}
		}
		else//perfect diagonal
		{
			tx++;
			ty++;
			i++;
			printf("going diagonal: %d, %d\n", coord_x, coord_y);
			int old_coord_x = coord_x;
			coord_x = tileA->m_coord_x < tileB->m_coord_x ? coord_x + 1 : coord_x - 1;

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}

			coord_y = tileA->m_coord_y < tileB->m_coord_y ? coord_y + 1 : coord_y - 1;

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}

			if ((*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[old_coord_x][coord_y]->m_type != Water_Empty
				|| (*CurrentGame).m_waterzones[m_DMS.m_degree_x][m_DMS.m_degree_y]->m_watertiles[old_coord_x][coord_y]->m_can_be_seen == false)
			{
				printf("FAILED: %d, %d\n", coord_x, coord_y);
				return true;
			}
		}
	}

	return false;
}