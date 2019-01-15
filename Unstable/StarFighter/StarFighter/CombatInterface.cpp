#include "CombatInterface.h"

extern Game* CurrentGame;

CombatInterface::CombatInterface()
{
	
}

CombatInterface::~CombatInterface()
{

}

void CombatInterface::Init(Ship* ship, Ship* enemy_ship)
{
	m_ship = ship;
	m_enemy_ship = enemy_ship;
	
	for (int i = 0; i < 2; i++)
	{
		//life bars
		float offset = i == 0 ? COMBAT_LIFEBAR_OFFSET_X : COMBAT_LIFEBAR_MINI_OFFSET_X;
		m_lifebars[i] = new GameEntity(UI_None);
		m_lifebars[i]->m_shape_container.setSize(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));
		m_lifebars[i]->m_shape_container.setOrigin(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X * 0.5f, COMBAT_LIFEBAR_SIZE_Y * 0.5f));
		m_lifebars[i]->m_shape_container.setPosition(sf::Vector2f(offset + COMBAT_LIFEBAR_SIZE_X * 0.5, COMBAT_LIFEBAR_OFFSET_Y - COMBAT_LIFEBAR_SIZE_Y * 0.5f));
		m_lifebars[i]->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
		m_lifebars[i]->m_shape_container.setOutlineThickness(2.f);
		m_lifebars[i]->m_shape_container.setOutlineColor(sf::Color::Black);
		sf::Vector2f lifebar_pos = m_lifebars[i]->m_shape_container.getPosition();

		m_lifebars[i]->m_shape.setSize(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));
		m_lifebars[i]->m_shape.setOrigin(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X * 0.5f, COMBAT_LIFEBAR_SIZE_Y * 0.5f));
		m_lifebars[i]->m_shape.setPosition(lifebar_pos);
		m_lifebars[i]->m_shape.setFillColor(sf::Color::Green);
		m_lifebars[i]->m_shape.setOutlineThickness(2.f);
		m_lifebars[i]->m_shape.setOutlineColor(sf::Color::Black);

		//flood bars
		m_floodbars[i] = new GameEntity(UI_None);
		m_floodbars[i]->m_shape_container.setSize(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
		m_floodbars[i]->m_shape_container.setOrigin(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X * 0.5f, COMBAT_FLOODBAR_SIZE_Y * 0.5f));
		m_floodbars[i]->m_shape_container.setPosition(sf::Vector2f(offset + COMBAT_FLOODBAR_SIZE_X * 0.5, m_lifebars[i]->m_shape_container.getPosition().y + (COMBAT_LIFEBAR_SIZE_Y + COMBAT_FLOODBAR_SIZE_Y + m_lifebars[i]->m_shape_container.getOutlineThickness()) * 0.5f));
		m_floodbars[i]->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
		m_floodbars[i]->m_shape_container.setOutlineThickness(2.f);
		m_floodbars[i]->m_shape_container.setOutlineColor(sf::Color::Black);
		sf::Vector2f floodbar_pos = m_floodbars[i]->m_shape_container.getPosition();

		m_floodbars[i]->m_shape.setSize(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
		m_floodbars[i]->m_shape.setOrigin(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X * 0.5f, COMBAT_FLOODBAR_SIZE_Y * 0.5f));
		m_floodbars[i]->m_shape.setPosition(floodbar_pos);
		m_floodbars[i]->m_shape.setFillColor(sf::Color(0, 100, 170, 255));//blue "water"
		m_floodbars[i]->m_shape.setOutlineThickness(2.f);
		m_floodbars[i]->m_shape.setOutlineColor(sf::Color::Black);
	}
}


void CombatInterface::Update(sf::Time deltaTime)
{
	float threshold[3] = { 0.7, 0.5, 0.2 };

	for (int i = 0; i < 2; i++)
	{
		//size of life bar
		int health = i == 0 ? m_ship->m_health : m_enemy_ship->m_health;
		Bound(health, sf::Vector2i(0, m_ship->m_health_max));

		float life_ratio = i == 0 ? 1.0f * health / m_ship->m_health_max : 1.0f * m_enemy_ship->m_health / m_enemy_ship->m_health_max;

		m_lifebars[i]->m_shape.setSize(sf::Vector2f(life_ratio * COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));

		//life bar color
		if (life_ratio >= threshold[1])
		{
			m_lifebars[i]->m_shape.setFillColor(sf::Color::Green);
		}
		else if (life_ratio >= threshold[2])
		{
			m_lifebars[i]->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "damaged"
		}
		else
		{
			m_lifebars[i]->m_shape.setFillColor(sf::Color::Red);
		}

		//size of flood bar
		int flood = i == 0 ? m_ship->m_flood : m_enemy_ship->m_flood;
		Bound(flood, sf::Vector2i(0, m_ship->m_flood_max));

		float flood_ratio = i == 0 ? 1.0f * flood / m_ship->m_flood_max : 1.0f * flood / m_enemy_ship->m_flood_max;

		m_floodbars[i]->m_shape.setSize(sf::Vector2f(flood_ratio * COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
	}
}

void CombatInterface::Draw(sf::RenderTexture& screen)
{
	//life bars
	for (int i = 0; i < 2; i++)
	{
		m_floodbars[i]->Draw(screen);
		m_lifebars[i]->Draw(screen);
		
	}
}