#include "Ammo.h"

extern Game* CurrentGame;

Ammo::Ammo(AmmoType type, sf::Vector2f position, float angle, float distance_combat, Ship* target_ship, RoomTile* target_tile, sf::Vector2f target_position) : GameEntity(UI_None)
{
	m_type = type;
	m_angle = angle;
	setRotation(angle);
	m_distance_combat = distance_combat;
	m_target_tile = target_tile;
	m_target_position = target_position;
	m_phase = Shoot_Ougoing;
	m_FX_hit = new FX(FX_Explosion);
	m_FX_miss = new FX(FX_Splash);
	m_target_ship = target_ship;

	m_damage = CANNONBALL_DAMAGE;
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
	delete m_FX_miss;
}

void Ammo::Update(Time deltaTime)
{
	//apply movement
	m_position.x += m_speed.x * deltaTime.asSeconds();
	m_position.y += m_speed.y * deltaTime.asSeconds();

	//hitting target
	if (abs(m_position.x - m_target_position.x) < 16.f && abs(m_position.y - m_target_position.y) < 16.f)
	{
		//"boom"
		m_can_be_seen = false;
		
		if (m_target_tile != NULL)
		{
			FX* FX_hit = m_FX_hit->Clone();
			FX_hit->m_position = m_target_position;
			FX_hit->UpdatePosition();
			(*CurrentGame).m_FX.push_back(FX_hit);
			m_phase = Shoot_Hit;
		}
		else
		{
			FX* FX_miss = m_FX_miss->Clone();
			FX_miss->m_position = m_target_position;
			FX_miss->UpdatePosition();
			(*CurrentGame).m_FX.push_back(FX_miss);
		}
	}

	GameEntity::Update(deltaTime);
}