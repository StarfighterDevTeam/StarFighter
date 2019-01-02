#include "GameEntity.h"

extern Game* CurrentGame;

GameEntity::GameEntity()
{
	m_hovered = false;
	m_selected = false;
}

GameEntity::GameEntity(sf::Vector2f size) : GameEntity()
{
	m_size = size;
}

void GameEntity::Update(Time deltaTime)
{
	//update position
	m_shape_container.setPosition(m_position);
	m_shape.setPosition(m_position);
	m_text.SetPosition(m_position);

	//get inputs
	MouseAction& mouse_click = (*CurrentGame).m_mouse_click;

	if ((*CurrentGame).m_window_has_focus == true
		&& (*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - m_shape_container.getSize().x / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + m_shape_container.getSize().x / 2
		&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - m_shape_container.getSize().y / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + m_shape_container.getSize().y / 2)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_ui = this;
	}
	else
	{
		m_hovered = false;
	}

	if (mouse_click == Mouse_LeftClick)
	{
		if (m_hovered)
		{
			m_selected = true;
			(*CurrentGame).m_selected_ui = this;
		}
		else
		{
			m_selected = false;
		}
	}

	m_shape_container.setOutlineColor(sf::Color::White);
	if (m_hovered)
	{
		m_shape_container.setOutlineColor(sf::Color::Red);
	}
	if (m_selected)
	{
		m_shape_container.setOutlineColor(sf::Color::Green);
	}

	//Actions
	if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_ui != NULL)
	{

	}
	else if (m_hovered && mouse_click == Mouse_RightClick)
	{

	}
	else if (m_hovered && mouse_click == Mouse_LeftClick)
	{
		
	}
}

void GameEntity::Draw(sf::RenderTexture& screen)
{
	screen.draw(this->m_shape_container);
	screen.draw(this->m_shape);
	screen.draw(this->m_text);
};