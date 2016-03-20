#include "SFPanel.h"

SFPanel::SFPanel(sf::Vector2f size, SFPanelTypes panel_type)
{
	setSize(size);
	m_visible = true;
	m_panel_type = panel_type;
	m_playerShip = NULL;

	sf::Color _darkblue = sf::Color::Color(6, 87, 94, 255);//dark blue-green
	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//white

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

	//body text
	m_text.setCharacterSize(18);
	m_text.setColor(_white);
}

SFPanel::~SFPanel()
{
	m_playerShip = NULL;
}


bool SFPanel::IsCursorCollidingWithRectangle(GameObject& cursor, RectangleShape& object)
{
	if (!cursor.m_visible)
	{
		return false;
	}

	sf::IntRect boundsA(SimpleCollision::FToIRect(object.getGlobalBounds()));
	sf::IntRect boundsB(SimpleCollision::FToIRect(cursor.getGlobalBounds()));
	return boundsA.intersects(boundsB);
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

void SFPanel::SetFocusedItem(GameObject* item)
{

}

ObjectGrid* SFPanel::GetGrid(bool fake_grid, size_t grid)
{
	return NULL;
	//see override function in SFMenuPanel and other types of SF panels
}

sf::Vector2i SFPanel::GetFocusedIndex()
{
	return sf::Vector2i(-1, -1);
	//see override function in SFMenuPanel and other types of SF panels
}

int SFPanel::GetFocusedGrid()
{
	return -1;
	//see override function in SFMenuPanel and other types of SF panels
}

bool SFPanel::GetPrioritaryFeedback()
{
	return false;
	//see override function in SFMenuPanel and other types of SF panels
}

void SFPanel::SetPrioritaryFeedback(bool has_priotiary_feedback)
{
	return;
	//see override function in SFMenuPanel and other types of SF panels
}

int SFPanel::GetTeleportationCost()
{
	return -1;
	//see override function in SFMapPanel and other types of SF panels
}

void SFPanel::SetMapViewOffset(sf::Vector2f offset)
{

}

string SFPanel::GetTeleportationDestination()
{
	return "";
	//see override function in SFMapPanel and other types of SF panels
}

Dialog* SFPanel::GetDialog()
{
	return NULL;
	//see override function in SFDialogPanel
}

void SFPanel::ClearHighlight()
{
	return;
	//see override function in SFMenuPanel and other types of SF panels
}

void SFPanel::ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* fake_grid)
{
	return;
	//see override function in SFMenuPanel and other types of SF panels
}