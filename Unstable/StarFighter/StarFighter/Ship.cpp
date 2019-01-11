#include "Ship.h"

extern Game* CurrentGame;

Ship::Ship(DMS_Coord coord, ShipType type) : GameEntity(UI_EnemyShip)
{
	m_type = type;
	m_destination = NULL;
	m_seaport = NULL;

	//get on tile
	SetDMSCoord(coord);

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/enemy_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);

	setAnimation(texture, 1, 2);

	m_can_be_seen = false;

	//m_nb_crew[NB_CREW_TYPES];
	//m_nb_crew_working[NB_CREW_TYPES];
	//m_nb_crew_max;

	//vector<Room*> m_rooms;
	//vector<RoomConnexion*> m_connexions;
	//vector<CrewMember*> m_crew;

	m_speed = sf::Vector2f(0, 0);
	m_angle = -90;
}

Ship::~Ship()
{

}

WaterTile* Ship::GetWaterTileAtDMSCoord(DMS_Coord coord)
{
	WaterTile* tile = (*CurrentGame).m_waterzones[coord.m_degree_x][coord.m_degree_y]->m_watertiles[coord.m_minute_x][coord.m_minute_y];

	return tile;
}

bool Ship::SetDMSCoord(DMS_Coord coord)
{
	WaterTile* tile = Ship::GetWaterTileAtDMSCoord(coord);

	m_DMS = coord;
	m_tile = tile;

	//new seaport?
	if (tile->m_seaport != NULL && tile != (WaterTile*)m_seaport)
	{
		m_seaport = tile->m_seaport;
		m_seaport->m_ships.push_back(this);
	}

	return true;
}

void Ship::UpdatePosition(DMS_Coord warship_DMS)
{
	m_position.x = WATERTILE_OFFSET_X + WATERTILE_SIZE * (0.5f - (warship_DMS.m_minute_x + warship_DMS.m_second_x / 60) + NB_WATERTILE_VIEW_RANGE + (m_DMS.m_minute_x + m_DMS.m_second_x / 60));
	m_position.y = WATERTILE_OFFSET_Y + WATERTILE_SIZE * (0.5f - (NB_WATERTILE_Y - warship_DMS.m_minute_y - warship_DMS.m_second_y / 60) + NB_WATERTILE_VIEW_RANGE + NB_WATERTILE_Y - (m_DMS.m_minute_y + m_DMS.m_second_y / 60));//from bottom to top

	GameEntity::UpdatePosition();
}

void Ship::Update(Time deltaTime, DMS_Coord warship_DMS)
{
	m_can_be_seen = m_tile->m_can_be_seen;

	UpdatePosition(warship_DMS);
	UpdateAnimation();
}

void Ship::UpdateAnimation()
{
	if (m_angle < 0.f)
	{
		m_angle += 360.f;
	}
	m_angle = fmod(m_angle, 360.f);
	if (m_angle > 0.f && m_angle < 180.f)
	{
		setAnimationLine(0);//Boat facing right
	}
	else if (m_angle > 180.f)
	{
		setAnimationLine(1);//Boat facing left
	}
	setRotation(m_angle);
}

void Ship::GetAngleForSpeed(float& angle)
{
	//find angle for speed vector
	if (m_speed.x != 0 || m_speed.y != 0)
	{
		if (m_speed.x == 0)
		{
			angle = m_speed.y >= 0 ? 180.f : 0.f;
		}
		else if (m_speed.y == 0)
		{
			angle = m_speed.x >= 0 ? 90.f : 270.f;
		}
		else
		{
			if (m_speed.x >= 0)
			{
				angle = (atan(m_speed.y / m_speed.x) * 180.f / M_PI) + 90.f;
			}
			else
			{
				angle = (atan(m_speed.y / m_speed.x) * 180.f / M_PI) + 90.f + 180.f;
			}
		}
	}
	else
	{
		//default value
		angle = m_currentAnimationIndex == 0 ? 90.f : 270.f;
	}
}