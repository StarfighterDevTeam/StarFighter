#include "SFStratagemPanel.h"

extern Game* CurrentGame;

#define STRATAGEM_PANEL_SPACE_BETWEEN_LINES				10
#define STRATAGEM_PANEL_SPACE_BETWEEN_CODES				4
#define STRATAGEM_PANEL_SPACE_AFTER_ITEM_IMAGE			26
#define STRATAGEM_PANEL_SPACE_AFTER_ITEM_NAME			100

ItemBox::ItemBox()
{
	m_item = NULL;
	m_item_image = NULL;
	for (int i = 0; i < MAX_CODES; i++)
	{
		m_codes[i] = NULL;
	}

	m_current_code_index = 0;
}

void ItemBox::Init(Item* item, GameObject& code_sprite)
{
	if (!item)
	{
		return;
	}

	m_item = item;
	m_item_image = item->Clone();

	m_item_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_item_text.setCharacterSize(16);
	m_item_text.setColor(sf::Color::White);

	ostringstream ss;
	ss << m_item->m_display_name;
	m_item_text.setString(ss.str());

	if (item->m_stratagem)
	{
		for (size_t i = 0; i < MAX_CODES; i++)
		{
			if (m_item->m_stratagem->m_code[i] > 0)
			{
				m_codes[i] = code_sprite.Clone();
				m_codes[i]->setRotation(90 * (m_item->m_stratagem->m_code[i] - 1));
			}
			else
			{
				m_codes[i] = NULL;
			}
		}
	}
}

void ItemBox::Draw(sf::RenderTexture& screen)
{
	if (!m_item)
	{
		return;
	}
	screen.draw(m_item_text);
	if (m_item_image)
	{
		screen.draw(*m_item_image);
	}

	for (size_t i = 0; i < MAX_CODES; i++)
	{
		if (m_codes[i])
		{
			screen.draw(*m_codes[i]);
		}
	}
}

void ItemBox::SetPosition(sf::Vector2f position)
{
	float offset_x = 0;
	offset_x += ITEM_SIZE / 2;
	m_item_image->setPosition(sf::Vector2f(position.x + offset_x, position.y + ITEM_SIZE));

	offset_x += STRATAGEM_PANEL_SPACE_AFTER_ITEM_IMAGE;
	m_item_text.setPosition(sf::Vector2f(position.x + offset_x, position.y + ITEM_SIZE / 2 + 5));

	offset_x += STRATAGEM_PANEL_SPACE_AFTER_ITEM_NAME + CODE_SIZE / 2;
	for (size_t i = 0; i < MAX_CODES; i++)
	{
		if (m_codes[i])
		{
			offset_x += STRATAGEM_PANEL_SPACE_BETWEEN_CODES + CODE_SIZE;
			m_codes[i]->setPosition(sf::Vector2f(position.x + offset_x, position.y + ITEM_SIZE / 2 + CODE_SIZE/2));
		}
	}
}

ItemBox::~ItemBox()
{
	m_item = NULL;
	delete m_item_image;
	for (size_t i = 0; i < MAX_CODES; i++)
	{
		if (m_codes[i])
		{
			delete m_codes[i];
		}
	}
}

void ItemBox::CopyAndDelete(ItemBox& item_to_copy)
{
	//copy
	m_item = item_to_copy.m_item;
	for (size_t i = 0; i < MAX_CODES; i++)
	{
		m_codes[i] = item_to_copy.m_codes[i];
	}
	m_item_image = item_to_copy.m_item_image;
	m_item_text = item_to_copy.m_item_text;
	m_current_code_index = item_to_copy.m_current_code_index;

	//delete
	item_to_copy.m_item = NULL;
	for (size_t i = 0; i < MAX_CODES; i++)
	{
		item_to_copy.m_codes[i] = NULL;
	}
	item_to_copy.m_item_image = NULL;
	item_to_copy.m_item_text.setString("");
	item_to_copy.m_current_code_index = 0;
}

// PANEL
SFStratagemPanel::SFStratagemPanel(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	if (!playerShip || !playerShip->m_current_collision)
	{
		return;
	}

	if (playerShip->m_current_collision->m_collider_type != AgentObject)
	{
		return;
	}

	m_current_stratagem = NULL;
	for (int i = 0; i < MAX_CODES; i++)
	{
		m_current_code[i] = 0;
	}

	m_code = GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/arrow.png", sf::Vector2f(CODE_SIZE, CODE_SIZE), sf::Vector2f(CODE_SIZE / 2, CODE_SIZE / 2), 1, NBVAL_CodeAnimations);

	Agent* agent = (Agent*)playerShip->m_current_collision;

	m_playerShip = playerShip;

	//title
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	ostringstream ss_title;
	ss_title << playerShip->m_current_collision->m_display_name;
	m_title_text.setString(ss_title.str());

	//awareness gauge
	m_rectContainer.setSize(sf::Vector2f(SFSTRATAGEM_PANEL_WIDTH - 2*INTERACTION_MARGIN_SIDES, SFSTRATAGEM_PANEL_AWARENESS_HEIGHT));
	m_rectContainer.setFillColor(sf::Color::Black);
	m_rect.setSize(sf::Vector2f(1, SFSTRATAGEM_PANEL_AWARENESS_HEIGHT));
	m_rect.setFillColor(sf::Color::Green);

	//size and position of panel
	size_t itemsVectorSize = agent->m_items.size();
	if (itemsVectorSize > 1)
	{
		size.y += (itemsVectorSize -1) * (ITEM_SIZE + STRATAGEM_PANEL_SPACE_BETWEEN_LINES);
	}
	
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	sf::Vector2f position = sf::Vector2f(540, 540);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;

	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//stratagems (1 item = 1 box of codes)
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;

	size_t j = 0;
	for (size_t i = 0; i < itemsVectorSize; i++)
	{
		m_boxes[i].Init(agent->m_items[i], m_code);

		if (m_boxes[i].m_item)
		{
			if (j > 0)
			{
				text_height += ITEM_SIZE + STRATAGEM_PANEL_SPACE_BETWEEN_LINES;
			}

			j++;

			m_boxes[i].SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		}
	}

	//awareness gauge
	text_height += INTERACTION_INTERBLOCK;
	m_rectContainer.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_rect.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
}

SFStratagemPanel::~SFStratagemPanel()
{
	m_current_stratagem = NULL;
}

void SFStratagemPanel::Update(sf::Time deltaTime)
{
	Agent* agent = (Agent*)m_playerShip->m_current_collision;
	if (!agent)
	{
		return;
	}

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;

	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//stratagems (1 item = 1 box of codes)
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	size_t itemsVectorSize = agent->m_items.size();
	size_t j = 0;
	for (size_t i = 0; i < itemsVectorSize; i++)
	{
		if (m_boxes[i].m_item)
		{
			if (j > 0)
			{
				text_height += ITEM_SIZE + STRATAGEM_PANEL_SPACE_BETWEEN_LINES;
			}

			j++;

			m_boxes[i].SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		}
	}

	//awareness gauge
	text_height += ITEM_SIZE + STRATAGEM_PANEL_SPACE_BETWEEN_LINES + INTERACTION_INTERBLOCK;
	m_rectContainer.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_rect.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	m_rect.setSize(sf::Vector2f(m_playerShip->m_awareness_map[agent] * (SFSTRATAGEM_PANEL_WIDTH - 2 * INTERACTION_MARGIN_SIDES), SFSTRATAGEM_PANEL_AWARENESS_HEIGHT));
}

void SFStratagemPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);

		if (m_actions)
		{
			m_actions->Draw(screen);
		}

		Agent* agent = (Agent*)m_playerShip->m_current_collision;
		if (agent)
		{
			size_t itemsVectorSize = agent->m_items.size();
			for (size_t i = 0; i < itemsVectorSize; i++)
			{
				m_boxes[i].Draw(screen);
			}
		}

		screen.draw(m_rectContainer);
		screen.draw(m_rect);
	}
}

Item* SFStratagemPanel::CheckCodeInput(int input)
{
	Item* item_stolen = NULL;
	
	Agent* agent = (Agent*)m_playerShip->m_current_collision;
	if (!agent)
	{
		return item_stolen;
	}

	m_playerShip->m_awareness_map[agent] += WALKER_AWARENESS_PER_CODE;

	//is there a code in progress?
	bool stratagem_in_progress = false;
	for (size_t i = 0; i < MAX_ITEMS_PER_AGENT; i++)
	{
		if (m_boxes[i].m_current_code_index > 0)
		{
			stratagem_in_progress = true;
			break;
		}
	}

	if (!stratagem_in_progress)
	{
		for (size_t i = 0; i < MAX_ITEMS_PER_AGENT; i++)//test all codes
		{
			if (!m_boxes[i].m_item)
			{
				continue;
			}
			if (input == m_boxes[i].m_item->m_stratagem->m_code[m_boxes[i].m_current_code_index])//code successful | TODO : check if m_item and m_stratagem exist
			{
				m_boxes[i].m_codes[m_boxes[i].m_current_code_index]->setAnimationLine(Code_Validated);
				m_boxes[i].m_current_code_index++;

				if (m_boxes[i].m_current_code_index == MAX_CODES || m_boxes[i].m_item->m_stratagem->m_code[m_boxes[i].m_current_code_index] == 0)//code complete ?
				{
					m_boxes[i].m_current_code_index = 0;

					item_stolen = m_boxes[i].m_item;

					m_boxes[i].m_item = NULL;
					agent->m_items[i] = NULL;

					//reset all feedbacks
					for (size_t k = 0; k < MAX_ITEMS_PER_AGENT; k++)
					{
						for (int l = 0; l < MAX_CODES; l++)
						{
							if (m_boxes[k].m_codes[l] > 0)
							{
								m_boxes[k].m_codes[l]->setAnimationLine(Code_Normal);
							}
						}
					}
				}
			}
			else// code failed
			{
				for (size_t j = 0; j < MAX_CODES; j++)
				{
					if (m_boxes[i].m_codes[j] > 0)
					{
						m_boxes[i].m_codes[j]->setAnimationLine(Code_Invalid);
					}
				}
			}
		}

	}
	else //if (stratagem_in_progress == true)
	{
		for (size_t i = 0; i < MAX_ITEMS_PER_AGENT; i++)
		{
			if (!m_boxes[i].m_item)
			{
				continue;
			}
			if (m_boxes[i].m_current_code_index > 0)//test only codes in progress
			{
				if (input == m_boxes[i].m_item->m_stratagem->m_code[m_boxes[i].m_current_code_index])//code successful | TODO : check if m_item and m_stratagem exist
				{
					m_boxes[i].m_codes[m_boxes[i].m_current_code_index]->setAnimationLine(Code_Validated);
					m_boxes[i].m_current_code_index++;

					if (m_boxes[i].m_current_code_index == MAX_CODES || m_boxes[i].m_item->m_stratagem->m_code[m_boxes[i].m_current_code_index] == 0)//code complete ?
					{
						m_boxes[i].m_current_code_index = 0;

						item_stolen = m_boxes[i].m_item;

						m_boxes[i].m_item = NULL;
						agent->m_items[i] = NULL;

						//reset all feedbacks
						for (size_t k = 0; k < MAX_ITEMS_PER_AGENT; k++)
						{
							for (int l = 0; l < MAX_CODES; l++)
							{
								if (m_boxes[k].m_codes[l] > 0)
								{
									m_boxes[k].m_codes[l]->setAnimationLine(Code_Normal);
								}
							}
						}
					}
				}
				else// code failed
				{
					for (size_t j = 0; j < MAX_CODES; j++)
					{
						m_boxes[i].m_current_code_index = 0;

						if (m_boxes[i].m_codes[j] > 0)
						{
							m_boxes[i].m_codes[j]->setAnimationLine(Code_Invalid);
						}
					}
				}
			}
		}
	}

	//reset feedbacks ?
	stratagem_in_progress = false;
	for (size_t i = 0; i < MAX_ITEMS_PER_AGENT; i++)
	{
		if (m_boxes[i].m_current_code_index > 0)
		{
			stratagem_in_progress = true;
			break;
		}
	}
	if (!stratagem_in_progress)
	{
		for (size_t i = 0; i < MAX_ITEMS_PER_AGENT; i++)
		{
			for (size_t j = 0; j < MAX_CODES; j++)
			{
				if (m_boxes[i].m_codes[j] > 0)
				{
					m_boxes[i].m_codes[j]->setAnimationLine(Code_Normal);
				}
			}
		}
	}

	return item_stolen;
}