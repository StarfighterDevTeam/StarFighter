#include "SFPanel.h"

SFPanel::SFPanel(sf::Vector2f size, SFPanelTypes panel_type)
{
	setSize(size);
	m_visible = true;
	m_panel_type = panel_type;
	m_playerShip = NULL;

	sf::Color _darkblue = sf::Color::Color(6, 87, 94, 255);//dark blue-green

	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow

	setSize(size);
	setOrigin(sf::Vector2f(size.x / 2, size.y / 2));
	setFillColor(sf::Color(10, 10, 10, 230));//dark grey
	setOutlineThickness(2);
	setOutlineColor(_darkblue);//dark blue-green

	//title text
	m_title_text.setCharacterSize(20);
	m_title_text.setColor(_yellow);

	//actions text
	m_actions_text.setCharacterSize(18);
	m_actions_text.setColor(_yellow);
}

SFPanel::~SFPanel()
{
	m_playerShip = NULL;
}

void SFPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//see override function in SFMenuPanel and other types of SF panels
}

void SFPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		//if (!m_title_text.getString().isEmpty())
		//	m_title_text.draw(screen);
		//sf::Text m_text;
		//sf::Text m_actions_text;

		screen.draw(*this);
	}
	//see override function in SFMenuPanel and other types of SF panels
}

void SFPanel::SetSelectedOptionIndex(int index)
{
	//see override function in SFMenuPanel and other types of SF panels
}

int SFPanel::GetSelectedOptionIndex()
{
	return -1;
	//see override function in SFMenuPanel and other types of SF panels
}

GameObject* SFPanel::GetCursor()
{
	return NULL;
	//see override function in SFMenuPanel and other types of SF panels
}

GameObject* SFPanel::GetFocusedItem()
{
	return NULL;
	//see override function in SFMenuPanel and other types of SF panels
}

ObjectGrid* SFPanel::GetGrid()
{
	return NULL;
	//see override function in SFMenuPanel and other types of SF panels
}

sf::Vector2i SFPanel::GetFocusedIndex()
{
	return sf::Vector2i(-1, -1);
	//see override function in SFMenuPanel and other types of SF panels
}