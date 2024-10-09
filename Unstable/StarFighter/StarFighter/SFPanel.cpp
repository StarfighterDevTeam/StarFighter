#include "SFPanel.h"


//ACTION BOX
SFActionBox::SFActionBox(sf::Font* font)
{
	for (int i = 0; i < NBVAL_ActionButtons; i++)
	{
		string filename;
		switch (i)
		{
			case ActionButton_A:
			{
				filename = INTERACTION_BUTTON_A_FILENAME;
				break;
			}
			case ActionButton_X:
			{
				filename = INTERACTION_BUTTON_X_FILENAME;
				break;
			}
			case ActionButton_Y:
			{
				filename = INTERACTION_BUTTON_Y_FILENAME;
				break;
			}
			case ActionButton_B:
			{
				filename = INTERACTION_BUTTON_B_FILENAME;
				break;
			}
		}

		assert(!filename.empty());

		m_boxes[i] = GameObject(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), filename, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT), sf::Vector2f(INTERACTION_BUTTON_WIDTH * 0.5f, INTERACTION_BUTTON_HEIGHT * 0.5f));

		m_texts[i].setFont(*font);
		m_texts[i].setCharacterSize(18);
		m_texts[i].setFillColor(Color::White);
	}
}

void SFActionBox::SetString(string button_name, ActionButtons button)
{
	m_texts[button].setString(button_name);
}

void SFActionBox::SetPosition(sf::Vector2f position)
{
	int j = 0;
	for (int i = 0; i < NBVAL_ActionButtons; i++)
	{
		if (!m_texts[i].getString().isEmpty())
		{
			m_boxes[i].setPosition(position.x + m_boxes[i].m_size.x * 0.5f, position.y + m_boxes[i].m_size.y * 0.5f + (m_boxes[i].m_size.y * 0.5f + INTERACTION_INTERBLOCK) * j);
			m_texts[i].setPosition(m_boxes[i].getPosition().x + m_boxes[i].m_size.x + INTERACTION_BUTTON_MARGIN, m_boxes[i].getPosition().y - m_texts[i].getGlobalBounds().height/2 - 3);//because fuck this
			j++;
		}	
	}
}

void SFActionBox::Draw(sf::RenderTexture& screen)
{
	for (int i = 0; i < NBVAL_ActionButtons; i++)
	{
		if (!m_texts[i].getString().isEmpty())
		{
			screen.draw(m_texts[i]);
			screen.draw(m_boxes[i]);
		}
	}
}

//ACTION BOX WITH SELECTION
SFActionBoxWithSelection::SFActionBoxWithSelection()
{
	m_box = GameObject(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), INTERACTION_BUTTON_A_FILENAME, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT), sf::Vector2f(INTERACTION_BUTTON_WIDTH * 0.5f, INTERACTION_BUTTON_HEIGHT * 0.5f));
	m_selected_index = -1;
}

void SFActionBoxWithSelection::AddOption(string option_name, sf::Font* font)
{
	sf::Text new_option;
	new_option.setFont(*font);
	new_option.setCharacterSize(18);
	new_option.setFillColor(sf::Color::White);
	new_option.setString(option_name);

	m_texts.push_back(new_option);
}

void SFActionBoxWithSelection::SetSelectedAction(int index)
{
	m_selected_index = index;
	m_box.setPosition(m_texts[m_selected_index].getPosition().x - m_box.m_size.x - INTERACTION_BUTTON_MARGIN, m_texts[m_selected_index].getPosition().y + m_box.m_size.y - 5);//because fuck this
}

void SFActionBoxWithSelection::SetPosition(sf::Vector2f position)
{
	int j = 0;
	int buttonsVectorSize = m_texts.size();
	for (int i = 0; i < buttonsVectorSize; i++)
	{
		if (!m_texts[i].getString().isEmpty())
		{
			if (m_selected_index == j)
			{
				m_box.setPosition(position.x + m_box.m_size.x * 0.5f, position.y + m_box.m_size.y * 0.5f + (m_box.m_size.y * 0.5f + INTERACTION_INTERBLOCK) * j);
			}

			m_texts[i].setPosition(position.x + m_box.m_size.x + m_box.m_size.x * 0.5f + INTERACTION_BUTTON_MARGIN, position.y + m_texts[i].getGlobalBounds().height * 0.5f + (m_box.m_size.y * 0.5f + INTERACTION_INTERBLOCK) * j);
			j++;
		}
	}
}

void SFActionBoxWithSelection::Draw(sf::RenderTexture& screen)
{
	size_t optionsVectorSize = m_texts.size();
	if (optionsVectorSize > 0)
	{
		screen.draw(m_box);
	}
	for (size_t i = 0; i < optionsVectorSize; i++)
	{
		if (!m_texts[i].getString().isEmpty())
		{
			screen.draw(m_texts[i]);
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
	m_actions = NULL;

	sf::Color _darkblue = sf::Color::Color(6, 87, 94, 255);//dark blue-green
	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//white

	setSize(size);
	setOrigin(sf::Vector2f(size.x * 0.5f, size.y * 0.5f));
	setFillColor(sf::Color(10, 10, 10, 230));//dark grey
	setOutlineThickness(2);
	setOutlineColor(_darkblue);//dark blue-green

	//title text
	m_title_text.setCharacterSize(20);
	m_title_text.setFillColor(_yellow);

	//body text
	m_text.setCharacterSize(18);
	m_text.setFillColor(_white);
}

SFPanel::~SFPanel()
{
	delete m_actions;
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

void SFPanel::Update(sf::Time deltaTime)
{
	//see override function in specific types of SFPanels
}

void SFPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		screen.draw(*this);
	}
}