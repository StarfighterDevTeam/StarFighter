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

	for (int i = 0; i < NB_PLAYERS_MAX; i++)
	{
		m_mages.push_back(Mage(i));
		m_mages.back().InitSlots(i);
		m_mages.back().InitCards();
		m_mages.back().ShuffleLibrary();
		m_mages.back().DrawCard(5);
	}
	

	for (int i = 0; i < NB_MONSTERS; i++)
	{
		m_monsters.push_back(Monster());
	}
	
	SummonMonster();
}

void InGameState::SummonMonster()
{
	//clean previous curses
	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		m_monster_curses_names[i].setColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_descriptions[i].setColor(sf::Color(255, 255, 255, 255));

		m_monster_curses_slots[i].m_status = CardSlot_Occupied;

		for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
		{
			m_monster_curses_costs[i][j].m_status = CardSlot_Free;
		}
	}

	//get current curses
	Monster& monster = m_monsters.back();

	for (int i = 0; i < monster.m_curses.size(); i++)
	{
		m_monster_curses_names[i].setString(monster.m_curses[i].m_display_name);
		m_monster_curses_descriptions[i].setString(monster.m_curses[i].m_description);

		for (int j = 0; j < monster.m_curses[i].m_costs.size(); j++)
		{
			m_monster_curses_costs[i][j].GetCard(monster.m_curses[i].m_costs[j]);
			m_monster_curses_costs[i][j].m_status = CardSlot_Occupied;
		}
	}
}

void InGameState::InitTable()
{
	(*CurrentGame).m_hovered_slot = NULL;
	(*CurrentGame).m_selected_slot = NULL;
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;
	m_current_player = 0;
	
	//Altar
	m_altar_slot.m_stack = Stack_Altar;

	m_altar_slot.m_shape_container.setPosition(sf::Vector2f(500 + ((CARD_WIDTH +20) * NB_CARDS_ALTAR) * 0.5, 500));
	m_altar_slot.m_shape_container.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10) * 0.5, CARD_HEIGHT / 2 + 5));
	m_altar_slot.m_shape_container.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10, CARD_HEIGHT + 10));
	m_altar_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_altar_slot.m_shape_container.setOutlineThickness(2);
	m_altar_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_altar_slot.m_shape.setPosition(sf::Vector2f(500 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 500));
	m_altar_slot.m_shape.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10) * 0.5, CARD_HEIGHT / 2 + 5));
	m_altar_slot.m_shape.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10, CARD_HEIGHT + 10));
	m_altar_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_altar_slot.m_shape.setOutlineThickness(0);
	m_altar_slot.m_shape.setFillColor(sf::Color(255, 255, 255, 0));

	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_altar_slots[i].m_status = CardSlot_Free;
		m_altar_slots[i].m_stack = Stack_AltarSlot;
		m_altar_slots[i].m_index = i;

		m_altar_slots[i].m_shape_container.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_altar_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_altar_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_altar_slots[i].m_shape_container.setOutlineThickness(2);
		m_altar_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_altar_slots[i].m_shape.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_altar_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_altar_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_altar_slots[i].m_shape.setOutlineThickness(0);
		m_altar_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_altar_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_altar_slots[i].m_text.setCharacterSize(18);
		m_altar_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_altar_slots[i].m_text.setPosition(sf::Vector2f(500 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_text.setString("");
	}

	//Monster curses
	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		m_monster_curses_names[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_monster_curses_names[i].setCharacterSize(18);
		m_monster_curses_names[i].setColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_names[i].setPosition(sf::Vector2f(1100, 100 - CARD_HEIGHT / 2 - 50 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_names[i].setString("");

		m_monster_curses_descriptions[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_monster_curses_descriptions[i].setCharacterSize(18);
		m_monster_curses_descriptions[i].setColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_descriptions[i].setPosition(sf::Vector2f(1100, 100 - CARD_HEIGHT / 2 - 30 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_descriptions[i].setString("");
		
		m_monster_curses_slots[i].m_status = CardSlot_Free;
		m_monster_curses_slots[i].m_stack = Stack_MonsterCurses;
		m_monster_curses_slots[i].m_index = i;

		m_monster_curses_slots[i].m_shape.setPosition(sf::Vector2f(1100 + ((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX) * 0.5, 100 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_slots[i].m_shape.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10) * 0.5, CARD_HEIGHT / 2 + 5));
		m_monster_curses_slots[i].m_shape.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10, CARD_HEIGHT + 10));
		m_monster_curses_slots[i].m_shape.setOutlineColor(sf::Color(255, 255, 255, 0));
		m_monster_curses_slots[i].m_shape.setOutlineThickness(0);
		m_monster_curses_slots[i].m_shape.setFillColor(sf::Color(0, 0, 0, 255));

		m_monster_curses_slots[i].m_shape_container.setPosition(sf::Vector2f(1100 + ((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX) * 0.5, 100 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_slots[i].m_shape_container.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10) * 0.5, CARD_HEIGHT / 2 + 5));
		m_monster_curses_slots[i].m_shape_container.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10, CARD_HEIGHT + 10));
		m_monster_curses_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_slots[i].m_shape_container.setOutlineThickness(2);
		m_monster_curses_slots[i].m_shape_container.setFillColor(sf::Color(255, 255, 255, 0));

		for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
		{
			m_monster_curses_costs[i][j].m_status = CardSlot_Free;
			m_monster_curses_costs[i][j].m_stack = Stack_None;
			m_monster_curses_costs[i][j].m_index = i;

			m_monster_curses_costs[i][j].m_shape.setPosition(sf::Vector2f(1100 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_monster_curses_costs[i][j].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
			m_monster_curses_costs[i][j].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
			m_monster_curses_costs[i][j].m_shape.setOutlineColor(sf::Color(0, 0, 0, 255));
			m_monster_curses_costs[i][j].m_shape.setOutlineThickness(2);
			m_monster_curses_costs[i][j].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

			m_monster_curses_costs[i][j].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_monster_curses_costs[i][j].m_text.setCharacterSize(18);
			m_monster_curses_costs[i][j].m_text.setColor(sf::Color(0, 0, 0, 255));
			m_monster_curses_costs[i][j].m_text.setPosition(sf::Vector2f(1100 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_monster_curses_costs[i][j].m_text.setString("");
		}
	}
}

Curse::Curse(int cost, int nb_costs, int index)
{
	m_index = index;
	m_status = CardSlot_Occupied;

	if (cost == 0 || nb_costs == 0)
	{
		m_display_name = "No curse";
		m_description = "";
		return;
	}

	if (nb_costs > SPELL_NB_COSTS_MAX)
	{
		nb_costs = SPELL_NB_COSTS_MAX;
	}

	if (nb_costs > cost)
	{
		nb_costs = cost;
		printf("Creating a curse with more nb_costs than cost possible. (%d, %d)\n", nb_costs, cost);
	}

	int half = cost % nb_costs;
	int avg_cost = cost / nb_costs;
	
	for (int i = 0; i < nb_costs; i++)
	{
		int type = RandomizeIntBetweenValues((int)Mana_Fire, (int)Mana_Earth);

		if (i < half)
		{
			int cost = avg_cost > 2 ? 2 : avg_cost;
			m_costs.push_back(Card((ManaType)type, (ManaValue)(cost + 1), -1));
		}
		else
		{
			int cost = avg_cost - 1 > 2 ? 3 : avg_cost;
			m_costs.push_back(Card((ManaType)type, (ManaValue)(cost), -1));
		}
	}

	m_display_name = "Curse";
	m_description = "Burns one card slot at random in player hand.";
}

bool Curse::Effect()
{
	//do something
	printf("Monster attack (%d).\n", m_index);
	return true;
}

Monster::Monster()
{
	//first curses to trigger to last in the vector
	m_curses.push_back(Curse(5, 3, m_curses.size() + 1));
	m_curses.push_back(Curse(4, 3, m_curses.size() + 1));
	m_curses.push_back(Curse(3, 2, m_curses.size() + 1));
	m_curses.push_back(Curse(3, 2, m_curses.size() + 1));
	m_curses.push_back(Curse(2, 2, m_curses.size() + 1));
	m_curses.push_back(Curse(0, 0, m_curses.size() + 1));
}

void Monster::Attack()
{
	if (m_curses.back().m_status == CardSlot_Occupied)
	{
		m_curses.back().Effect();
	}
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
	(*CurrentGame).m_hovered_slot = NULL;
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;

	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		(*CurrentGame).m_selected_slot = NULL;
	}

	//Cards slots selection
	int p = m_current_player;
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		m_mages[p].m_hand_slots[i].Update((*CurrentGame).m_mouse_click);
	}

	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_altar_slots[i].Update((*CurrentGame).m_mouse_click);
	}
	m_altar_slot.Update((*CurrentGame).m_mouse_click);

	for (int p = 0; p < NB_PLAYERS_MAX; p++)
	{
		m_mages[p].m_library_slot.Update((*CurrentGame).m_mouse_click);
		m_mages[p].m_graveyard_slot.Update((*CurrentGame).m_mouse_click);
	}

	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		m_monster_curses_slots[i].Update((*CurrentGame).m_mouse_click);
	}

	//Actions
	Actions played = Action_None;
	if ((*CurrentGame).m_play_card_slot != NULL)
	{
		played = PlayCard(p, (*CurrentGame).m_play_card_slot->m_index, (*CurrentGame).m_target_slot->m_index);
	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_MonsterCurses)
	{
		played = AltarAttack(p, (*CurrentGame).m_target_slot->m_index);
	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_Library)
	{
		played = m_mages[p].DrawCard();
	}

	//Cards UI update
	for (int i = 0; i < NB_PLAYERS_MAX; i++)
	{
		m_mages[i].m_library_slot.m_text.setString(to_string((int)m_mages[i].m_libary.size()));

		if (m_mages[i].m_graveyard.empty())
		{
			m_mages[i].m_graveyard_slot.m_shape.setFillColor(sf::Color(0, 0, 0, 255));
			m_mages[i].m_graveyard_slot.m_text.setString("");
		}
		else
		{
			m_mages[i].m_graveyard_slot.GetCard(m_mages[i].m_graveyard.back());
		}
	}
	
	if (!m_monsters.empty() && !m_monsters.back().m_curses.empty())
	{
		m_monster_curses_names[m_monsters.back().m_curses.size() - 1].setColor(sf::Color(255, 0, 0, 255));
		m_monster_curses_descriptions[m_monsters.back().m_curses.size() - 1].setColor(sf::Color(255, 0, 0, 255));
	}

	//End of turn
	if (played == Action_DrawCard)
	{
		p = (m_current_player + 1) % NB_PLAYERS_MAX;
		m_current_player = p;
		//m_mages[p].DrawCard();

		if (!m_monsters.empty())
		{
			Monster& monster = m_monsters.back();
			if (!monster.m_curses.empty())
			{
				//attack with current curse
				monster.Attack();

				m_monster_curses_slots[monster.m_curses.size() - 1].m_status = CardSlot_Free;

				//clean current curse from display
				//for (int i = 0; i < m_monsters.back().m_curses.back().m_costs.size(); i++)
				//{
				//	m_monster_curses_costs[m_monsters.back().m_curses.size() - 1][i].m_status = CardSlot_Free;
				//}

				//go to next curse
				monster.m_curses.pop_back();
			}

			//go to next monster if all curses have been used
			if (monster.m_curses.empty())
			{
				m_monsters.pop_back();
				if (!m_monsters.empty())
				{
					SummonMonster();
				}
			}
		}
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

	for (int p = 0; p < NB_PLAYERS_MAX; p++)
	{
		for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
		{
			(*CurrentGame).m_mainScreen.draw(m_mages[p].m_hand_slots[i].m_shape_container);

			if (m_mages[p].m_hand_slots[i].m_status != CardSlot_Free)
				(*CurrentGame).m_mainScreen.draw(m_mages[p].m_hand_slots[i].m_shape);

			if (m_mages[p].m_hand_slots[i].m_status == CardSlot_Occupied)
				(*CurrentGame).m_mainScreen.draw(m_mages[p].m_hand_slots[i].m_text);
		}

		//Library
		(*CurrentGame).m_mainScreen.draw(m_mages[p].m_library_slot.m_shape_container);
		(*CurrentGame).m_mainScreen.draw(m_mages[p].m_library_slot.m_shape);
		(*CurrentGame).m_mainScreen.draw(m_mages[p].m_library_slot.m_text);

		//Graveyard
		(*CurrentGame).m_mainScreen.draw(m_mages[p].m_graveyard_slot.m_shape_container);
		(*CurrentGame).m_mainScreen.draw(m_mages[p].m_graveyard_slot.m_shape);
		(*CurrentGame).m_mainScreen.draw(m_mages[p].m_graveyard_slot.m_text);
	}
	
	//Altar
	(*CurrentGame).m_mainScreen.draw(m_altar_slot.m_shape);
	(*CurrentGame).m_mainScreen.draw(m_altar_slot.m_shape_container);

	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		(*CurrentGame).m_mainScreen.draw(m_altar_slots[i].m_shape_container);

		if (m_altar_slots[i].m_status != CardSlot_Free)
		{
			(*CurrentGame).m_mainScreen.draw(m_altar_slots[i].m_shape);
			(*CurrentGame).m_mainScreen.draw(m_altar_slots[i].m_text);
		}
	}

	//Monster
	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		if (m_monster_curses_slots[i].m_status != CardSlot_Free)
		{
			(*CurrentGame).m_mainScreen.draw(m_monster_curses_slots[i].m_shape);
			(*CurrentGame).m_mainScreen.draw(m_monster_curses_slots[i].m_shape_container);

			(*CurrentGame).m_mainScreen.draw(m_monster_curses_names[i]);
			(*CurrentGame).m_mainScreen.draw(m_monster_curses_descriptions[i]);

			for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
			{
				if (m_monster_curses_costs[i][j].m_status != CardSlot_Free)
				{
					(*CurrentGame).m_mainScreen.draw(m_monster_curses_costs[i][j].m_shape);
					(*CurrentGame).m_mainScreen.draw(m_monster_curses_costs[i][j].m_text);
				}
			}
		}
	}

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
int InGameState::GetFreeAltarCardSlot()
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

Actions InGameState::PlayCard(int player_index, int hand_slot, int altar_slot)
{
	if (m_mages[player_index].m_hand_slots[hand_slot].m_status == CardSlot_Occupied)
	{
		if (m_altar_slots[altar_slot].m_status == CardSlot_Free)
		{
			m_altar_slots[altar_slot].GetCard(m_mages[player_index].m_hand_slots[hand_slot].m_card);
			m_altar_slots[altar_slot].m_status = CardSlot_Occupied;

			m_mages[player_index].m_hand_slots[hand_slot].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
			m_mages[player_index].m_hand_slots[hand_slot].m_text.setString("");
			m_mages[player_index].m_hand_slots[hand_slot].m_status = CardSlot_Free;

			m_mages[player_index].m_hand_slots[hand_slot].m_selected = false;
			m_mages[player_index].m_hand_slots[hand_slot].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));

			return Action_HandToAltar;
		}
		else
		{
			printf("Cannot play card: altar slot occupied.\n");
			return Action_None;
		}
	}
	else
	{
		printf("Cannot play card: no card in hand at this slot.\n");
		return Action_None;
	}
}

Actions InGameState::AltarAttack(int player_index, int curse_slot)
{
	//Get altar mana
	bool altar_empty = true;
	int mana[NB_MANATYPES] = { 0, 0, 0, 0 };
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		if (m_altar_slots[i].m_status == CardSlot_Occupied)
		{
			altar_empty = false;
			mana[m_altar_slots[i].m_card.m_type] += m_altar_slots[i].m_card.m_value;
		}
	}
	if (altar_empty)
	{
		printf("Altar is empty\n");
		return Action_None;
	}

	//Get curse cost
	int cost[NB_MANATYPES] = { 0, 0, 0, 0 };
	for (int i = 0; i < m_monsters.back().m_curses[curse_slot].m_costs.size(); i++)
	{
		Card& curse_cost = m_monsters.back().m_curses[curse_slot].m_costs[i];
		cost[curse_cost.m_type] += curse_cost.m_value;
	}

	//Compare mana and cost
	for (int i = 0; i < NB_MANATYPES; i++)
	{
		if (mana[i] < cost[i])
		{
			printf("Insufficient mana in Altar to dispell this curse.\n");
			return Action_None;
		}
	}

	//Select optimal mana consumption to match cost automatically
	int mana_optim[NB_MANATYPES] = { 0, 0, 0, 0 };
	for (int i = 0; i < NB_MANATYPES; i++)
	{
		if (cost[i] > 0)
		{
			for (int j = 1; j <= Mana_3; j++)
			{
				for (int k = 0; k < NB_CARDS_ALTAR; k++)
				{
					if (m_altar_slots[k].m_status == CardSlot_Occupied && m_altar_slots[k].m_card.m_type == i && m_altar_slots[k].m_card.m_value == j)//mana 1, then 2, then 3...
					{
						mana_optim[i] += j;

						//discard card
						m_altar_slots[k].m_status = CardSlot_Free;
						m_mages[m_altar_slots[k].m_card.m_owner].m_graveyard.push_back(m_altar_slots[k].m_card);
						m_altar_slots[k].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
						m_altar_slots[k].m_text.setString("");

						//go to next mana_type?
						if (mana_optim[i] >= cost[i])
						{
							//exit loop
							k = NB_CARDS_ALTAR;
							j = Mana_3 + 1;
						}
					}
				}
			}
		}
	}

	//Dispell
	m_monster_curses_slots[curse_slot].m_status = CardSlot_Burnt;
	m_monster_curses_names[curse_slot].setColor(sf::Color(128, 128, 128, 255));
	m_monster_curses_descriptions[curse_slot].setColor(sf::Color(128, 128, 128, 255));

	//Clear altar
	//for (int i = 0; i < NB_CARDS_ALTAR; i++)
	//{
	//	m_mages[player_index].m_graveyard.push_back(m_altar_slots[i].m_card);
	//	m_altar_slots[i].m_status = CardSlot_Free;
	//	m_altar_slots[i].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
	//	m_altar_slots[i].m_text.setString("");
	//}
	
	return Action_AltarToCurse;
}

void Mage::InitSlots(int player_index)
{
	//Hand
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		m_hand_slots[i].m_status = CardSlot_Free;
		m_hand_slots[i].m_stack = Stack_Hand;
		m_hand_slots[i].m_index = i;

		m_hand_slots[i].m_shape_container.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_hand_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_hand_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_hand_slots[i].m_shape_container.setOutlineThickness(2);
		m_hand_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_hand_slots[i].m_shape.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_hand_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_hand_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_hand_slots[i].m_shape.setOutlineThickness(0);
		m_hand_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_hand_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_hand_slots[i].m_text.setCharacterSize(18);
		m_hand_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_hand_slots[i].m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_text.setString("");
	}

	//Library
	m_library_slot.m_stack = Stack_Library;

	m_library_slot.m_shape_container.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + player_index * 700, 1000));
	m_library_slot.m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_library_slot.m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_library_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_library_slot.m_shape_container.setOutlineThickness(2);
	m_library_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_library_slot.m_shape.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + player_index * 700, 1000));
	m_library_slot.m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_library_slot.m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_library_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_library_slot.m_shape.setOutlineThickness(0);
	m_library_slot.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

	m_library_slot.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_library_slot.m_text.setCharacterSize(18);
	m_library_slot.m_text.setColor(sf::Color(255, 255, 255, 255));
	m_library_slot.m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + player_index * 700, 1000));
	m_library_slot.m_text.setString("");

	//Graveyard
	m_graveyard_slot.m_stack = Stack_Graveyard;

	m_graveyard_slot.m_shape_container.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 6 + player_index * 700, 1000));
	m_graveyard_slot.m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_graveyard_slot.m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_graveyard_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_graveyard_slot.m_shape_container.setOutlineThickness(2);
	m_graveyard_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_graveyard_slot.m_shape.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 6 + player_index * 700, 1000));
	m_graveyard_slot.m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
	m_graveyard_slot.m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
	m_graveyard_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_graveyard_slot.m_shape.setOutlineThickness(0);
	m_graveyard_slot.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

	m_graveyard_slot.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_graveyard_slot.m_text.setCharacterSize(18);
	m_graveyard_slot.m_text.setColor(sf::Color(0, 0, 0, 255));
	m_graveyard_slot.m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 6 + player_index * 700, 1000));
	m_graveyard_slot.m_text.setString("");
}


void Mage::InitCards()
{
	m_cards.push_back(Card(Mana_Fire, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Fire, Mana_1, m_index));
	//m_cards.push_back(Card(Mana_Fire, Mana_1, m_index));	//weakness
	m_cards.push_back(Card(Mana_Fire, Mana_2, m_index));

	m_cards.push_back(Card(Mana_Water, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Water, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Water, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Water, Mana_2, m_index));

	m_cards.push_back(Card(Mana_Earth, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Earth, Mana_1, m_index));
	//m_cards.push_back(Card(Mana_Earth, Mana_1, m_index));	//weakness
	m_cards.push_back(Card(Mana_Earth, Mana_2, m_index));

	m_cards.push_back(Card(Mana_Air, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Air, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Air, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Air, Mana_2, m_index));

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

Actions Mage::DrawCard(int nb_cards)
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

	return Action_DrawCard;
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


void CardSlot::GetCard(Card& card)
{
	m_card.m_value = card.m_value; 
	m_card.m_type = card.m_type;
	m_card.m_owner = card.m_owner;

	//display
	m_shape.setFillColor(GetManaColor(card.m_type));
	m_text.setString(std::to_string((int)(card.m_value)));
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
	if ((*CurrentGame).m_hovered_slot == NULL &&
		(*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - m_shape_container.getSize().x / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + m_shape_container.getSize().x / 2
		&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - m_shape_container.getSize().y / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + m_shape_container.getSize().y / 2)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_slot = this;
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
	if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Hand && (*CurrentGame).m_selected_slot->m_status == CardSlot_Occupied && m_stack == Stack_AltarSlot)
	{
		(*CurrentGame).m_play_card_slot = (*CurrentGame).m_selected_slot;
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Altar && m_stack == Stack_MonsterCurses)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_Library)
	{
		(*CurrentGame).m_target_slot = this;
	}
}