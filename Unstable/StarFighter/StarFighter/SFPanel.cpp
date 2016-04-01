#include "SFPanel.h"


//ACTION BOX
SFActionBox::SFActionBox(sf::Vector2f position, sf::Font* font, string action1, string action2, string action3, string action4)
{
	setPosition(position);

	float offset = 20;
	float text_margin = 4;

	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	
	for (int i = 0; i < 4; i++)
	{
		m_boxes[i].setSize(sf::Vector2f(24, 24));
		m_boxes[i].setOrigin(m_boxes[i].getSize().x / 2, m_boxes[i].getSize().y/2);
		
		m_texts[i].setFont(*font);
		m_texts[i].setCharacterSize(14);
		m_texts[i].setColor(Color::White);


		switch (i)
		{
			//down (A)
			case 0:
			{
				m_boxes[i].setPosition(sf::Vector2f(getPosition().x, getPosition().y - offset/2));
				m_boxes[i].setFillColor(sf::Color::Green);
				sf::Texture* texture = loader->loadTexture("Assets/2D/HUD/Button_A.png", 16, 16);
				m_boxes[i].setOrigin(m_boxes[i].getSize().x / 2, m_boxes[i].getSize().y / 2);
				m_boxes[i].setTexture(texture);

				m_texts[i].setString(action1);
				m_texts[i].setPosition(m_boxes[i].getPosition().x - m_texts[i].getGlobalBounds().width/2, m_boxes[i].getPosition().y + m_boxes[i].getSize().y / 2 + text_margin);
				break;
			}
			//left (X)
			case 1:
			{
				m_boxes[i].setPosition(sf::Vector2f(getPosition().x - offset, getPosition().y));
				m_boxes[i].setFillColor(sf::Color::Blue);
				sf::Texture* texture = loader->loadTexture("Assets/2D/HUD/Button_X.png", 16, 16);
				m_boxes[i].setOrigin(m_boxes[i].getSize().x / 2, m_boxes[i].getSize().y / 2);
				m_boxes[i].setTexture(texture);

				m_texts[i].setString(action2);
				m_texts[i].setPosition(m_boxes[i].getPosition().x - m_boxes[i].getSize().x / 2 - text_margin - m_texts[i].getGlobalBounds().width, m_boxes[i].getPosition().y);
				break;
			}
			//right (B)
			case 2:
			{
				m_boxes[i].setPosition(sf::Vector2f(getPosition().x + offset, getPosition().y));
				m_boxes[i].setFillColor(sf::Color::Red);
				sf::Texture* texture = loader->loadTexture("Assets/2D/HUD/Button_B.png", 16, 16);
				m_boxes[i].setOrigin(m_boxes[i].getSize().x / 2, m_boxes[i].getSize().y / 2);
				m_boxes[i].setTexture(texture);

				m_texts[i].setString(action3);
				m_texts[i].setPosition(m_boxes[i].getPosition().x + m_boxes[i].getSize().x / 2 + text_margin + m_texts[i].getGlobalBounds().width, m_boxes[i].getPosition().y);
				break;
			}
			//up (Y)
			case 3:
			{
				m_boxes[i].setPosition(sf::Vector2f(getPosition().x, getPosition().y + offset/2));
				m_boxes[i].setFillColor(sf::Color::Yellow);
				sf::Texture* texture = loader->loadTexture("Assets/2D/HUD/Button_Y.png", 16, 16);
				m_boxes[i].setOrigin(m_boxes[i].getSize().x / 2, m_boxes[i].getSize().y / 2);
				m_boxes[i].setTexture(texture);

				m_texts[i].setString(action4);
				m_texts[i].setPosition(m_boxes[i].getPosition().x - m_texts[i].getGlobalBounds().width / 2, m_boxes[i].getPosition().y - m_boxes[i].getSize().y / 2 - text_margin);
				break;
			}
		}
	}

	//setSize(sf::Vector2f(m_texts[1].getGlobalBounds().width + m_boxes[0].getSize().x + 2 * offset + m_texts[3].getGlobalBounds().width, m_texts[0].getGlobalBounds().height + offset + m_boxes[0].getSize().y + m_texts[3].getGlobalBounds().height));
	//setPosition(getPosition().x - getSize().x / 2, getPosition().y);
}

void SFActionBox::Draw(sf::RenderTexture& screen)
{
	for (int i = 0; i < 4; i++)
	{
		if (!m_texts[i].getString().isEmpty())
		{
			screen.draw(m_texts[i]);
			screen.draw(m_boxes[i]);
		}
	}
}


//PANEL
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
	//see override function in SFInventoryPanel and other types of SF panels
}

GameObject* SFPanel::GetFocusedItem()
{
	return NULL;
	//see override function in SFInventoryPanel and other types of SF panels
}

void SFPanel::SetFocusedItem(GameObject* item)
{
	//see override function in SFInventoryPanel and other types of SF panels
}

ObjectGrid* SFPanel::GetGrid(bool fake_grid, int grid)
{
	return NULL;
	//see override function in SFInventoryPanel and other types of SF panels
}

sf::Vector2i SFPanel::GetFocusedIndex()
{
	return sf::Vector2i(-1, -1);
	//see override function in SFInventoryPanel and other types of SF panels
}

int SFPanel::GetFocusedIntIndex()
{
	return -1;
	//see override function in SFInventoryPanel and other types of SF panels
}

int SFPanel::GetFocusedGrid()
{
	return -1;
	//see override function in SFInventoryPanel and other types of SF panels
}

bool SFPanel::GetPrioritaryFeedback()
{
	return false;
	//see override function in SFInventoryPanel and other types of SF panels
}

SFItemStatsPanel* SFPanel::GetItemStatsPanel()
{
	return NULL;
	//see override function in SFInventoryPanel and other types of SF panels
}

void SFPanel::SetItemStatsPanel(SFItemStatsPanel* panel)
{
	return;
	//see override function in SFInventoryPanel and other types of SF panels
}

void SFPanel::SetPrioritaryFeedback(bool has_priotiary_feedback)
{
	return;
	//see override function in SFInventoryPanel and other types of SF panels
}

float SFPanel::GetDuration()
{
	return -1;
	//see override function in SFDialogPanel and other types of SF panels
}

float SFPanel::GetDurationClockElpased()
{
	return -1;
	//see override function in SFDialogPanel and other types of SF panels
}

int SFPanel::GetItemsStatsPanelIndex()
{
	return -1;
	//see override function in SFTradePanel and other types of SF panels
}

void SFPanel::SetItemsStatsPanelIndex(int index)
{
	//see override function in SFTradePanel and other types of SF panels
}

int SFPanel::GetItemsStatsPanelNumberOfOptions()
{
	return -1;
	//see override function in SFTradePanel and other types of SF panels
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