#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(AmmoType type, sf::Vector2f position, float angle, float distance_combat) : GameEntity(UI_None)
{
	m_type = type;
	m_angle = angle;
	setRotation(angle);
	m_distance_combat = distance_combat;
	m_phase = Shoot_Ougoing;

	m_ref_speed = CANNONBALL_SPEED;

	m_can_be_seen = true;

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/cannonball.png", (int)CANNONBALL_SIZE, (int)CANNONBALL_SIZE);

	setAnimation(texture, 1, 1);

	float tile_size = ROOMTILE_SIZE;
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

	switch (m_phase)
	{
		case Shoot_Ougoing:
		{
			if (m_position.y < -m_size.y * 0.5f)
			{
				m_phase = Shoot_Warping;
				m_warp_timer = m_distance_combat / AMMO_WARP_DISTANCE_PER_SECOND * m_ref_speed / CANNONBALL_SPEED;
			}
			break;
		}
		case Shoot_Warping:
		{
			if (m_warp_timer > 0)
			{
				m_warp_timer -= deltaTime.asSeconds();
			}

			if (m_warp_timer < 0)
			{
				m_phase = Shoot_Incoming;

				m_position.x = 1500.f;
				m_position.y = 1000.f;
			}
			break;
		}
		case Shoot_Incoming:
		{
			break;
		}
	}

	//out of sight? = destroy it.
	/*
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
		if (m_position.x + m_size.x * 0.5f >= WATERTILE_OFFSET_X || m_position.x < -m_size.x * 0.5f || m_position.y < -m_size.y * 0.5f || m_position.y + m_size.y * 0.5f >(*CurrentGame).m_screen_size.y)
		{
			m_can_be_seen = false;
		}
	}
	*/

	GameEntity::Update(deltaTime);
}