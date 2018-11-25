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
	NewGame();
}

void InGameState::NewGame()
{
	InitTable();

	m_mages.clear();
	for (int i = 0; i < NB_PLAYERS_MAX; i++)
	{
		m_mages.push_back(Mage(i));
		m_mages.back().InitSlots(i);
		m_mages.back().InitCards();
		m_mages.back().ShuffleLibrary();
		m_mages.back().DrawCard(5);
	}

	//Monsters
	m_monsters.clear();
	for (int i = 0; i < NB_MONSTERS; i++)
	{
		m_monsters.push_back(Monster());
	}
	SummonMonster();

	//Blessings
	m_blessings.clear();
	for (int i = 0; i < NB_BLESSING_TYPES; i++)
	{
		m_blessings.push_back(Blessing((BlessingType)i, i));
	}
	InitBlessings();
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
		if (monster.m_curses[i].m_costs.empty())
		{
			m_monster_curses_names[i].setColor(sf::Color(0, 255, 0, 255));
		}

		for (int j = 0; j < monster.m_curses[i].m_costs.size(); j++)
		{
			m_monster_curses_costs[i][j].GetCard(monster.m_curses[i].m_costs[j]);
			m_monster_curses_costs[i][j].m_status = CardSlot_Occupied;
		}
	}

	//display monster
	m_monster_name.setString(monster.m_display_name);
	int textWidth = m_monster_name.getGlobalBounds().width;
	m_monster_name.setPosition(sf::Vector2f(m_monster_slot.m_shape.getPosition().x - textWidth * 0.5f, 24));

	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	string filename;
	GetMonsterFilename(filename, (int)monster.m_type);

	sf::Texture* texture = loader->getTexture(filename);
	m_monster_slot.m_shape.setTexture(texture);
}

void InGameState::InitBlessings()
{
	for (int i = 0; i < m_blessings.size(); i++)
	{
		Blessing& blessing = m_blessings[i];

		m_blessing_names[i].setString(blessing.m_display_name);
		m_blessing_descriptions[i].setString(blessing.m_description);

		for (int j = 0; j < blessing.m_costs.size(); j++)
		{
			m_blessing_costs[i][j].GetCard(blessing.m_costs[j]);
			m_blessing_costs[i][j].m_status = CardSlot_Occupied;
		}
	}
}

void InGameState::GetMonsterFilename(string& filename, int monster_type)
{
	ostringstream ss;
	ss << "2D/monster_" << monster_type << ".png";
	string textureName = ss.str();
	filename = ss.str();
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

	m_altar_slot.m_shape_container.setPosition(sf::Vector2f(800 + ((CARD_WIDTH +20) * NB_CARDS_ALTAR) * 0.5, 500));
	m_altar_slot.m_shape_container.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10) * 0.5, CARD_HEIGHT / 2 + 5));
	m_altar_slot.m_shape_container.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_CARDS_ALTAR + 10, CARD_HEIGHT + 10));
	m_altar_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_altar_slot.m_shape_container.setOutlineThickness(2);
	m_altar_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_altar_slot.m_shape.setPosition(sf::Vector2f(800 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 500));
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

		m_altar_slots[i].m_shape_container.setPosition(sf::Vector2f(800 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_altar_slots[i].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_altar_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_altar_slots[i].m_shape_container.setOutlineThickness(2);
		m_altar_slots[i].m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

		m_altar_slots[i].m_shape.setPosition(sf::Vector2f(800 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
		m_altar_slots[i].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
		m_altar_slots[i].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
		m_altar_slots[i].m_shape.setOutlineThickness(0);
		m_altar_slots[i].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

		m_altar_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_altar_slots[i].m_text.setCharacterSize(18);
		m_altar_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_altar_slots[i].m_text.setPosition(sf::Vector2f(800 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i, 500));
		m_altar_slots[i].m_text.setString("");
	}

	//Monster
	TextureLoader *loader;
	loader = TextureLoader::getInstance();

	for (int i = 0; i < NB_MONSTER_TYPES; i++)
	{
		string textureName;
		GetMonsterFilename(textureName, i);
		sf::Texture* texture = loader->loadTexture(textureName, MONSTER_WIDTH, MONSTER_HEIGHT);
	}

	m_monster_name.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_monster_name.setCharacterSize(24);
	m_monster_name.setColor(sf::Color(255, 255, 255, 255));
	m_monster_name.setPosition(sf::Vector2f(800, 24));
	m_monster_name.setString("");
	
	m_monster_slot.m_stack = Stack_None;

	m_monster_slot.m_shape_container.setPosition(sf::Vector2f(800 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 200));
	m_monster_slot.m_shape_container.setOrigin(sf::Vector2f(MONSTER_WIDTH * 0.5f, MONSTER_HEIGHT * 0.5f));
	m_monster_slot.m_shape_container.setSize(sf::Vector2f(MONSTER_WIDTH, MONSTER_HEIGHT));
	m_monster_slot.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_monster_slot.m_shape_container.setOutlineThickness(2);
	m_monster_slot.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_monster_slot.m_shape.setPosition(sf::Vector2f(800 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 200));
	m_monster_slot.m_shape.setOrigin(sf::Vector2f(MONSTER_WIDTH * 0.5f, MONSTER_HEIGHT * 0.5f));
	m_monster_slot.m_shape.setSize(sf::Vector2f(MONSTER_WIDTH, MONSTER_HEIGHT));
	m_monster_slot.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_monster_slot.m_shape.setOutlineThickness(0);
	m_monster_slot.m_shape.setTextureRect(sf::IntRect(0, 0, 256, 256));

	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		m_monster_curses_names[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_monster_curses_names[i].setCharacterSize(18);
		m_monster_curses_names[i].setColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_names[i].setPosition(sf::Vector2f(1400, 100 - CARD_HEIGHT / 2 - 50 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_names[i].setString("");

		m_monster_curses_descriptions[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_monster_curses_descriptions[i].setCharacterSize(18);
		m_monster_curses_descriptions[i].setColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_descriptions[i].setPosition(sf::Vector2f(1400, 100 - CARD_HEIGHT / 2 - 30 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_descriptions[i].setString("");
		
		m_monster_curses_slots[i].m_status = CardSlot_Free;
		m_monster_curses_slots[i].m_stack = Stack_MonsterCurses;
		m_monster_curses_slots[i].m_index = i;

		m_monster_curses_slots[i].m_shape.setPosition(sf::Vector2f(1400 + ((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX) * 0.5, 100 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_slots[i].m_shape.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10) * 0.5, CARD_HEIGHT / 2 + 5));
		m_monster_curses_slots[i].m_shape.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10, CARD_HEIGHT + 10));
		m_monster_curses_slots[i].m_shape.setOutlineColor(sf::Color(255, 255, 255, 0));
		m_monster_curses_slots[i].m_shape.setOutlineThickness(0);
		m_monster_curses_slots[i].m_shape.setFillColor(sf::Color(0, 0, 0, 255));

		m_monster_curses_slots[i].m_shape_container.setPosition(sf::Vector2f(1400 + ((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX) * 0.5, 100 + (CARD_HEIGHT + 70) * i));
		m_monster_curses_slots[i].m_shape_container.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10) * 0.5, CARD_HEIGHT / 2 + 5));
		m_monster_curses_slots[i].m_shape_container.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10, CARD_HEIGHT + 10));
		m_monster_curses_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_monster_curses_slots[i].m_shape_container.setOutlineThickness(2);
		m_monster_curses_slots[i].m_shape_container.setFillColor(sf::Color(255, 255, 255, 0));

		for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
		{
			m_monster_curses_costs[i][j].m_status = CardSlot_Free;
			m_monster_curses_costs[i][j].m_stack = Stack_MonsterCosts;
			m_monster_curses_costs[i][j].m_index = i;

			m_monster_curses_costs[i][j].m_shape.setPosition(sf::Vector2f(1400 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_monster_curses_costs[i][j].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
			m_monster_curses_costs[i][j].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
			m_monster_curses_costs[i][j].m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
			m_monster_curses_costs[i][j].m_shape.setOutlineThickness(0);
			m_monster_curses_costs[i][j].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

			m_monster_curses_costs[i][j].m_shape_container.setPosition(sf::Vector2f(1400 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_monster_curses_costs[i][j].m_shape_container.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
			m_monster_curses_costs[i][j].m_shape_container.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
			m_monster_curses_costs[i][j].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
			m_monster_curses_costs[i][j].m_shape_container.setOutlineThickness(2);
			m_monster_curses_costs[i][j].m_shape_container.setFillColor(sf::Color(255, 255, 255, 0));

			m_monster_curses_costs[i][j].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_monster_curses_costs[i][j].m_text.setCharacterSize(18);
			m_monster_curses_costs[i][j].m_text.setColor(sf::Color(0, 0, 0, 255));
			m_monster_curses_costs[i][j].m_text.setPosition(sf::Vector2f(1400 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_monster_curses_costs[i][j].m_text.setString("");
		}
	}

	//Blessings
	for (int i = 0; i < NB_BLESSING_TYPES; i++)
	{
		m_blessing_names[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_blessing_names[i].setCharacterSize(18);
		m_blessing_names[i].setColor(sf::Color(255, 255, 255, 255));
		m_blessing_names[i].setPosition(sf::Vector2f(100, 100 - CARD_HEIGHT / 2 - 50 + (CARD_HEIGHT + 70) * i));
		m_blessing_names[i].setString("");

		m_blessing_descriptions[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_blessing_descriptions[i].setCharacterSize(18);
		m_blessing_descriptions[i].setColor(sf::Color(255, 255, 255, 255));
		m_blessing_descriptions[i].setPosition(sf::Vector2f(100, 100 - CARD_HEIGHT / 2 - 30 + (CARD_HEIGHT + 70) * i));
		m_blessing_descriptions[i].setString("");

		m_blessing_slots[i].m_status = CardSlot_Occupied;
		m_blessing_slots[i].m_stack = Stack_Blessings;
		m_blessing_slots[i].m_index = i;

		m_blessing_slots[i].m_shape.setPosition(sf::Vector2f(100 + ((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX) * 0.5, 100 + (CARD_HEIGHT + 70) * i));
		m_blessing_slots[i].m_shape.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10) * 0.5, CARD_HEIGHT / 2 + 5));
		m_blessing_slots[i].m_shape.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10, CARD_HEIGHT + 10));
		m_blessing_slots[i].m_shape.setOutlineColor(sf::Color(255, 255, 255, 0));
		m_blessing_slots[i].m_shape.setOutlineThickness(0);
		m_blessing_slots[i].m_shape.setFillColor(sf::Color(0, 0, 0, 255));

		m_blessing_slots[i].m_shape_container.setPosition(sf::Vector2f(100 + ((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX) * 0.5, 100 + (CARD_HEIGHT + 70) * i));
		m_blessing_slots[i].m_shape_container.setOrigin(sf::Vector2f(((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10) * 0.5, CARD_HEIGHT / 2 + 5));
		m_blessing_slots[i].m_shape_container.setSize(sf::Vector2f((CARD_WIDTH + 20) * NB_MONSTER_SPELLS_MAX + 10, CARD_HEIGHT + 10));
		m_blessing_slots[i].m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		m_blessing_slots[i].m_shape_container.setOutlineThickness(2);
		m_blessing_slots[i].m_shape_container.setFillColor(sf::Color(255, 255, 255, 0));

		for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
		{
			m_blessing_costs[i][j].m_status = CardSlot_Free;
			m_blessing_costs[i][j].m_stack = Stack_None;
			m_blessing_costs[i][j].m_index = i;

			m_blessing_costs[i][j].m_shape.setPosition(sf::Vector2f(100 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_blessing_costs[i][j].m_shape.setOrigin(sf::Vector2f(CARD_WIDTH / 2, CARD_HEIGHT / 2));
			m_blessing_costs[i][j].m_shape.setSize(sf::Vector2f(CARD_WIDTH, CARD_HEIGHT));
			m_blessing_costs[i][j].m_shape.setOutlineColor(sf::Color(0, 0, 0, 255));
			m_blessing_costs[i][j].m_shape.setOutlineThickness(2);
			m_blessing_costs[i][j].m_shape.setFillColor(sf::Color(255, 255, 255, 0));

			m_blessing_costs[i][j].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_blessing_costs[i][j].m_text.setCharacterSize(18);
			m_blessing_costs[i][j].m_text.setColor(sf::Color(0, 0, 0, 255));
			m_blessing_costs[i][j].m_text.setPosition(sf::Vector2f(100 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * j, 100 + (CARD_HEIGHT + 70) * i));
			m_blessing_costs[i][j].m_text.setString("");
		}
	}
}

Curse::Curse(int cost, int nb_costs, int index, CurseType type)
{
	m_index = index;
	m_status = CardSlot_Occupied;
	m_type = type;

	if (cost == 0 || nb_costs == 0)
	{
		m_display_name = "No curse";
		m_description = "";
		m_status = CardSlot_Free;
		return;
	}

	if (nb_costs > SPELL_NB_COSTS_MAX)
	{
		nb_costs = SPELL_NB_COSTS_MAX;
	}

	if (nb_costs > cost)
	{
		printf("Creating a curse with more nb_costs than cost possible. (%d, %d)\n", nb_costs, cost);
		nb_costs = cost;
	}

	int half = cost % nb_costs;
	int avg_cost = cost / nb_costs;
	
	for (int i = 0; i < nb_costs; i++)
	{
		int type = RandomizeIntBetweenValues((int)Mana_Fire, (int)NB_MANATYPES);

		if (i < half)
		{
			int cost = avg_cost > 3 ? 3 : avg_cost;
			m_costs.push_back(Card((ManaType)type, (ManaValue)(cost + 1), -1));
		}
		else
		{
			int cost = avg_cost - 1 > 3 ? 4 : avg_cost;
			m_costs.push_back(Card((ManaType)type, (ManaValue)(cost), -1));
		}
	}

	m_display_name = "Curse";
	m_description = "Burns one card slot at random in player hand.";
}

bool Curse::Effect()
{
	//do something
	//printf("Monster attack (%d).\n", m_index);

	//int r = RandomizeIntBetweenValues(0, NB_PLAYERS_MAX);

	return true;
}

Blessing::Blessing(BlessingType type, int index)
{
	m_index = index;
	m_status = CardSlot_Occupied;
	m_type = type;

	switch (m_type)
	{
		case Blessing_Heal:
		{
			m_costs.push_back(Card((ManaType)NB_MANATYPES, (ManaValue)2, -1));

			m_display_name = "Healing";
			m_description = "Heals a burnt slot in your hand.";

			break;
		}
		case Blessing_Draw:
		{
			m_costs.push_back(Card((ManaType)NB_MANATYPES, (ManaValue)2, -1));

			m_display_name = "Drawing";
			m_description = "Draw one card.";

			break;
		}
		case Blessing_Telepathy:
		{
			m_costs.push_back(Card((ManaType)NB_MANATYPES, (ManaValue)2, -1));

			m_display_name = "Telepathy";
			m_description = "Pass on your turn to another mage.";

			break;
		}
	}
}

Monster::Monster(MonsterType type)
{
	if (type == NB_MONSTER_TYPES)
	{
		type = (MonsterType)RandomizeIntBetweenValues(0, NB_MONSTER_TYPES - 1);
	}
	m_type = type;

	switch (m_type)
	{
		case Monster_Squale:
			m_display_name = "The Squale";
			break;
		case Monster_Kraken:
			m_display_name = "The Kraken";
			break;
		case Monster_Spiky:
			m_display_name = "The Spiked Demon";
			break;
		case Monster_Golem:
			m_display_name = "Stone Golem";
			break;
		case Monster_Ogre:
			m_display_name = "Axe Ogre";
			break;
		case Monster_Yeti:
			m_display_name = "Snow King";
			break;
		case Monster_Blademaster:
			m_display_name = "The Blademaster";
			break;
		case Monster_Wolf:
			m_display_name = "Night Wolf";
			break;
		case Monster_Tatoo:
			m_display_name = "Armored Tatoo";
			break;
		case Monster_Frankenstein:
			m_display_name = "Frankenstein";
			break;
		default:
			m_display_name = "Unkown";
			break;
	}

	//first curses to trigger to last in the vector
	m_curses.push_back(Curse(5, 3, m_curses.size(), Curse_BurnRandomPlayer));//curse 5
	m_curses.push_back(Curse(4, 3, m_curses.size(), Curse_BurnRandomPlayer));//curse 4
	m_curses.push_back(Curse(3, 2, m_curses.size(), Curse_BurnRandomPlayer));//curse 3
	m_curses.push_back(Curse(3, 2, m_curses.size(), Curse_BurnRandomPlayer));//curse 2
	m_curses.push_back(Curse(2, 1, m_curses.size(), Curse_BurnRandomPlayer));//curse 1
	m_curses.push_back(Curse(0, 0, m_curses.size(), Curse_NoEffect));//curse 0
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

	//m_mages[p].m_library_slot.Update((*CurrentGame).m_mouse_click);
	//m_mages[p].m_graveyard_slot.Update((*CurrentGame).m_mouse_click);
	m_mages[p].m_end_of_turn.Update((*CurrentGame).m_mouse_click);
	
	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
		{
			if (m_monster_curses_costs[i][j].m_status == CardSlot_Burnt)//costs slot burnt = collect reward
			{
				m_monster_curses_costs[i][j].Update((*CurrentGame).m_mouse_click);
			}
		}
	}
	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		if (m_monster_curses_slots[i].m_status != CardSlot_Burnt)//curse slot burnt = cannot be dispelled anymore
		{
			m_monster_curses_slots[i].Update((*CurrentGame).m_mouse_click);
		}
	}

	for (int i = 0; i < NB_BLESSING_TYPES; i++)
	{
		m_blessing_slots[i].Update((*CurrentGame).m_mouse_click);
	}

	bool altar_selection[NB_CARDS_ALTAR];
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		altar_selection[i] = m_altar_slots[i].m_selected;
	}
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		m_altar_slots[i].Update((*CurrentGame).m_mouse_click);
	}
	for (int i = 0; i < NB_CARDS_ALTAR; i++)
	{
		//cumulative selection in altar
		if (altar_selection[i] && !m_altar_slots[i].m_selected && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_AltarSlot)
		{
			m_altar_slots[i].m_selected = true;
		}
	}

	m_altar_slot.Update((*CurrentGame).m_mouse_click);

	//Actions
	Actions played = Action_None;
	if ((*CurrentGame).m_play_card_slot != NULL)
	{
		played = PlayCard(p, (*CurrentGame).m_play_card_slot->m_index, (*CurrentGame).m_target_slot->m_index);
	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_MonsterCurses)
	{
		int index = (*CurrentGame).m_target_slot->m_index;
		played = AltarSpendMana(p, Stack_MonsterCurses, index);

		if (played == Action_ManaSuccess)
		{
			//Dispell
			m_monster_curses_slots[index].m_status = CardSlot_Burnt;

			for (int i = 0; i < SPELL_NB_COSTS_MAX; i++)
			{
				if (i < NB_MANATYPES)
				{
					m_monster_curses_costs[index][i].m_status = CardSlot_Burnt;
					m_monster_curses_costs[index][i].m_card.m_type = (ManaType)i;
					m_monster_curses_costs[index][i].m_shape.setFillColor(CardSlot::GetManaColor((ManaType)i));
					m_monster_curses_costs[index][i].m_card.m_value = (ManaValue)2;
					m_monster_curses_costs[index][i].m_text.setString(std::to_string(2));
				}
				else
				{
					m_monster_curses_costs[index][i].m_status = CardSlot_Free;
				}
			}

			m_monster_curses_names[index].setString("Curse dispelled");
			m_monster_curses_names[index].setColor(sf::Color(0, 255, 0, 255));

			m_monster_curses_descriptions[index].setString("Choose a reward that will go in your graveyard");
			m_monster_curses_descriptions[index].setColor(sf::Color(0, 255, 0, 255));
		}
	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_MonsterCosts)
	{
		//get reward
		m_mages[p].m_graveyard.push_back((*CurrentGame).m_target_slot->m_card);

		//clean rewards
		int index = (*CurrentGame).m_target_slot->m_index;
		for (int i = 0; i < NB_MANATYPES; i++)
		{
			m_monster_curses_costs[index][i].m_status = CardSlot_Free;
		}

		m_monster_curses_slots[index].m_status = CardSlot_Occupied;
		m_monster_curses_descriptions[index].setString("");

	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_Blessings)
	{
		played = AltarSpendMana(p, Stack_Blessings, (*CurrentGame).m_target_slot->m_index);

		if (played == Action_ManaSuccess)
		{
			//Use blessing
			int index = (*CurrentGame).m_target_slot->m_index;
			BlessingPlayer(p, m_blessings[index].m_type);
		}
	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_Library)
	{
		played = m_mages[p].DrawCard();
	}
	else if ((*CurrentGame).m_target_slot != NULL && (*CurrentGame).m_target_slot->m_stack == Stack_EndOfTurn)
	{
		if ((*CurrentGame).m_target_slot->m_status != CardSlot_Burnt)
		{
			EndOfTurn();
		}
		else
		{
			NewGame();
		}
	}

	//Cards UI automatic update
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
		if (m_monster_curses_slots[m_monsters.back().m_curses.size() - 1].m_status == CardSlot_Occupied)
		{
			m_monster_curses_names[m_monsters.back().m_curses.size() - 1].setColor(sf::Color(255, 0, 0, 255));
			m_monster_curses_descriptions[m_monsters.back().m_curses.size() - 1].setColor(sf::Color(255, 0, 0, 255));
		}
		else
		{
			m_monster_curses_names[m_monsters.back().m_curses.size() - 1].setColor(sf::Color(0, 255, 0, 255));
			m_monster_curses_descriptions[m_monsters.back().m_curses.size() - 1].setColor(sf::Color(0, 255, 0, 255));
		}
	}

	//useless?
	(*CurrentGame).updateScene(deltaTime);


	this->mainWindow->clear();
}

bool InGameState::AllPlayersDead()
{
	bool all_players_dead = true;
	for (int i = 0; i < NB_PLAYERS_MAX; i++)
	{
		if (m_mages[i].m_is_alive)
		{
			all_players_dead = false;
			break;
		}
	}

	return all_players_dead;
}

void InGameState::EndOfTurn()
{
	//Reset selection
	(*CurrentGame).m_selected_slot = NULL;

	//Monster attack
	if (!m_monsters.empty())
	{
		Monster& monster = m_monsters.back();
		if (!monster.m_curses.empty())
		{
			//attack with current curse
			Curse& curse = monster.m_curses.back();
			if (m_monster_curses_slots[curse.m_index].m_status == CardSlot_Occupied && m_monster_curses_costs[curse.m_index][0].m_status != CardSlot_Free)
			{
				switch (monster.m_curses.back().m_type)
				{
					case Curse_NoEffect:
					{
						break;
					}
					case Curse_BurnRandomPlayer:
					{
						//pick a random player alive
						vector<int> alive_players;
						for (int i = 0; i < NB_PLAYERS_MAX; i++)
						{
							if (m_mages[i].m_is_alive)
							{
								alive_players.push_back(i);
							}
						}
						int r = RandomizeIntBetweenValues(0, alive_players.size() - 1);
						int index = alive_players[r];

						//Burn the random player
						BurnPlayer(index);
						break;
					}
				}	
			}

			//get rid of current curse and go to next curse
			m_monster_curses_slots[monster.m_curses.size() - 1].m_status = CardSlot_Free;
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

		//setting next turn
		if (AllPlayersDead() == false)
		{
			//pass on turn to next alive player
			int p = (m_current_player + 1) % NB_PLAYERS_MAX;
			while (m_mages[p].m_is_alive == false)
			{
				p = (p + 1) % NB_PLAYERS_MAX;
			}
			m_current_player = p;
			//draw cards to complete hand
			int cards_to_draw = NB_CARDS_HAND - m_mages[m_current_player].GetHandCount();
			m_mages[m_current_player].DrawCard(cards_to_draw);
		}
		else
		{
			//GAME OVER
			m_mages[m_current_player].m_end_of_turn.m_text.setString("RESTART NEW GAME");
			m_mages[m_current_player].m_end_of_turn.m_text.setColor(sf::Color(255, 255, 255, 255));
			m_mages[m_current_player].m_end_of_turn.m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 3 - m_mages[m_current_player].m_end_of_turn.m_text.getGlobalBounds().width / 2 + m_current_player * 700, m_mages[m_current_player].m_end_of_turn.m_text.getPosition().y));//- 1 for pixel perfection
			m_mages[m_current_player].m_end_of_turn.m_status = CardSlot_Burnt;
			//player.m_end_of_turn.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
		}
	}
}

bool InGameState::BurnPlayer(int player_index)
{
	Mage& player = m_mages[player_index];

	//Pick an unburnt card at random. this card slot is going to be burnt.
	vector<int> clean_cards;
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		if (player.m_hand_slots[i].m_status != CardSlot_Burnt)
		{
			clean_cards.push_back(i);
		}
	}

	int r = RandomizeIntBetweenValues(0, clean_cards.size() - 1);
	int index = clean_cards[r];
	
	//Burn card
	player.m_hand_slots[index].m_status = CardSlot_Burnt;
	player.m_hand_slots[index].m_shape.setFillColor(sf::Color(128, 128, 128, 255));

	printf("Monster attack. Burning player %d (slot: %d).\n", player_index, index);

	if (clean_cards.size() == 1)
	{
		player.m_is_alive = false;
		player.m_end_of_turn.m_text.setString("DEAD");
		player.m_end_of_turn.m_text.setColor(sf::Color(255, 0, 0, 255));
		player.m_end_of_turn.m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 3 - player.m_end_of_turn.m_text.getGlobalBounds().width / 2 + player_index * 700, player.m_end_of_turn.m_text.getPosition().y));//- 1 for pixel perfection
		player.m_end_of_turn.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));

		printf("Player %d is dead.\n", player.m_index);

		return true;
	}

	return false;
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

		//"End turn" button
		if (m_current_player == p || m_mages[p].m_is_alive == false)
		{
			(*CurrentGame).m_mainScreen.draw(m_mages[p].m_end_of_turn.m_shape_container);
			(*CurrentGame).m_mainScreen.draw(m_mages[p].m_end_of_turn.m_shape);
			(*CurrentGame).m_mainScreen.draw(m_mages[p].m_end_of_turn.m_text);
		}
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
	(*CurrentGame).m_mainScreen.draw(m_monster_name);
	(*CurrentGame).m_mainScreen.draw(m_monster_slot.m_shape);
	(*CurrentGame).m_mainScreen.draw(m_monster_slot.m_shape_container);

	for (int i = 0; i < NB_MONSTER_SPELLS_MAX; i++)
	{
		if (m_monster_curses_slots[i].m_status != CardSlot_Free)
		{
			(*CurrentGame).m_mainScreen.draw(m_monster_curses_slots[i].m_shape);
			if (m_monster_curses_slots[i].m_status == CardSlot_Occupied)
			{
				(*CurrentGame).m_mainScreen.draw(m_monster_curses_slots[i].m_shape_container);
			}

			(*CurrentGame).m_mainScreen.draw(m_monster_curses_names[i]);
			(*CurrentGame).m_mainScreen.draw(m_monster_curses_descriptions[i]);

			for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
			{
				if (m_monster_curses_costs[i][j].m_status != CardSlot_Free)
				{
					if (m_monster_curses_costs[i][j].m_status == CardSlot_Burnt)
					{
						(*CurrentGame).m_mainScreen.draw(m_monster_curses_costs[i][j].m_shape_container);
					}
					(*CurrentGame).m_mainScreen.draw(m_monster_curses_costs[i][j].m_shape);
					(*CurrentGame).m_mainScreen.draw(m_monster_curses_costs[i][j].m_text);
				}
			}
		}
	}

	//Blessings
	for (int i = 0; i < NB_BLESSING_TYPES; i++)
	{
		if (m_blessing_slots[i].m_status != CardSlot_Free)
		{
			(*CurrentGame).m_mainScreen.draw(m_blessing_slots[i].m_shape);
			(*CurrentGame).m_mainScreen.draw(m_blessing_slots[i].m_shape_container);

			(*CurrentGame).m_mainScreen.draw(m_blessing_names[i]);
			(*CurrentGame).m_mainScreen.draw(m_blessing_descriptions[i]);

			for (int j = 0; j < SPELL_NB_COSTS_MAX; j++)
			{
				if (m_blessing_costs[i][j].m_status != CardSlot_Free)
				{
					(*CurrentGame).m_mainScreen.draw(m_blessing_costs[i][j].m_shape);
					(*CurrentGame).m_mainScreen.draw(m_blessing_costs[i][j].m_text);
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

void InGameState::UseAltarCard(int index)
{
	m_altar_slots[index].m_status = CardSlot_Free;
	m_mages[m_altar_slots[index].m_card.m_owner].m_graveyard.push_back(m_altar_slots[index].m_card);
	m_altar_slots[index].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
	m_altar_slots[index].m_text.setString("");
}

Actions InGameState::AltarSpendMana(int player_index, CardStack stack, int slot)
{
	vector<int> mana[NB_MANATYPES];

	vector<Card> costs;
	switch (stack)
	{
		case Stack_MonsterCurses:
		{
			if (m_monsters.back().m_curses[slot].m_status == CardSlot_Free)
			{
				return Action_None;
			}

			for (int i = 0; i < m_monsters.back().m_curses[slot].m_costs.size(); i++)
			{
				costs.push_back(m_monsters.back().m_curses[slot].m_costs[i]);
			}
			break;
		}
		case Stack_Blessings:
		{
			for (int i = 0; i < m_blessings[slot].m_costs.size(); i++)
			{
				costs.push_back(m_blessings[slot].m_costs[i]);
			}
			break;
		}
		default:
			return Action_None;
	}

	//Colored mana first, then uncolored
	for (int colored = 1; colored >= 0; colored--)
	{
		for (int i = 0; i < costs.size(); i++)
		{
			Card& cost = costs[i];

			//Gathering remaining mana
			for (int m = 0; m < NB_MANATYPES; m++)
			{
				mana[m].clear();
				for (int k = 0; k < NB_CARDS_ALTAR; k++)
				{
					if (m_altar_slots[k].m_status == CardSlot_Occupied)
					{
						if (m_altar_slots[k].m_selected == true || m_altar_slot.m_selected == true)
						{
							if (m_altar_slots[k].m_card.m_type == (ManaType)m)
							{
								mana[m].push_back(k);
							}
						}
					}
				}
			}

			if (cost.m_type == NB_MANATYPES && colored == 1)
			{
				continue;
			}

			if (cost.m_type != NB_MANATYPES && colored == 0)
			{
				continue;
			}

			//Get all combinations possible
			int index = -1;
			int mana_waste = -1;
			int nb_cards = -1;
			int slot = -1;
			vector<vector<int> > possibilities;
			vector<int> possibility;

			for (int j = 0; j < NB_MANATYPES; j++)//for uncolored
			{
				if (colored == 1)
				{
					j = cost.m_type;// and will break at the end of the loop
				}

				//insert loop here
				int comb_size = mana[j].size();
				if (comb_size == 0)
				{
					if (colored == 1)
					{
						printf("Cannot dispell curse. Insufficient mana (0 mana of type %d).\n", cost.m_type);
						for (int k = 0; k < NB_CARDS_ALTAR; k++)
						{
							if (m_altar_slots[k].m_status == CardSlot_Temp)
							{
								m_altar_slots[k].m_status = CardSlot_Occupied;
							}
						}

						return Action_None;
					}
					else
					{
						continue;
					}
				}

				switch (comb_size)
				{
				case 1:
					possibilities.push_back(mana[j]);
					possibility.clear();
					break;
				case 2:

					possibility.push_back(mana[j][0]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibilities.push_back(possibility);
					possibility.clear();
					break;
				case 3:
					possibility.push_back(mana[j][0]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					break;
				case 4:
					possibility.push_back(mana[j][0]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][2]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][2]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][2]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][2]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					possibility.push_back(mana[j][0]);
					possibility.push_back(mana[j][1]);
					possibility.push_back(mana[j][2]);
					possibility.push_back(mana[j][3]);
					possibilities.push_back(possibility);
					possibility.clear();
					break;
				default:
					break;
				}

				if (colored == 1)
				{
					break;//only for colored costs
				}
			}

			//Assess all combinations
			int pb_size = possibilities.size();

			for (int p = 0; p < pb_size; p++)
			{
				int mana_sum = 0;
				int vector_size = possibilities[p].size();
				int min_slot = possibilities[p].front();
				for (int m = 0; m < vector_size; m++)
				{
					int k = possibilities[p][m];
					mana_sum += m_altar_slots[k].m_card.m_value;
				}

				//we're looking for the lowest mana waste, then the lowest number of cards possible, that fits the cost
				if (mana_sum >= cost.m_value && (mana_waste < 0 || mana_sum - cost.m_value <= mana_waste) && (nb_cards < 0 || vector_size <= nb_cards))
				{
					if (mana_sum - cost.m_value == mana_waste && vector_size == nb_cards && slot >= 0 && min_slot > slot)
					{
						continue;//for ex-aequo, we keep the solution on the most left
					}

					index = p;
					mana_waste = mana_sum - cost.m_value;
					nb_cards = vector_size;
					slot = min_slot;
				}
			}

			if (index < 0)
			{
				printf("Cannot dispell curse. Insufficient mana (%d mana of type %d).\n", cost.m_value, cost.m_type);

				if (colored == 1)
				{
					printf("Altar slots indexes for selected cards of this type of mana: ");
					for (int k = 0; k < mana[k].size(); k++)
					{
						printf("%d ", mana[cost.m_type][k]);
					}
					printf("\n");
				}

				for (int k = 0; k < NB_CARDS_ALTAR; k++)
				{
					if (m_altar_slots[k].m_status == CardSlot_Temp)
					{
						m_altar_slots[k].m_status = CardSlot_Occupied;
					}

				}

				return Action_None;
			}

			//Execute the choice of combination
			for (int m = 0; m < possibilities[index].size(); m++)
			{
				int k = possibilities[index][m];
				m_altar_slots[k].m_status = CardSlot_Temp;
			}
		}
	}

	//Discard used cards
	for (int k = 0; k < NB_CARDS_ALTAR; k++)
	{
		if (m_altar_slots[k].m_status == CardSlot_Temp)
		{
			m_altar_slots[k].m_status = CardSlot_Free;
			m_mages[m_altar_slots[k].m_card.m_owner].m_graveyard.push_back(m_altar_slots[k].m_card);
			m_altar_slots[k].m_shape.setFillColor(CardSlot::GetStatusColor(CardSlot_Free));
			m_altar_slots[k].m_text.setString("");
		}
	}

	m_altar_slot.m_selected = false;
	for (int k = 0; k < NB_CARDS_ALTAR; k++)
	{
		m_altar_slots[k].m_selected = false;
	}
	(*CurrentGame).m_selected_slot = NULL;

	return Action_ManaSuccess;
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

	//End of turn
	m_end_of_turn.m_stack = Stack_EndOfTurn;

	m_end_of_turn.m_shape_container.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 3 + player_index * 700, 1000));
	m_end_of_turn.m_shape_container.setOrigin(sf::Vector2f((CARD_WIDTH * 5 + 20 * 4) / 2, CARD_HEIGHT / 2));
	m_end_of_turn.m_shape_container.setSize(sf::Vector2f(CARD_WIDTH*5 + 20*4, CARD_HEIGHT));
	m_end_of_turn.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	m_end_of_turn.m_shape_container.setOutlineThickness(2);
	m_end_of_turn.m_shape_container.setFillColor(sf::Color(0, 0, 0, 0));

	m_end_of_turn.m_shape.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 3 + player_index * 700, 1000));
	m_end_of_turn.m_shape.setOrigin(sf::Vector2f((CARD_WIDTH * 5 + 20 * 4) / 2, CARD_HEIGHT / 2));
	m_end_of_turn.m_shape.setSize(sf::Vector2f(CARD_WIDTH*5 + 20*4, CARD_HEIGHT));
	m_end_of_turn.m_shape.setOutlineColor(sf::Color(0, 0, 0, 0));
	m_end_of_turn.m_shape.setOutlineThickness(0);
	m_end_of_turn.m_shape.setFillColor(sf::Color(0, 0, 0, 255));

	m_end_of_turn.m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_end_of_turn.m_text.setCharacterSize(18);
	m_end_of_turn.m_text.setColor(sf::Color(255, 255, 255, 255));
	m_end_of_turn.m_text.setString("END TURN");
	m_end_of_turn.m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * 3 - m_end_of_turn.m_text.getGlobalBounds().width / 2 + player_index * 700, 1000 - m_end_of_turn.m_text.getCharacterSize() / 2 - 1));//- 1 for pixel perfection
}

void Mage::InitCards()
{
	m_cards.clear();

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

	m_cards.push_back(Card(Mana_Lightning, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Lightning, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Lightning, Mana_1, m_index));
	m_cards.push_back(Card(Mana_Lightning, Mana_2, m_index));

	m_libary.clear();
	for (vector<Card>::iterator it = m_cards.begin(); it < m_cards.end(); it++)
	{
		m_libary.push_back(*it);
	}

	m_graveyard.clear();
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

int Mage::GetHandCount()
{
	int count = 0;
	for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
	{
		if (m_hand_slots[i].m_status == CardSlot_Occupied)
		{
			count++;
		}
	}

	return count;
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
	else if (type == Mana_Lightning)
	{
		return sf::Color(230, 230, 0, 255);
	}
	else if (type == Mana_Earth)
	{
		return sf::Color(0, 255, 0, 255);
	}
	else
	{
		return sf::Color(100, 100, 100, 255);
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
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_AltarSlot && m_stack == Stack_MonsterCurses)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Altar && m_stack == Stack_MonsterCurses)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_MonsterCosts)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_AltarSlot && m_stack == Stack_Blessings)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_slot && (*CurrentGame).m_selected_slot->m_stack == Stack_Altar && m_stack == Stack_Blessings)
	{
		(*CurrentGame).m_target_slot = this;
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_Library)
	{
		(*CurrentGame).m_target_slot = this;
		m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));
	}
	else if (m_hovered && mouse_click == Mouse_RightClick && m_stack == Stack_EndOfTurn)
	{
		(*CurrentGame).m_target_slot = this;
	}
}

bool InGameState::BlessingPlayer(int player_index, BlessingType type)
{
	Mage& mage = m_mages[player_index];

	switch (type)
	{
		case Blessing_Heal:
		{
			for (int i = 0; i < NB_CARDS_HAND_MAX; i++)
			{
				if (mage.m_hand_slots[i].m_status == CardSlot_Burnt)
				{
					mage.m_hand_slots[i].m_status = CardSlot_Free;
					return true;
				}
			}
			break;
		}
		case Blessing_Draw:
		{
			mage.DrawCard();
			break;
		}
		case Blessing_Telepathy:
		{
			m_current_player = (m_current_player + 1) % NB_PLAYERS_MAX;
			break;
		}
	}

	return false;
}