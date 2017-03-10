#include "Agent.h"

extern Game* CurrentGame;

using namespace sf;

void Agent::Init()
{
	m_collider_type = AgentObject;
	m_max_current_awareness = 0;
	m_desired_angle = 0;
	m_desired_speed = 0;
	m_desired_destination = sf::Vector2f(0, 0);

	//Pathfind
	const int tile_size = TILE_SIZE;
	const int nb_tile_lines = (int)(*CurrentGame).m_map_size.x / tile_size;
	const int nb_tile_rows = (int)(*CurrentGame).m_map_size.y / tile_size;
	for (size_t i = 0; i < nb_tile_lines * nb_tile_rows + 1; i++)
	{
		Tile new_tile;
		m_tiles.push_back(new_tile);
	}

	int posX = getPosition().x / tile_size + 1;
	int posY = getPosition().y / tile_size + 1;
	m_current_tile = Game::GetTileIndex(posX, posY);
}

Agent::Agent()
{
	Init();
}

Agent::Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Agent::Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	Init();
}

Agent::~Agent()
{
	for (std::vector<Item*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		delete (*it);
	}
	m_items.clear();

	GameObject* ship = (GameObject*)(*CurrentGame).m_playerShip;
	ship->RemoveFromAwarenessMap(this);

	m_tiles.clear();
	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();
	m_current_path.clear();
}

void Agent::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);

	const int m_tile_size = TILE_SIZE;
	int posX = getPosition().x / m_tile_size + 1;
	int posY = getPosition().y / m_tile_size + 1;
	m_current_tile = Game::GetTileIndex(posX, posY);
	//printf("current tile: %d, posX: %f, posY :%f\n", m_current_tile, getPosition().x, getPosition().y);

	//if (m_angleClock.getElapsedTime().asSeconds() > ANGLE_UPDATE_TIME)
	//{
	//	m_angleClock.restart();
	//	UpdateAngleToDestination();
	//}
}

void Agent::GenerateItems()
{
	int nb_items = RandomizeIntBetweenValues(1, MAX_ITEMS_PER_AGENT);
	for (int i = 0; i < nb_items; i++)
	{
		ItemType item_type = (ItemType)RandomizeIntBetweenValues(1, NB_ItemTypes-1);
		if (item_type != 0)
		{
			Item* new_item = new Item(item_type);
			m_items.push_back(new_item);
		}
	}
}

Stratagem* Agent::GetRandomStratagem(ItemType item_type)
{
	Stratagem* new_stratagem = new Stratagem(item_type);
	return new_stratagem;
}


bool Agent::TurnToDesiredAngle(sf::Time deltaTime)
{
	float cur_angle = getRotation();
	float delta = cur_angle - m_desired_angle;
	if (delta > 180)
		delta -= 360;
	else if (delta < -180)
		delta += 360;

	if (delta == 0)
	{
		return false;
	}
	else if (delta > 0)
	{
		if (abs(delta) > abs(m_angular_speed*deltaTime.asSeconds()))
		{
			rotate(-abs(m_angular_speed*deltaTime.asSeconds()));
			m_speed.x = 0;
			m_speed.y = 0;
		}
		else
		{
			setRotation(m_desired_angle);
			m_speed.x = 0;
			m_speed.y = 0;
		}
	}
	else if (delta < 0)
	{
		if (abs(delta) > abs(m_angular_speed)*deltaTime.asSeconds())
		{
			rotate(abs(m_angular_speed)*deltaTime.asSeconds());
			m_speed.x = 0;
			m_speed.y = 0;
		}
		else
		{
			setRotation(m_desired_angle);
			m_speed.x = 0;
			m_speed.y = 0;
		}
	}

	return true;
}

void Agent::GoToWaypoint(size_t index)
{
	const int m_tile_size = TILE_SIZE;
	//sf::Vector2f pos1 = sf::Vector2f(m_tile_size * Game::GetTilePosX(m_current_tile) - m_tile_size / 2, m_tile_size * Game::GetTilePosY(m_current_tile) - m_tile_size / 2);
	m_desired_destination = sf::Vector2f(m_tile_size * Game::GetTilePosX(index) - m_tile_size / 2, m_tile_size * Game::GetTilePosY(index) - m_tile_size / 2);
	float angle = GetAngleRadBetweenPositions(getPosition(), m_desired_destination);

	m_desired_angle = angle * 180 / M_PI - 180;
}

void Agent::UpdateAngleToDestination()
{
	float angle = GetAngleRadBetweenPositions(getPosition(), m_desired_destination);

	m_desired_angle = angle * 180 / M_PI - 180;
}

//PATHFIND
void Agent::IteratePathFindingOnIndex(size_t index, size_t target_index)
{
	m_closed_list_pathfind.push_back(index);
	m_open_list_pathfind.remove(index);
	//printf("New tile added to closed list: %d, posX=%d, posY=%d\n", index, Game::GetTilePosX(index), Game::GetTilePosY(index));

	const int pos1_x = Game::GetTilePosX(index);
	const int pos1_y = Game::GetTilePosY(index);

	//are we arrived at destination?
	const int tile_size = TILE_SIZE;
	const int nb_tile_lines = (int)(*CurrentGame).m_map_size.x / tile_size;
	const int nb_tile_rows = (int)(*CurrentGame).m_map_size.y / tile_size;
	//const int nb_tile_lines = NB_TILE_LINES;
	//const int nb_tile_rows = NB_TILE_ROWS;
	for (int j = -1; j < 2; j++)
	{
		for (int i = -1; i < 2; i++)
		{
			if (pos1_x + i > 0 && pos1_x + i < nb_tile_lines + 1 && pos1_y + j > 0 && pos1_y + j < nb_tile_rows + 1)
			{
				size_t next_index = Game::GetTileIndex(pos1_x + i, pos1_y + j);
				if (next_index == target_index)
				{
					m_tiles[target_index].m_parent = index;
					return;
				}
			}
		}
	}

	//...no, so go on.
	for (int j = -1; j < 2; j++)
	{
		for (int i = -1; i < 2; i++)
		{
			if (pos1_x + i > 0 && pos1_x + i < nb_tile_lines + 1 && pos1_y + j > 0 && pos1_y + j < nb_tile_rows + 1)
			{
				size_t next_index = Game::GetTileIndex(pos1_x + i, pos1_y + j);
				if ((*CurrentGame).m_tile_types[next_index] != Tile_Building)
				{
					//tiles that are legitimate to compute	
					if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), next_index) == m_closed_list_pathfind.end())//tile unknown until now
					{
						if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), next_index) == m_open_list_pathfind.end())
						{
							//CASE where the tile is not on the closed list nor on the open list
							m_open_list_pathfind.push_back(next_index);

							//compute Heuristic value
							int pos2_x = Game::GetTilePosX(target_index);
							int pos2_y = Game::GetTilePosY(target_index);

							int H_value_x = (pos1_x + i) > pos2_x ? (pos1_x + i) - pos2_x : pos2_x - (pos1_x + i);
							int H_value_y = (pos1_y + j) > pos2_y ? (pos1_y + j) - pos2_y : pos2_y - (pos1_y + j);
							m_tiles[next_index].m_heuristic = H_value_x + H_value_y;

							//compute Movement cost
							m_tiles[next_index].m_movement_cost = ((i < 0 && j < 0) || (i < 0 && j > 0) || (i > 0 && j < 0) || (i > 0 && j > 0)) ? 14 : 10;
							m_tiles[next_index].m_movement_cost += m_tiles[index].m_movement_cost;

							//G value
							m_tiles[next_index].m_G_value = m_tiles[next_index].m_heuristic + m_tiles[next_index].m_movement_cost;

							//printf("New tile added to open list: %d, H=%d, F=%d, G=%d\n", next_index, m_tiles[next_index].m_heuristic, m_tiles[next_index].m_movement_cost, m_tiles[next_index].m_G_value);

							//parent node
							m_tiles[next_index].m_parent = index;
						}
						else
						{
							//CASE where the tile is already on the open list
							int movement_cost = ((i < 0 && j < 0) || (i < 0 && j > 0) || (i > 0 && j < 0) || (i > 0 && j > 0)) ? 14 : 10;//movement cost
							if (m_tiles[index].m_movement_cost + movement_cost < m_tiles[next_index].m_movement_cost)
							{
								m_tiles[next_index].m_parent = index;//comment to remove diagonal movements
								//printf("Parent updated from index %d to index %d\n", next_index, index);
							}
						}
					}
				}
			}
		}
	}
}

void Agent::FindShortestPath(size_t start_index, size_t target_index)
{
	if (start_index == target_index || (*CurrentGame).m_tile_types[target_index] == Tile_Building)
	{
		return;
	}

	//start
	m_open_list_pathfind.push_back(start_index);
	while (!m_open_list_pathfind.empty() && m_tiles[target_index].m_parent == 0)
	{
		//choose next best tile to compute
		size_t min_G_value = 0;
		size_t next_index = 0;
		for (list<size_t>::iterator it = m_open_list_pathfind.begin(); it != m_open_list_pathfind.end(); it++)
		{
			if (m_tiles[*it].m_G_value < min_G_value || min_G_value == 0)
			{
				min_G_value = m_tiles[*it].m_G_value;
				next_index = *it;
			}
		}

		//compute this tile
		IteratePathFindingOnIndex(next_index, target_index);
	}

	//path found
	m_current_path.clear();
	size_t way_point = target_index;
	while (way_point != start_index)
	{
		m_current_path.push_front(way_point);
		if (m_tiles[way_point].m_parent == 0)
		{
			printf("BUG waypoint = 0, child tile: %d\n", way_point);
		}
		way_point = m_tiles[way_point].m_parent;
	}

	//clear data
	size_t tilesVectorSize = m_tiles.size();
	for (size_t i = 1; i < tilesVectorSize; i++)
	{
		m_tiles[i].m_heuristic = 0;
		m_tiles[i].m_movement_cost = 0;
		m_tiles[i].m_G_value = 0;
		m_tiles[i].m_parent = 0;
	}
	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();
}

void Agent::FindShortestPathTo(size_t target_index)
{
	const int tile_size = TILE_SIZE;
	int posX = getPosition().x / tile_size + 1;
	int posY = getPosition().y / tile_size + 1;
	m_current_tile = Game::GetTileIndex(posX, posY);

	FindShortestPath(m_current_tile, target_index);
}