#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).m_playerShip = playerShip;
	//(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);

	//Load saved file
	//if (!Ship::LoadShip(playerShip))
	//{
	//	//or create a new save file
	//	Ship::SaveShip(playerShip);
	//}

	//Loading scripts
	LoadCSVFile(SHIP_CSV_FILE);

	GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = background->m_size;
	//(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	//(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);

	//BIG BOOK
	InitTable();

	m_mage.InitCards();
	m_mage.ShuffleLibrary();
	m_mage.DrawCard(5);

	m_monsters.push_back(Monster());
}

void InGameState::InitTable()
{
	(*CurrentGame).m_selected_slot = NULL;
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;

	//Hand
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		m_mage.m_hand_slots[i].m_status = CardSlot_Free;
		m_mage.m_hand_slots[i].m_stack = Stack_Hand;
		m_mage.m_hand_slots[i].m_index = i;

		m_mage.m_hand_slots[i].m_shape_container.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 700));
		m_mage.m_hand_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_mage.m_hand_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_mage.m_hand_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_mage.m_hand_slots[i].m_shape_container.setOutlineThickness(2);
		m_mage.m_hand_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_mage.m_hand_slots[i].m_shape.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 700));
		m_mage.m_hand_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_mage.m_hand_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_mage.m_hand_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_mage.m_hand_slots[i].m_shape.setOutlineThickness(0);
		m_mage.m_hand_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_mage.m_hand_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_mage.m_hand_slots[i].m_text.setCharacterSize(18);
		m_mage.m_hand_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_mage.m_hand_slots[i].m_text.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 700));
		m_mage.m_hand_slots[i].m_text.setString("");
	}

	//Altar
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_mage.m_altar_slots[i].m_status = CardSlot_Free;
		m_mage.m_altar_slots[i].m_stack = Stack_Altar;
		m_mage.m_altar_slots[i].m_index = i;

		m_mage.m_altar_slots[i].m_shape_container.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_mage.m_altar_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_mage.m_altar_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_mage.m_altar_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_mage.m_altar_slots[i].m_shape_container.setOutlineThickness(2);
		m_mage.m_altar_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_mage.m_altar_slots[i].m_shape.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_mage.m_altar_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_mage.m_altar_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_mage.m_altar_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_mage.m_altar_slots[i].m_shape.setOutlineThickness(0);
		m_mage.m_altar_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_mage.m_altar_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_mage.m_altar_slots[i].m_text.setCharacterSize(18);
		m_mage.m_altar_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_mage.m_altar_slots[i].m_text.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_mage.m_altar_slots[i].m_text.setString("");
	}

	//Library
	m_mage.m_library_slot.m_stack = Stack_Altar;

	m_mage.m_library_slot.m_shape_container.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2, 800));
	m_mage.m_library_slot.m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_mage.m_library_slot.m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_mage.m_library_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_mage.m_library_slot.m_shape_container.setOutlineThickness(2);
	m_mage.m_library_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_mage.m_library_slot.m_shape.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2, 800));
	m_mage.m_library_slot.m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_mage.m_library_slot.m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_mage.m_library_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_mage.m_library_slot.m_shape.setOutlineThickness(0);
	m_mage.m_library_slot.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

	m_mage.m_library_slot.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_mage.m_library_slot.m_text.setCharacterSize(18);
	m_mage.m_library_slot.m_text.setColor(sf::Color(255, 255, 255, 255));
	m_mage.m_library_slot.m_text.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2, 800));
	m_mage.m_library_slot.m_text.setString("");

	//Graveyard
	m_mage.m_graveyard_slot.m_stack = Stack_Graveyard;

	m_mage.m_graveyard_slot.m_shape_container.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 6, 800));
	m_mage.m_graveyard_slot.m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_mage.m_graveyard_slot.m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_mage.m_graveyard_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_mage.m_graveyard_slot.m_shape_container.setOutlineThickness(2);
	m_mage.m_graveyard_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_mage.m_graveyard_slot.m_shape.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 6, 800));
	m_mage.m_graveyard_slot.m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_mage.m_graveyard_slot.m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_mage.m_graveyard_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_mage.m_graveyard_slot.m_shape.setOutlineThickness(0);
	m_mage.m_graveyard_slot.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

	m_mage.m_graveyard_slot.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_mage.m_graveyard_slot.m_text.setCharacterSize(18);
	m_mage.m_graveyard_slot.m_text.setColor(sf::Color(0, 0, 0, 255));
	m_mage.m_graveyard_slot.m_text.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 6, 800));
	m_mage.m_graveyard_slot.m_text.setString("");
}

Spell::Spell(int cost, int nb_costs)
{
	if (nb_costs > cost)
	{
		nb_costs = cost;
		printf("Creating a spell with more nb_costs than cost possible. (%d, %d)\n", nb_costs, cost);
	}

	int half = nb_costs % cost == 0 ? 0 : nb_costs / 2;
	int avg_cost = cost / nb_costs;;
	
	for (int i = 0; i < nb_costs; i++)
	{
		int type = RandomizeIntBetweenValues((int)Mana_Fire, (int)Mana_Earth);

		if (i < half)
		{
			int cost = avg_cost > 2 ? 2 : avg_cost;
			m_costs.push_back(Card((ManaType)type, (ManaValue)(cost)));
		}
		else
		{
			int cost = avg_cost - 1 > 2 ? 3 : avg_cost;
			m_costs.push_back(Card((ManaType)type, (ManaValue)(cost - 1)));
		}
	}

	m_display_name = "Spell";
	m_description = "Burns one card slot at random in player hand.";
}

Monster::Monster()
{
	m_spells.push_back(Spell(2, 2));
	m_spells.push_back(Spell(3, 2));
	m_spells.push_back(Spell(3, 2));
	m_spells.push_back(Spell(4, 3));
}

void InGameState::Update(sf::Time deltaTime)
{
	//Get mouse inputs
	sf::Vector2i mousepos2i = sf::Mouse::getPosition(*(*CurrentGame).getMainWindow());
	(*CurrentGame).m_mouse_pos = (*CurrentGame).getMainWindow()->mapPixelToCoords(mousepos2i, (*CurrentGame).m_view);

	if ((*CurrentGame).m_mouse_click_timer > 0)
	{
		(*CurrentGame).m_mouse_click_timer -= deltaTime.asSeconds();
	}

	(*CurrentGame).m_mouse_click = Mouse_None;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (*CurrentGame).m_window_has_focus && (*CurrentGame).m_mouse_click_timer <= 0)
	{
		(*CurrentGame).m_mouse_click = Mouse_LeftClick;
		(*CurrentGame).m_mouse_click_timer = 0.2f;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (*CurrentGame).m_window_has_focus && (*CurrentGame).m_mouse_click_timer <= 0)
	{
		(*CurrentGame).m_mouse_click = Mouse_RightClick;
		(*CurrentGame).m_mouse_click_timer = 0.2f;
	}

	//Reset flags
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;

	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		(*CurrentGame).m_selected_slot = NULL;
	}

	//Cards slots selection
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		m_mage.m_hand_slots[i].Update((*CurrentGame).m_mouse_click);
	}
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_mage.m_altar_slots[i].Update((*CurrentGame).m_mouse_click);
	}
	m_mage.m_library_slot.Update((*CurrentGame).m_mouse_click);
	m_mage.m_graveyard_slot.Update((*CurrentGame).m_mouse_click);

	//Actions
	bool played = false;
	if ((*CurrentGame).m_play_card_slot != NULL)
	{
		played = m_mage.PlayCard((*CurrentGame).m_play_card_slot->m_index, (*CurrentGame).m_target_slot->m_index);
	}

	if (m_mage.GetFreeAltarCardSlot() == -1)
	{
		m_mage.Attack();
	}

	if (played)
	{
		m_mage.DrawCard();
	}

	//Cards UI update
	m_mage.m_library_slot.m_text.setString(to_string((int)m_mage.m_libary.size()));

	if (m_mage.m_graveyard.empty())
	{
		m_mage.m_graveyard_slot.m_shape.setFillColor(sf::Color(0, 0, 0, 255));
		m_mage.m_graveyard_slot.m_text.setString("");
	}
	else
	{
		m_mage.m_graveyard_slot.GetCard(m_mage.m_graveyard.back());
	}

	//useless?
	(*CurrentGame).updateScene(deltaTime);

	//move camera
	//UpdateCamera(deltaTime);
	//
	////Create and destroy HUD panels
	////case 1: destroying a panel
	//if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel == SFPanel_None && (*CurrentGame).m_playerShip->m_SFTargetPanel)
	//{
	//	DestroySFPanel((*CurrentGame).m_playerShip);
	//}
	//else if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel != SFPanel_None)
	//{
	//	//case 2: creating a panel
	//	if (!(*CurrentGame).m_playerShip->m_SFTargetPanel)
	//	{
	//		CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
	//	}
	//	//case 3: changing panel
	//	else if ((*CurrentGame).m_playerShip->m_SFTargetPanel->m_panel_type != (*CurrentGame).m_playerShip->m_is_asking_SFPanel)
	//	{
	//		DestroySFPanel((*CurrentGame).m_playerShip);
	//		CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
	//	}
	//}

	this->mainWindow->clear();
}

void InGameState::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//Draw
	(*CurrentGame).drawScene();//background

	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		(*CurrentGame).m_mainScreen.draw(m_mage.m_hand_slots[i].m_shape_container);

		if (m_mage.m_hand_slots[i].m_status != CardSlot_Free)
			(*CurrentGame).m_mainScreen.draw(m_mage.m_hand_slots[i].m_shape);

		if (m_mage.m_hand_slots[i].m_status == CardSlot_Occupied)
		(*CurrentGame).m_mainScreen.draw(m_mage.m_hand_slots[i].m_text);
	}

	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		(*CurrentGame).m_mainScreen.draw(m_mage.m_altar_slots[i].m_shape_container);

		if (m_mage.m_altar_slots[i].m_status != CardSlot_Free)
		{
			(*CurrentGame).m_mainScreen.draw(m_mage.m_altar_slots[i].m_shape);
			(*CurrentGame).m_mainScreen.draw(m_mage.m_altar_slots[i].m_text);
		}
	}

	(*CurrentGame).m_mainScreen.draw(m_mage.m_library_slot.m_shape_container);
	(*CurrentGame).m_mainScreen.draw(m_mage.m_library_slot.m_shape);
	(*CurrentGame).m_mainScreen.draw(m_mage.m_library_slot.m_text);

	(*CurrentGame).m_mainScreen.draw(m_mage.m_graveyard_slot.m_shape_container);
	(*CurrentGame).m_mainScreen.draw(m_mage.m_graveyard_slot.m_shape);
	(*CurrentGame).m_mainScreen.draw(m_mage.m_graveyard_slot.m_text);

	//Display
	(*CurrentGame).m_mainScreen.display();
	sf::Sprite temp((*CurrentGame).m_mainScreen.getTexture());
	temp.scale((*CurrentGame).m_scale_factor.x, (*CurrentGame).m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	(*CurrentGame).getMainWindow()->draw(temp);

	
}

void InGameState::Release()
{
	//TODO
}

void InGameState::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

void InGameState::DestroySFPanel(Ship* playerShip)
{
	if (playerShip->m_SFTargetPanel)
	{
		(*CurrentGame).removeFromFeedbacks(playerShip->m_SFTargetPanel);
		delete playerShip->m_SFTargetPanel;
		playerShip->m_SFTargetPanel = NULL;
	}
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Specific:
		{
			playerShip->m_SFTargetPanel = new SFPanelSpecific(sf::Vector2f(SFPANEL_SPECIFIC_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_Specific, playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::LoadCSVFile(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	vector<vector<string> > allConfigs = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allConfigVectorSize = allConfigs.size();
	for (size_t i = 0; i < allConfigVectorSize; i++)
	{
		(*CurrentGame).m_gameObjectsConfig.insert(std::map<string, vector<string> >::value_type(allConfigs[i][0], allConfigs[i]));
	}

	allConfigs.clear();
}


//BIG BOOK
void Mage::InitCards()
{
	m_cards.push_back(Card(Mana_Fire, Mana_1));
	m_cards.push_back(Card(Mana_Fire, Mana_1));
	//m_cards.push_back(Card(Mana_Fire, Mana_1));	//weakness
	m_cards.push_back(Card(Mana_Fire, Mana_2));

	m_cards.push_back(Card(Mana_Water, Mana_1));
	m_cards.push_back(Card(Mana_Water, Mana_1));
	m_cards.push_back(Card(Mana_Water, Mana_1));
	m_cards.push_back(Card(Mana_Water, Mana_2));

	m_cards.push_back(Card(Mana_Earth, Mana_1));
	m_cards.push_back(Card(Mana_Earth, Mana_1));
	//m_cards.push_back(Card(Mana_Earth, Mana_1));	//weakness
	m_cards.push_back(Card(Mana_Earth, Mana_2));

	m_cards.push_back(Card(Mana_Air, Mana_1));
	m_cards.push_back(Card(Mana_Air, Mana_1));
	m_cards.push_back(Card(Mana_Air, Mana_1));
	m_cards.push_back(Card(Mana_Air, Mana_2));

	for (vector<Card>::iterator it = m_cards.begin(); it < m_cards.end(); it++)
	{
		m_libary.push_back(*it);
	}
}

void Mage::ShuffleLibrary()
{
	int library_size = m_libary.size();

	vector<Card> old_library;

	for (int i = 0; i < library_size; i++)
	{
		old_library.push_back(m_libary[i]);
	}

	m_libary.clear();

	for (int i = 0; i < library_size; i++)
	{
		int n = RandomizeIntBetweenValues(0, library_size - 1);
		m_libary.push_back(old_library[n]);
	}

	old_library.clear();
}

void Mage::DrawCard(int nb_cards)
{
	for (int k = 0; k < nb_cards; k++)
	{
		//libray empty = put graveyard into library then shuffle
		if (m_libary.empty())
		{
			int graveyard_size = m_graveyard.size();

			for (int i = 0; i < graveyard_size; i++)
			{
				m_libary.push_back(m_graveyard[i]);
			}

			m_graveyard.clear();

			ShuffleLibrary();
		}

		//draw card if not > max allowed
		int free_slot = GetFreeHandCardSlot();
		if (free_slot >= 0)
		{
			m_hand_slots[free_slot].GetCard(m_libary.back());
			m_hand_slots[free_slot].m_status = CardSlot_Occupied;
			m_libary.pop_back();
		}
		else
		{
			printf("Cannot draw card: no free slot in hand.\n");
		}
	}
}

int Mage::GetFreeHandCardSlot()
{
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		if (m_hand_slots[i].m_status == CardSlot_Free)
		{
			return i;
		}
	}

	return -1;
}

int Mage::GetFreeAltarCardSlot()
{
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		if (m_altar_slots[i].m_status == CardSlot_Free)
		{
			return i;
		}
	}

	return -1;
}

bool Mage::PlayCard(int hand_slot, int altar_slot)
{
	if (m_hand_slots[hand_slot].m_status == CardSlot_Occupied)
	{
		if (m_altar_slots[altar_slot].m_status == CardSlot_Free)
		{
			m_altar_slots[altar_slot].GetCard(m_hand_slots[hand_slot].m_card);
			m_altar_slots[altar_slot].m_status = CardSlot_Occupied;

			m_hand_slots[hand_slot].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
			m_hand_slots[hand_slot].m_text.setString("");
			m_hand_slots[hand_slot].m_status = CardSlot_Free;

			m_hand_slots[hand_slot].m_selected = false;

			return true;
		}
		else
		{
			printf("Cannot play card: altar slot occupied.\n");
			return false;
		}
	}
	else
	{
		printf("Cannot play card: no card in hand at this slot.\n");
		return false;
	}
}

void CardSlot::GetCard(Card& card)
{
	m_card.m_value = card.m_value; 
	m_card.m_type = card.m_type;

	//display
	m_shape.setFillColor(GetManaColor(card.m_type));
	m_text.setString(std::to_string((int)(card.m_value) + 1));
}

sf::Color CardSlot::GetManaColor(ManaType type)
{
	if (type == Mana_Fire)
	{
		return sf::Color(255, 0, 0, 255);
	}
	else if (type == Mana_Water)
	{
		return sf::Color(0, 0, 255, 255);
	}
	else if (type == Mana_Air)
	{
		return sf::Color(230, 230, 255, 255);
	}
	else// if (type == Mana_Earth)
	{
		return sf::Color(0, 255, 0, 255);
	}
}

sf::Color CardSlot::GetStatusColor(CardSlotStatus status)
{
	if (status == CardSlot_Free)
	{
		return sf::Color(0, 0, 0, 0);
	}
	else// if (status == CardSlot_Burnt)
	{
		return sf::Color(255, 255, 0, 255);
	}
}

void CardSlot::Update(MouseAction mouse_click)
{
	if ((*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - CARD_WIDTH / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + CARD_WIDTH / 2
		&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - CARD_HEIGHT / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + CARD_HEIGHT / 2)
	{
		m_hovered = true;
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
			(*CurrentGame).m_selected_slot = this;
		}
		else
		{
			m_selected = false;
		}
	}

	m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	if (m_hovered)
	{
		m_shape_container.setOutlineColor(sf::Color(255, 0, 0, 255));
	}
	if (m_selected)
	{
		m_shape_container.setOutlineColor(sf::Color(0, 255, 0, 255));
	}

	//Actions
	//PLAY CARD
	if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Hand && (*CurrentGame).m_selected_slot->m_status == CardSlot_Occupied && m_stack == Stack_Altar)
	{
		(*CurrentGame).m_play_card_slot = (*CurrentGame).m_selected_slot;
		(*CurrentGame).m_target_slot = this;
	}
}

void Mage::Attack()
{
	//todo: attack

	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_graveyard.push_back(m_altar_slots[i].m_card);

		m_altar_slots[i].m_status = CardSlot_Free;
		m_altar_slots[i].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
		m_altar_slots[i].m_text.setString("");
	}
}