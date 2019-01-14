#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(AmmoType type, sf::Vector2f position, float angle, float distance_combat, Ship* target_ship, RoomTile* target_tile) : GameEntity(UI_None)
{
	m_type = type;
	m_angle = angle;
	setRotation(angle);
	m_distance_combat = distance_combat;
	m_target_tile = target_tile;
	m_phase = Shoot_Ougoing;
	m_FX_hit = new FX();
	m_target_ship = target_ship;

	m_ref_speed = CANNONBALL_SPEED;
	m_radius = CANNONBALL_RADIUS;

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

Ammo::~Ammo()
{
	delete m_FX_hit;
}

void Ammo::Update(Time deltaTime)
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

				m_position.x = m_target_tile->m_position.x;// ROOMTILE_MINI_OFFSET_X;
				m_position.y = (*CurrentGame).m_screen_size.y + m_size.y * 0.5f;
			}
			break;
		}
		case Shoot_Incoming:
		{
			//hitting target
			if (abs(m_position.x - m_target_tile->m_position.x) < 16.f && abs(m_position.y - m_target_tile->m_position.y) < 16.f)
			{
				//"boom"
				m_can_be_seen = false;
				FX* FX_hit = m_FX_hit->Clone();
				FX_hit->m_position = m_target_tile->m_position;
				FX_hit->UpdatePosition();
				(*CurrentGame).m_FX.push_back(FX_hit);

				m_phase = Shoot_Hit;
			}

			break;
		}
	}

	GameEntity::Update(deltaTime);
}