#include "Agent.h"

extern Game* CurrentGame;

using namespace sf;

void Agent::Init()
{
	m_collider_type = AgentObject;
	m_max_current_awareness = 0;
	m_desired_angle = 0;
	m_desired_speed = sf::Vector2f(0,0);

	//Pathfind
	m_nb_tile_lines = NB_TILE_LINES;
	m_nb_tile_rows = NB_TILE_ROWS;
	m_tile_size = TILE_SIZE;

	//m_nb_tile_lines = (*CurrentGame).map_size.x / TILE_SIZE +1;
	//m_nb_tile_rows = (*CurrentGame).map_size.y / TILE_SIZE +1;
	m_current_tile = GetTileIndex((int)(getPosition().x / m_tile_size + 1), (int)(getPosition().y / m_tile_size + 1));
	
	m_tiles[16].m_type = Tile_Building;
	m_tiles[26].m_type = Tile_Building;
	m_tiles[28].m_type = Tile_Building;
	m_tiles[34].m_type = Tile_Building;

	FindShortedPath(14, 35);
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

	m_open_list_pathfind.clear();
	m_closed_list_pathfind.clear();
	m_current_path.clear();
}

void Agent::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
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

//PATHFIND
int Agent::GetTilePosX(size_t tile_index)
{
	int pos_x = (tile_index - 1) % m_nb_tile_rows + 1;
	return pos_x;
}

int Agent::GetTilePosY(size_t tile_index)
{
	int pos_y = (tile_index - 1) / m_nb_tile_rows + 1;
	//pos_y++;
	return pos_y;
}

size_t Agent::GetTileIndex(int pos_x, int pos_y)
{
	size_t index = pos_x;
	index += (pos_y - 1) * m_nb_tile_rows;
	return index;
}

void Agent::IteratePathFindingOnIndex(size_t index, size_t target_index)
{
	m_closed_list_pathfind.push_back(index);
	m_open_list_pathfind.remove(index);
	printf("New tile added to closed list: %d, posX=%d, posY=%d\n", index, GetTilePosX(index), GetTilePosY(index));

	const int pos1_x = GetTilePosX(index);
	const int pos1_y = GetTilePosY(index);

	//are we arrived at destination?
	for (int j = -1; j < 2; j++)
	{
		for (int i = -1; i < 2; i++)
		{
			if (pos1_x + i > 0 && pos1_x + i < m_nb_tile_lines + 1 && pos1_y + j > 0 && pos1_y + j < m_nb_tile_rows + 1)
			{
				size_t next_index = GetTileIndex(pos1_x + i, pos1_y + j);
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
			if (pos1_x + i > 0 && pos1_x + i < m_nb_tile_lines + 1 && pos1_y + j > 0 && pos1_y + j < m_nb_tile_rows + 1)
			{
				size_t next_index = GetTileIndex(pos1_x + i, pos1_y + j);
				if (m_tiles[next_index].m_type != Tile_Building)
				{
					//tiles that are legitimate to compute	
					if (find(m_closed_list_pathfind.begin(), m_closed_list_pathfind.end(), next_index) == m_closed_list_pathfind.end())//tile unknown until now
					{
						if (find(m_open_list_pathfind.begin(), m_open_list_pathfind.end(), next_index) == m_open_list_pathfind.end())
						{
							//CASE where the tile is not on the closed list nor on the open list
							m_open_list_pathfind.push_back(next_index);

							//compute Heuristic value
							int pos2_x = GetTilePosX(target_index);
							int pos2_y = GetTilePosY(target_index);

							int H_value_x = (pos1_x + i) > pos2_x ? (pos1_x + i) - pos2_x : pos2_x - (pos1_x + i);
							int H_value_y = (pos1_y + j) > pos2_y ? (pos1_y + j) - pos2_y : pos2_y - (pos1_y + j);
							m_tiles[next_index].m_heuristic = H_value_x + H_value_y;

							//compute Movement cost
							m_tiles[next_index].m_movement_cost = ((i < 0 && j < 0) || (i < 0 && j > 0) || (i > 0 && j < 0) || (i > 0 && j > 0)) ? 14 : 10;
							m_tiles[next_index].m_movement_cost += m_tiles[index].m_movement_cost;

							//G value
							m_tiles[next_index].m_G_value = m_tiles[next_index].m_heuristic + m_tiles[next_index].m_movement_cost;

							printf("New tile added to open list: %d, H=%d, F=%d, G=%d\n", next_index, m_tiles[next_index].m_heuristic, m_tiles[next_index].m_movement_cost, m_tiles[next_index].m_G_value);

							//parent node
							m_tiles[next_index].m_parent = index;
						}
						else
						{
							//CASE where the tile is already on the open list
							int movement_cost = ((i < 0 && j < 0) || (i < 0 && j > 0) || (i > 0 && j < 0) || (i > 0 && j > 0)) ? 14 : 10;//movement cost
							if (m_tiles[index].m_movement_cost + movement_cost < m_tiles[next_index].m_movement_cost)
							{
								m_tiles[next_index].m_parent = index;
								printf("Parent updated from index %d to index %d\n", next_index, index);
							}
						}
					}
				}
			}
		}
	}
}

void Agent::FindShortedPath(size_t start_index, size_t target_index)
{
	if (start_index == target_index || m_tiles[target_index].m_type == Tile_Building)
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
		m_current_path.push_back(way_point);
		way_point = m_tiles[way_point].m_parent;
	}
}