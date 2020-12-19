#include "SFUpgradesPanel.h"

extern Game* CurrentGame;

//UPGRADES PANEL
SFUpgradesPanel::SFUpgradesPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Upgrades)
{
	m_playerShip = playerShip;
	m_selected_index = 0;

	m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	m_actions->SetString("Quit", ActionButton_B);
	m_actions->SetString("Buy", ActionButton_A);

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts
	m_title_text.setString("Use your money to buy upgrades for your ship");

	//size and position
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

	//positioning panel content
	float text_height = 0;

	text_height += m_title_text.getGlobalBounds().height * 0.5 + INTERACTION_INTERBLOCK;
	m_title_text.setOrigin(sf::Vector2f(m_title_text.getGlobalBounds().width * 0.5, m_title_text.getGlobalBounds().height * 0.5));
	m_title_text.setPosition(getPosition().x, getPosition().y - getSize().y / 2 + text_height);

	//upgrades
	text_height += 2 * INTERACTION_INTERBLOCK;
	float container_pos_y = text_height;
	for (int i = 0; i < NB_UPGRADE_CHOICES; i++)
	{
		m_sold_out[i] = m_playerShip->m_targetShop->m_sold_out[i];

		//container
		m_upgrade_container[i].setSize(sf::Vector2f(UPGRADES_CONTAINER_WIDTH, UPGRADES_CONTAINER_HEIGHT));
		m_upgrade_container[i].setOrigin(sf::Vector2f(m_upgrade_container[i].getSize().x * 0.5, m_upgrade_container[i].getSize().y * 0.5));
		m_upgrade_container[i].setFillColor(COLOR_DARKGREY);
		m_upgrade_container[i].setOutlineThickness(2);
		m_upgrade_container[i].setOutlineColor(COLOR_DARKBLUE);
		m_upgrade_container[i].setPosition(sf::Vector2f(getPosition().x - getSize().x * 0.5 + INTERACTION_PANEL_MARGIN_SIDES + (UPGRADES_CONTAINER_WIDTH + m_upgrade_container[i].getOutlineThickness()) * (0.5 + i), getPosition().y - getSize().y * 0.5 + container_pos_y + UPGRADES_CONTAINER_HEIGHT * 0.5));
	
		//upgrade content
		float pos_x = m_upgrade_container[i].getPosition().x - m_upgrade_container[i].getSize().x * 0.5 + INTERACTION_PANEL_MARGIN_SIDES;
		text_height = m_upgrade_container[i].getPosition().y - m_upgrade_container[i].getSize().y * 0.5 + INTERACTION_PANEL_MARGIN_SIDES;
		
		//name
		m_upgrades[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_upgrades[i].setCharacterSize(20);
		m_upgrades[i].setColor(COLOR_YELLOW);
		m_upgrades[i].setPosition(sf::Vector2f(pos_x, text_height));
		m_upgrades[i].setString(SFReplaceTexts((*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[i]][UPGRADE_DISPLAY_NAME], 0));

		//picture
		text_height += m_upgrades[i].getGlobalBounds().height;
		text_height += UPGRADES_PICTURE_HEIGHT * 0.5;
		text_height += 2 * INTERACTION_INTERBLOCK;
		m_upgrade_picture_container[i].setSize(sf::Vector2f(UPGRADES_PICTURE_WIDTH + 2, UPGRADES_PICTURE_HEIGHT + 2));
		m_upgrade_picture_container[i].setOrigin(sf::Vector2f(m_upgrade_picture_container[i].getSize().x * 0.5, m_upgrade_picture_container[i].getSize().y * 0.5));
		m_upgrade_picture_container[i].setFillColor(COLOR_DARKGREY);
		m_upgrade_picture_container[i].setOutlineThickness(2);
		m_upgrade_picture_container[i].setOutlineColor(COLOR_DARKBLUE);
		m_upgrade_picture_container[i].setPosition(sf::Vector2f(pos_x + UPGRADES_PICTURE_WIDTH * 0.5, text_height));

		m_upgrade_picture[i] = new GameObject(m_upgrade_picture_container[i].getPosition(), sf::Vector2f(0, 0), (*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[i]][UPGRADE_IMAGE], sf::Vector2f(UPGRADES_PICTURE_WIDTH, UPGRADES_PICTURE_HEIGHT));

		//price
		text_height += UPGRADES_PICTURE_HEIGHT * 0.5;
		text_height += 1 * INTERACTION_INTERBLOCK;
		m_upgrade_price[i].setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_upgrade_price[i].setCharacterSize(14);
		m_upgrade_price[i].setColor(sf::Color::White);
		m_upgrade_price[i].setPosition(sf::Vector2f(pos_x, text_height));
		m_upgrade_price[i].setString("$ " + (*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[i]][UPGRADE_PRICE]);

		//description
		text_height += m_upgrade_price[i].getGlobalBounds().height;
		text_height += 2 * INTERACTION_INTERBLOCK;
		m_upgrade_description[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_upgrade_description[i].setCharacterSize(16);
		m_upgrade_description[i].setColor(sf::Color::White);
		m_upgrade_description[i].setPosition(sf::Vector2f(pos_x, text_height));
		m_upgrade_description[i].setString(SFReplaceTexts((*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[i]][UPGRADE_DESCRIPTION], 32));

		//unavailable feedback mask
		m_unavailable_mask[i].setSize(m_upgrade_container[i].getSize());
		m_unavailable_mask[i].setOrigin(sf::Vector2f(m_unavailable_mask[i].getSize().x * 0.5, m_unavailable_mask[i].getSize().y * 0.5));
		m_unavailable_mask[i].setFillColor(COLOR_BLACKMASK);
		m_unavailable_mask[i].setPosition(m_upgrade_container[i].getPosition());
		m_unavailable_mask[i].m_visible = false;

		//sold out text
		text_height += 4 * INTERACTION_INTERBLOCK;
		m_sold_out_text[i].setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_sold_out_text[i].setCharacterSize(18);
		m_sold_out_text[i].setColor(sf::Color::Red);
		m_sold_out_text[i].setPosition(sf::Vector2f(pos_x, text_height));
		m_sold_out_text[i].setString("SOLD OUT");
		m_sold_out_text[i].m_visible = m_sold_out[i];
	}
	
	//quit button
	text_height = container_pos_y + UPGRADES_CONTAINER_HEIGHT;
	text_height += 2 * INTERACTION_INTERBLOCK;
	m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//current money
	m_money_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
	m_money_text.setCharacterSize(16);
	m_money_text.setColor(sf::Color::White);
	m_money_text.setString("$ " + to_string(m_playerShip->m_money));
	m_money_text.setPosition(sf::Vector2f(getPosition().x + getSize().x / 2 - INTERACTION_PANEL_MARGIN_SIDES - 150, getPosition().y - getSize().y / 2 + text_height));
}

void SFUpgradesPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//select upgrade
	if (m_playerShip->m_inputs_states[Action_Left] == Input_Tap)
		m_selected_index = m_selected_index == 0 ? 0 : m_selected_index - 1;

	if (m_playerShip->m_inputs_states[Action_Right] == Input_Tap)
		m_selected_index = m_selected_index == NB_UPGRADE_CHOICES - 1 ? NB_UPGRADE_CHOICES - 1 : m_selected_index + 1;

	for (int i = 0; i < NB_UPGRADE_CHOICES; i++)
	{
		//sold out?
		m_sold_out[i] = m_playerShip->m_targetShop->m_sold_out[i];

		//selection feedback
		if (m_selected_index == i)
			m_upgrade_container[i].setFillColor(COLOR_LIGHTGREY);
		else
			m_upgrade_container[i].setFillColor(COLOR_DARKGREY);

		//sold out or insufficient money?
		m_unavailable_mask[i].m_visible = i != m_selected_index && (m_sold_out[i] == true || m_playerShip->m_money < stoi((*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[m_selected_index]][UPGRADE_PRICE]));
		m_sold_out_text[i].m_visible = m_sold_out[i];

		if (m_sold_out[i] == true)
			m_upgrade_price[i].setColor(sf::Color(128, 128, 128, 255));
		else if (m_playerShip->m_money < stoi((*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[i]][UPGRADE_PRICE]))
			m_upgrade_price[i].setColor(sf::Color::Red);
		else
			m_upgrade_price[i].setColor(sf::Color::White);
	}

	//money
	m_money_text.setString("$ " + to_string(m_playerShip->m_money));
}

void SFUpgradesPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		
		screen.draw(m_title_text);
	
		for (int i = 0; i < NB_UPGRADE_CHOICES; i++)
		{
			screen.draw(m_upgrade_container[i]);
			screen.draw(m_upgrades[i]);
			screen.draw(m_upgrade_description[i]);
			screen.draw(m_upgrade_price[i]);
			screen.draw(m_upgrade_picture_container[i]);
			m_upgrade_picture[i]->Draw(screen);
		
			if (m_unavailable_mask[i].m_visible == true)
				screen.draw(m_unavailable_mask[i]);

			if (m_sold_out_text[i].m_visible == true)
				screen.draw(m_sold_out_text[i]);
		}

		if (m_actions)
			m_actions->Draw(screen);

		screen.draw(m_money_text);
	}
}

bool SFUpgradesPanel::BuyUpgrade()
{
	if (m_playerShip->m_money < stoi((*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[m_selected_index]][UPGRADE_PRICE]))
		return false;

	if (m_sold_out[m_selected_index] == true)
		return false;

	m_playerShip->SetUpgrade(m_playerShip->m_targetShop->m_upgrades[m_selected_index]);
	m_playerShip->m_money -= stoi((*CurrentGame).m_upgradesConfig[m_playerShip->m_targetShop->m_upgrades[m_selected_index]][UPGRADE_PRICE]);
	m_playerShip->m_targetShop->m_sold_out[m_selected_index] = true;

	(*CurrentGame).PlaySFX(SFX_BuyOrSell);

	Ship::SavePlayerMoneyAndHealth(m_playerShip);
	Ship::SavePlayerUpgrades(m_playerShip);
	Shop::SaveShopUpgrades(m_playerShip->m_targetShop);

	return true;
}