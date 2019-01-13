#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(AmmoType type, sf::Vector2f position, float angle, MapView view) : GameEntity(UI_None)
{
	m_type = type;
	m_angle = angle;
	setRotation(angle);
	m_view = view;

	m_ref_speed = CANNONBALL_SPEED;

	m_can_be_seen = true;

	//shape for water tiles
	sf::Texture* texture = TextureLoader::getInstance()->loadTexture("2D/cannonball.png", (int)CANNONBALL_SIZE, (int)CANNONBALL_SIZE);

	setAnimation(texture, 1, 1);

	float tile_size = view == Map_Rooms ? ROOMTILE_SIZE : WATERTILE_SIZE;
	float angle_rad = angle * M_PI / 180.f;
	m_speed = sf::Vector2f(m_ref_speed * sin(angle_rad), - m_ref_speed * cos(angle_rad));
	float weapon_offset_x = (tile_size + m_size.x) * 0.5f * sin(angle_rad);
	float weapon_offset_y = -(tile_size + m_size.y) * 0.5f * cos(angle_rad);
	m_position = position + sf::Vector2f(weapon_offset_x, weapon_offset_y);
}

void Ammo::Update(Time deltaTime, DMS_Coord warship_DMS)
{
	//apply movement
	m_position.x += m_speed.x * deltaTime.asSeconds();
	m_position.y += m_speed.y * deltaTime.asSeconds();

	//out of sight? = destroy it.
	if (m_view == Map_Water)
	{
		DMS_Coord coord = WaterTile::GetDMSCoord(m_position, warship_DMS);
		if (coord.m_minute_x > 0 && m_speed.x < 0)//going left? check one "minute" further if possible
		{
			coord.m_minute_x++;
		}
		if (coord.m_minute_y > 0 && m_speed.y > 0)//going down? check one "minute" further if possible
		{
			coord.m_minute_y++;
		}

		WaterTile* tile = (*CurrentGame).m_waterzones[coord.m_degree_x][coord.m_degree_y]->m_watertiles[coord.m_minute_x][coord.m_minute_y];
		m_can_be_seen = tile->m_can_be_seen;
	}
	else if (m_view == Map_Rooms)
	{
		if (m_position.x + m_size.x * 0.5f >= ROOMS_RIGHT_CORNER_X || m_position.x < -m_size.x * 0.5f || m_position.y < -m_size.y * 0.5f || m_position.y + m_size.y * 0.5f >(*CurrentGame).m_screen_size.y)
		{
			m_can_be_seen = false;
		}
	}

	GameEntity::Update(deltaTime);
}