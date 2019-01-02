#include "GameEntity.h"

extern Game* CurrentGame;

GameEntity::GameEntity()
{
	m_hovered = false;
	m_selected = false;
	m_default_color = sf::Color::White;
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

	//get hovered state
	if ((*CurrentGame).m_window_has_focus == true
		&& (*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - m_shape_container.getSize().x / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + m_shape_container.getSize().x / 2
		&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - m_shape_container.getSize().y / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + m_shape_container.getSize().y / 2)
	{
		if ((*CurrentGame).m_hovered_ui != NULL && (*CurrentGame).m_hovered_ui != this)
		{
			(*CurrentGame).m_hovered_ui->m_hovered = false;
			(*CurrentGame).m_hovered_ui->m_shape_container.setOutlineColor(sf::Color::White);
		}
		m_hovered = true;
		(*CurrentGame).m_hovered_ui = this;
	}
	else
	{
		m_hovered = false;
	}

	//get selected state
	if (mouse_click == Mouse_LeftClick)
	{
		if (m_hovered == true)
		{
			if ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui != this)
			{
				(*CurrentGame).m_selected_ui->m_selected = false;
				(*CurrentGame).m_selected_ui->m_shape_container.setOutlineColor(sf::Color::White);
			}
			
			(*CurrentGame).m_selected_ui = this;
			m_selected = true;
		}
		else
		{
			m_selected = false;
		}
	}

	//apply color feedback
	m_shape_container.setOutlineColor(m_default_color);

	if (m_hovered)
	{
		m_shape_container.setOutlineColor(sf::Color::Red);
	}
	if (m_selected)
	{
		m_shape_container.setOutlineColor(sf::Color::Green);
	}
}

void GameEntity::Draw(sf::RenderTexture& screen)
{
	screen.draw(this->m_shape_container);
	screen.draw(this->m_shape);
	screen.draw(this->m_text);
};