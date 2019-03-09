#include "Rudder.h"

extern Game* CurrentGame;

Rudder::Rudder() : GameEntity(UI_Rudder)
{
	m_health_max = ENGINE_HEALTH_MAX;
	m_health = m_health_max;
	m_tile = NULL;
	m_ship = NULL;

	m_rotation_speed = 0.f;
	m_rotation_timer = 0.f;

	//shape for water tiles
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/rudder_icon.png", (int)ROOMTILE_SIZE, (int)ROOMTILE_SIZE * 2);

	setAnimation(texture, 1, 1);

	//UI
	m_default_color = sf::Color(0, 0, 0, 0);

	m_shape_container.setSize(sf::Vector2f(m_size));
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(m_default_color);
	m_shape_container.setFillColor(m_default_color);

	m_lifebar = new GameEntity(UI_None);
	m_lifebar->m_shape_container.setSize(sf::Vector2f(LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setOrigin(sf::Vector2f(LIFEBAR_SIZE_X * 0.5f, LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_lifebar->m_shape_container.setOutlineThickness(1.f);
	m_lifebar->m_shape_container.setOutlineColor(sf::Color::Black);

	m_lifebar->m_shape.setSize(sf::Vector2f(LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
	m_lifebar->m_shape.setOrigin(sf::Vector2f(LIFEBAR_SIZE_X * 0.5f, LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape.setFillColor(sf::Color::Green);

	m_systembar = new GameEntity(UI_None);
	m_systembar->m_shape_container.setSize(sf::Vector2f(LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
	m_systembar->m_shape_container.setOrigin(sf::Vector2f(LIFEBAR_SIZE_X * 0.5f, LIFEBAR_SIZE_Y * 0.5f));
	m_systembar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_systembar->m_shape_container.setOutlineThickness(1.f);
	m_systembar->m_shape_container.setOutlineColor(sf::Color::Black);

	m_systembar->m_shape.setSize(sf::Vector2f(LIFEBAR_SIZE_X, LIFEBAR_SIZE_Y));
	m_systembar->m_shape.setOrigin(sf::Vector2f(LIFEBAR_SIZE_X * 0.5f, LIFEBAR_SIZE_Y * 0.5f));
	m_systembar->m_shape.setFillColor(sf::Color(255, 242, 39, 255));//yellow "rof");
}

Rudder::~Rudder()
{
	delete m_lifebar;
	delete m_systembar;
}

void Rudder::Update(Time deltaTime)
{
	m_position = m_tile->m_position;

	if (m_tile->m_room->m_alliance == Alliance_Player)
	{
		GameEntity::Update(deltaTime);
	}
	else
	{
		GameEntity::UpdatePosition();
	}
}