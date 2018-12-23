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

	//ROBOT
	InitRobots();
}

void InGameState::InitRobots()
{
	Robot& robot = m_robots[0];

	//Robot 1
	robot.SetModule(Module_Infirmary, Index_BodyU);

	robot.SetModule(Module_Deflectors, Index_BodyM);
	robot.SetEquipment(Equipment_LightPlate, Index_BodyM);

	robot.SetModule(Module_CrewQuarter, Index_BodyD);
	robot.SetEquipment(Equipment_CQExtension, Index_BodyD);

	robot.SetModule(Module_Generator, Index_LegR);
	robot.SetEquipment(Equipment_GeneratorBooster, Index_LegR);

	robot.SetModule(Module_Gadget, Index_LegL);
	robot.SetEquipment(Equipment_GadgetEMP, Index_LegL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootR);

	robot.SetModule(Module_Weapon, Index_ForearmL);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ForearmL);

	robot.SetModule(Module_Weapon, Index_ForearmR);
	robot.SetEquipment(Equipment_LightPlate, Index_ForearmR);

	robot.SetModule(Module_Radar, Index_ShoulderL);
	
	robot.SetModule(Module_Weapon, Index_ShoulderR);
	robot.SetEquipment(Equipment_WeaponsScope, Index_ShoulderR);

	robot.SetCrewMember(Crew_Pilot, Index_Head);
	robot.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot.SetCrewMember(Crew_Scientist, Index_LegR);
	robot.SetCrewMember(Crew_Engineer, Index_LegL);
	robot.SetCrewMember(Crew_Warrior, Index_ForearmL);

	robot.SetWeapon(Weapon_FireSword, Index_ForearmL);

	robot.Initialize();

	robot = m_robots[1];

	//Robot 2
	robot.SetModule(Module_Generator, Index_BodyU);
	robot.SetEquipment(Equipment_GeneratorBooster, Index_BodyU);
	robot.SetEquipment(Equipment_LightPlate, Index_BodyU);

	robot.SetModule(Module_CrewQuarter, Index_BodyM);
	robot.SetEquipment(Equipment_CQExtension, Index_BodyM);
	robot.SetEquipment(Equipment_CQExtension, Index_BodyM);

	robot.SetModule(Module_Sensors, Index_BodyD);

	robot.SetModule(Module_Deflectors, Index_LegR);

	robot.SetModule(Module_Gadget, Index_LegL);
	robot.SetEquipment(Equipment_GadgetJammer, Index_LegL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootL);

	robot.SetEquipment(Equipment_LightPlate, Index_FootR);

	robot.SetModule(Module_Weapon, Index_ForearmL);
	robot.SetEquipment(Equipment_HeavyPlate, Index_ForearmL);

	robot.SetModule(Module_Weapon, Index_ShoulderL);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ShoulderL);

	robot.SetModule(Module_Weapon, Index_ShoulderR);
	robot.SetEquipment(Equipment_HeavyPlate, Index_ShoulderR);

	robot.SetModule(Module_Weapon, Index_ForearmR);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ForearmR);

	robot.SetCrewMember(Crew_Pilot, Index_Head);
	robot.SetCrewMember(Crew_Medic, Index_BodyU);
	robot.SetCrewMember(Crew_Scientist, Index_BodyU);
	robot.SetCrewMember(Crew_Gunner, Index_ShoulderR);
	robot.SetCrewMember(Crew_Warrior, Index_ForearmR);
	robot.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot.SetCrewMember(Crew_Engineer, Index_LegL);

	robot.SetWeapon(Weapon_Gun, Index_ShoulderR);
	robot.SetWeapon(Weapon_Hammer, Index_ForearmR);

	robot.Initialize();
}

/*
void InGameState::InitTable()
{
	(*CurrentGame).m_hovered_slot = NULL;
	(*CurrentGame).m_selected_slot = NULL;
	(*CurrentGame).m_play_card_slot = NULL;
	(*CurrentGame).m_target_slot = NULL;
	m_current_player = 0;

	//Altar
	m_altar_slot.m_stack = Stack_Altar;

	m_altar_slot.m_shape_container.setPosition(sf::Vector2f(800 + ((CARD_WIDTH + 20) * NB_CARDS_ALTAR) * 0.5, 500));
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
}
*/
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

	//ROBOT
	//m_robots[0].Update();
}

void InGameState::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//Draw
	(*CurrentGame).drawScene();//background

	//ROBOT

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

//ROBOT
/*
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

*/