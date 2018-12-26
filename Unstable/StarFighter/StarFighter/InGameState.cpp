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
	robot.m_index = 0;
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

	robot.SetModule(Module_Weapon, Index_HandL);
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_HandL);

	robot.SetModule(Module_Weapon, Index_HandR);
	robot.SetEquipment(Equipment_LightPlate, Index_HandR);

	robot.SetModule(Module_Radar, Index_ShoulderL);
	
	robot.SetModule(Module_Weapon, Index_ShoulderR);
	robot.SetEquipment(Equipment_WeaponsScope, Index_ShoulderR);

	robot.SetCrewMember(Crew_Captain, Index_Head);
	robot.SetCrewMember(Crew_Pilot, Index_Head);
	robot.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot.SetCrewMember(Crew_Scientist, Index_LegR);
	robot.SetCrewMember(Crew_Engineer, Index_LegL);
	robot.SetCrewMember(Crew_Warrior, Index_HandL);

	robot.SetWeapon(Weapon_FireSword, Index_HandL);

	robot.Initialize();

	Robot& robot2 = m_robots[1];

	//Robot 2
	robot2.m_index = 1;
	robot2.SetModule(Module_Generator, Index_BodyU);
	robot2.SetEquipment(Equipment_GeneratorBooster, Index_BodyU);
	robot2.SetEquipment(Equipment_LightPlate, Index_BodyU);

	robot2.SetModule(Module_CrewQuarter, Index_BodyM);
	robot2.SetEquipment(Equipment_CQExtension, Index_BodyM);
	robot2.SetEquipment(Equipment_CQExtension, Index_BodyM);

	robot2.SetModule(Module_Sensors, Index_BodyD);

	robot2.SetModule(Module_Deflectors, Index_LegR);

	robot2.SetModule(Module_Gadget, Index_LegL);
	robot2.SetEquipment(Equipment_GadgetJammer, Index_LegL);

	robot2.SetEquipment(Equipment_LightPlate, Index_FootL);

	robot2.SetEquipment(Equipment_LightPlate, Index_FootR);

	robot2.SetModule(Module_Weapon, Index_HandL);
	robot2.SetEquipment(Equipment_HeavyPlate, Index_HandL);

	robot2.SetModule(Module_Weapon, Index_ShoulderL);
	robot2.SetEquipment(Equipment_EnergeticWeapon, Index_ShoulderL);

	robot2.SetModule(Module_Weapon, Index_ShoulderR);
	robot2.SetEquipment(Equipment_HeavyPlate, Index_ShoulderR);

	robot2.SetModule(Module_Weapon, Index_HandR);
	robot2.SetEquipment(Equipment_EnergeticWeapon, Index_HandR);

	robot2.SetCrewMember(Crew_Captain, Index_Head);
	robot2.SetCrewMember(Crew_Pilot, Index_Head);
	robot2.SetCrewMember(Crew_Medic, Index_BodyU);
	robot2.SetCrewMember(Crew_Scientist, Index_BodyU);
	robot2.SetCrewMember(Crew_Gunner, Index_ShoulderR);
	robot2.SetCrewMember(Crew_Warrior, Index_HandR);
	robot2.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot2.SetCrewMember(Crew_Engineer, Index_LegL);

	robot2.SetWeapon(Weapon_Gun, Index_ShoulderR);
	robot2.SetWeapon(Weapon_Hammer, Index_HandR);

	robot2.Initialize();

	//Opposition
	robot.m_opponent = &robot2;

	robot2.m_opponent = &robot;
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

void InGameState::UI_GetAction(sf::Time deltaTime)
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

	//Reset UI flags
	(*CurrentGame).m_hovered_ui = NULL;
	(*CurrentGame).m_target_ui = NULL;
	(*CurrentGame).m_play_ui = NULL;
	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		(*CurrentGame).m_selected_ui = NULL;
	}

	//Get UI action
	for (int r = 0; r < 2; r++)
	{
		//slots
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_slots.begin(); it != m_robots[r].m_UI_slots.end(); it++)
		{
			it->Update((*CurrentGame).m_mouse_click, r);
		}
		//modules
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_modules.begin(); it != m_robots[r].m_UI_modules.end(); it++)
		{
			it->Update((*CurrentGame).m_mouse_click,r );
		}

		//crew members
		int c = 0;
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_crew.begin(); it != m_robots[r].m_UI_crew.end(); it++)
		{
			it->Update((*CurrentGame).m_mouse_click, r);
			UI_SyncSml(c, r);//sync small portraits with the big ones
			c++;
		}
	}

	//Do action
	if ((*CurrentGame).m_play_ui != NULL)
	{
		CrewMember* crew_member = (CrewMember*)(*CurrentGame).m_play_ui->m_parent;
		Module* module = (Module*)(*CurrentGame).m_target_ui->m_parent;
		RobotSlot* robot_slot = module->m_owner;
		m_robots[0].MoveCrewMemberToSlot(crew_member, robot_slot);
	}
}

void InGameState::UI_SyncSml(int crew_index, int robot_index)
{
	UI_Element& ui = m_robots[robot_index].m_UI_crew[crew_index];
	UI_Element& ui_sml = m_robots[robot_index].m_UI_crew_sml[crew_index];

	ui_sml.m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 255));

	if (ui.m_hovered)
	{
		ui_sml.m_shape_container.setOutlineColor(sf::Color(255, 0, 0, 255));
	}
	if (ui.m_selected)
	{
		ui_sml.m_shape_container.setOutlineColor(sf::Color(0, 255, 0, 255));
	}
}

void UI_Element::Update(MouseAction mouse_click, int robot_index)
{
	if ((*CurrentGame).m_window_has_focus == true &&
		(*CurrentGame).m_mouse_pos.x > m_shape_container.getPosition().x - m_shape_container.getSize().x / 2 && (*CurrentGame).m_mouse_pos.x < m_shape_container.getPosition().x + m_shape_container.getSize().x / 2
		&& (*CurrentGame).m_mouse_pos.y > m_shape_container.getPosition().y - m_shape_container.getSize().y / 2 && (*CurrentGame).m_mouse_pos.y < m_shape_container.getPosition().y + m_shape_container.getSize().y / 2)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_ui = this;
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
			(*CurrentGame).m_selected_ui = this;
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
	//MOVE CREW
	if (m_hovered && mouse_click == Mouse_RightClick && (*CurrentGame).m_selected_ui && (*CurrentGame).m_selected_ui->m_team == (TeamAlliances)robot_index)
	{
		if ((*CurrentGame).m_selected_ui->m_type == UI_Crew && (*CurrentGame).m_hovered_ui->m_type == UI_Module && (*CurrentGame).m_hovered_ui->m_team == Alliance1)
		{
			(*CurrentGame).m_play_ui = (*CurrentGame).m_selected_ui;
			(*CurrentGame).m_target_ui = this;
		}
	}
}

void InGameState::Update(sf::Time deltaTime)
{
	//Phase shift
	if (m_robots[0].m_ready_to_change_phase && m_robots[1].m_ready_to_change_phase)
	{
		(*CurrentGame).m_phase = (GamePhase)(((*CurrentGame).m_phase + 1) % NB_GAME_PHASES);
		if ((int)(*CurrentGame).m_phase == 0)
		{
			(*CurrentGame).m_turn++;
		}

		m_robots[0].m_ready_to_change_phase = false;
		m_robots[1].m_ready_to_change_phase = false;
	}

	//Get action from UI
	UI_GetAction(deltaTime);

	//ROBOT
	m_robots[0].Update();
	m_robots[1].Update();

	m_robots[0].UpdateUI();
	m_robots[1].UpdateUI();

	if ((*CurrentGame).m_phase == Phase_GrabResolution)
	{
		GrabResolution();
		m_robots[0].m_ready_to_change_phase = true;
		m_robots[1].m_ready_to_change_phase = true;
	}
	else if ((*CurrentGame).m_phase == Phase_GuardResolution)
	{
		GuardResolution();
		m_robots[0].m_ready_to_change_phase = true;
		m_robots[1].m_ready_to_change_phase = true;
	}
	if ((*CurrentGame).m_phase >= Phase_AttackResolution_12 && (*CurrentGame).m_phase <= Phase_AttackResolution_1)
	{
		AttackResolution();
		m_robots[0].m_ready_to_change_phase = true;
		m_robots[1].m_ready_to_change_phase = true;
	}
}

void InGameState::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//Draw
	(*CurrentGame).drawScene();//background

	//ROBOT
	for (int r = 0; r < 2; r++)
	{
		//slots
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_slots.begin(); it != m_robots[r].m_UI_slots.end(); it++)
		{
			//visible by this player?
			if (r == 0 || it->m_team == AllianceNeutral)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}

		//modules
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_modules.begin(); it != m_robots[r].m_UI_modules.end(); it++)
		{
			//visible by this player?
			if (r == 0 || it->m_team == AllianceNeutral)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}

		//crew members
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_crew.begin(); it != m_robots[r].m_UI_crew.end(); it++)
		{
			//visible by this player?
			if (r == 0 || it->m_team == AllianceNeutral)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_crew_sml.begin(); it != m_robots[r].m_UI_crew_sml.end(); it++)
		{
			//visible by this player?
			if (r == 0 || it->m_team == AllianceNeutral)
			{
				it->Draw((*CurrentGame).m_mainScreen);
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

//ROBOT
/*
		m_hand_slots[i].m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_hand_slots[i].m_text.setCharacterSize(18);
		m_hand_slots[i].m_text.setColor(sf::Color(0, 0, 0, 255));
		m_hand_slots[i].m_text.setPosition(sf::Vector2f(200 + CARD_WIDTH / 2 + (CARD_WIDTH + 20) * i + player_index * 700, 900));
		m_hand_slots[i].m_text.setString("");
*/

bool InGameState::Execution(Robot* attacker)
{
	if (attacker->m_unbalanced_counter == 0 && attacker->m_shutdown_global == false)
	{
		ActionAttack execution = attacker->GetExecutionAttack();
		if (execution.m_attack != NULL)
		{
			bool range_weapon_used = false;
			bool triggers_execution = false;
			printf("Launching execution attack.\n");
			ResolveAttack(execution.m_attack, execution.m_target_index, true, false, range_weapon_used, triggers_execution);

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool InGameState::CounterAttack(Robot* attacker)
{
	if (attacker->m_unbalanced_counter == 0 && attacker->m_shutdown_global == false)
	{
		ActionAttack counter_attack = attacker->GetCounterAttack();
		if (counter_attack.m_attack != NULL)
		{
			bool range_weapon_used = false;
			bool triggers_execution = false;
			printf("Launching counter-attack.\n");
			ResolveAttack(counter_attack.m_attack, counter_attack.m_target_index, false, true, range_weapon_used, triggers_execution);

			//Execution?
			if (triggers_execution)
			{
				Execution(attacker);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool InGameState::ResolveAttack(WeaponAttack* attack, SlotIndex target_index, bool is_execution, bool is_counter_attack, bool &range_weapon_used, bool &triggers_execution)
{
	Robot* robot = attack->m_owner->m_owner->m_owner;
	RobotSlot& robot_slot = *attack->m_owner->m_owner;
	Module* module = attack->m_owner->m_owner->m_module;

	Weapon* weapon = attack->m_owner;

	Robot* opponent = &m_robots[robot->m_index + 1 % 2];
	RobotSlot& target_slot = opponent->m_slots[target_index];
	Module* target_module = target_slot.m_module;

	if (module->m_health == 0)
	{
		printf("Module cannot be used because it's been destroyed.\n");
		return false;
	}
	else if (module->m_shutdown_counter > 0)
	{
		printf("Module cannot be used because it's been shut down.\n");
		return false;
	}
	else if (attack->m_energy_cells < attack->m_energy_cost)
	{
		printf("Attack cancelled: not enough Energy Cells left in weapon (available: %d; cost: %d).\n", attack->m_energy_cells, attack->m_energy_cost);
		return false;
	}
	else if (attack->m_crew_required != NB_CREW_TYPES && robot_slot.HasCrewRequired(attack->m_crew_required) == false)
	{
		printf("Attack cancelled: the attack requires a specific type of crew member in the module (currently not present or stunned).\n");
		return false;
	}
	else if (robot->m_unbalanced_counter > 0)
	{
		printf("Module cannot be used because robot is unbalanced.\n");
		return false;
	}
	else if (robot->m_shutdown_global == true)
	{
		printf("Module cannot be used because robot is undergoing a global shutdown.\n");
		return false;
	}
	else//Attack resolution
	{
		//Guard perfect?
		if (opponent->m_guard_speed == attack->m_speed)
		{
			//Counter attack
			printf("Perfect guard (%d). Possible counter-attack.\n", attack->m_speed);
			CounterAttack(robot);
		}
		else
		{
			//Guard redirection
			if (attack->m_speed <= opponent->m_guard_speed)
			{
				Module* shield = opponent->GetShield();
				if (shield != NULL)
				{
					target_index = shield->m_owner->m_index;
				}
				else if (target_index == Index_FootL || target_index == Index_LegL || target_index == Index_ShoulderL)
				{
					target_index = Index_HandL;
					printf("Guard redirects attack from side to hand.\n");
				}
				else if (target_index == Index_FootR || target_index == Index_LegR || target_index == Index_ShoulderR)
				{
					target_index = Index_HandR;
					printf("Guard redirects attack from side to hand.\n");
				}
				else if (target_index == Index_BodyU || target_index == Index_BodyM || target_index == Index_BodyD)
				{
					target_index = Index_HandR;

					//todo : choose hand

					printf("Guard redirects attack from torso to hand.\n");
				}
			}

			//if there are both a ranged and a close-combat weapon on this attack speed, the close-combat is prioritary to determine the position at the end of the turn
			if (weapon->m_ranged == false)
			{
				range_weapon_used = false;
			}

			//Randomization of resolutions
			int gunner_bonus = robot_slot.GetGunnerRangeBonus();
			int equipment_bonus = robot_slot.GetEquipmentRangeBonus();
			bool hit_success = weapon->m_ranged == false || is_execution == true || opponent->m_grabbed != NULL ||
				RandomizeIntBetweenValues(1, 6) >= attack->m_chance_of_hit + gunner_bonus + equipment_bonus + (target_slot.m_type == Slot_Head ? 1 : 0);

			bool fire_success = attack->m_chance_of_fire > 0 && RandomizeIntBetweenValues(1, 6) >= attack->m_chance_of_fire;

			bool electricity_sucess = attack->m_chance_of_electricity > 0 && RandomizeIntBetweenValues(1, 6) >= attack->m_chance_of_electricity;

			int warrior_bonus = robot_slot.GetWarriorBalanceBonus();
			int counter_attack_bonus = is_counter_attack ? 5 : 0;
			int unbalanced_score = attack->m_chance_of_unbalance + warrior_bonus + counter_attack_bonus + RandomizeIntBetweenValues(1, 20) - opponent->GetBalanceScore();

			//Hit resolution
			if (hit_success)
			{
				int damage = attack->m_damage;
				//Damage on grounded robot x2
				if (opponent->m_grounded == true || is_execution == true || opponent->m_grabbed != NULL)
				{
					damage *= 2;
				}

				bool module_damaged = false;

				//Energetical damage
				if (weapon->m_energetic)
				{
					//Deflector absorption
					for (vector<RobotSlot>::iterator it = opponent->m_slots.begin(); it != opponent->m_slots.end(); it++)
					{
						if (it->m_module->m_type == Module_Deflectors && it->m_module->IsOperationnal())
						{
							Module* deflector = it->m_module;
							if (deflector->m_health > 0)
							{
								//Damage absorbed
								if (deflector->m_health >= damage)
								{
									deflector->m_health -= damage;
									damage = 0;
								}
								else
								{
									deflector->m_health = 0;
									damage -= deflector->m_health;
								}
							}
						}
					}

					//Damage to target module
					if (damage > 0)
					{
						if (target_module != NULL)
						{
							module_damaged = true;
							if (target_module->m_health > damage)
							{
								target_module->m_health -= damage;
							}
							else
							{
								target_module->m_health = 0;
								opponent->DestroySlot(target_slot.m_index);
							}

							//Damage to robot
							if (opponent->m_health > damage)
							{
								opponent->m_health -= damage;
							}
							else
							{
								opponent->m_health = 0;
								printf("Robot %d's health reached 0 and is destroyed. GAME OVER.\n", opponent->m_index);
							}
						}
					}
				}
				else //Physical damage
				{
					if (target_module != NULL)
					{
						//Plate absorption (equipement of the target module)
						for (vector<Equipment*>::iterator it = target_slot.m_equipments.begin(); it != target_slot.m_equipments.end(); it++)
						{
							if ((*it)->m_type == Equipment_LightPlate || (*it)->m_type == Equipment_HeavyPlate)
							{
								if ((*it)->m_health > 0)
								{
									//Damage absorbed by plates
									if ((*it)->m_health >= damage)
									{
										(*it)->m_health -= damage;
										damage = 0;
									}
									else
									{
										(*it)->m_health = 0;
										damage -= (*it)->m_health;
									}
								}
							}
						}

						//Damage to module
						if (damage > 0)
						{
							module_damaged = true;
							if (target_module->m_health > damage)
							{
								target_module->m_health -= damage;
							}
							else
							{
								target_module->m_health = 0;
								opponent->DestroySlot(target_slot.m_index);
							}

							//Damage to robot
							if (opponent->m_health > damage)
							{
								opponent->m_health -= damage;
							}
							else
							{
								opponent->m_health = 0;
								printf("Robot %d's health reached 0 and is destroyed. GAME OVER.\n", opponent->m_index);
							}
						}
					}
				}

				//Randomization of damage to crew if module has been damaged
				if (module_damaged)
				{
					for (vector<CrewMember*>::iterator it = target_slot.m_crew.begin(); it != target_slot.m_crew.end(); it++)
					{
						bool hit_roll = RandomizeIntBetweenValues(1, 6) >= 4;
						if (hit_roll == true)
						{
							(*it)->m_health--;
						}
					}

					opponent->UpdateCrew(target_slot.m_index);
				}
			}

			//Fire resolution
			if (fire_success && target_module != NULL)
			{
				if (target_module->m_fire_counter < 2)
				{
					printf("Fire started.\n");
					target_module->m_fire_counter = 2;
				}
			}

			//Electric resolution
			if (electricity_sucess && target_module != NULL)
			{
				opponent->ShutdownSlot(target_slot.m_index);
			}

			//Stun resolution
			for (vector<CrewMember*>::iterator it = target_slot.m_crew.begin(); it != target_slot.m_crew.end(); it++)
			{
				bool stun_roll = attack->m_chance_of_stun > 0 && RandomizeIntBetweenValues(1, 6) >= attack->m_chance_of_stun;
				if (stun_roll == true)
				{
					if ((*it)->m_stun_counter < 2)
					{
						(*it)->m_stun_counter = 2;
						printf("Crew member stunned.\n");
					}
				}
			}

			//Balance resolution
			if (unbalanced_score > 0)
			{
				if (opponent->m_unbalanced_counter == 0)
				{
					opponent->m_unbalanced_counter = 2;
					opponent->m_unbalanced_value = unbalanced_score;
					printf("Robot %d gets unbalanced.\n", opponent->m_index);
				}
				else
				{
					opponent->m_unbalanced_counter = 2;
					opponent->m_unbalanced_value = unbalanced_score;

					if (opponent->m_grounded == false)
					{
						opponent->m_grounded = true;
						triggers_execution = true;
						printf("Robot %d gets unbalanced again and gets grounded. Execution move possible.\n", opponent->m_index);
					}
				}
			}
		}

		//Check global shutdown conditions
		opponent->UpdateShudownGlobal();

		//Distance update
		(*CurrentGame).m_distance_temp = range_weapon_used ? Distance_Ranged : Distance_Close;

		return true;
	}
}

void InGameState::AttackResolution()
{
	int speed = (int)(Phase_AttackResolution_1) - (int)(*CurrentGame).m_phase + 1;
	bool range_weapon_used = true;

	//Resolve attacks
	for (vector<ActionAttack>::iterator it = (*CurrentGame).m_attacks_list.begin(); it != (*CurrentGame).m_attacks_list.end(); it++)
	{
		if (it->m_attack->m_speed != speed)
		{
			continue;
		}

		printf("Attack resolution (speed %d).\n", speed);
		bool triggers_execution = false;
		ResolveAttack(it->m_attack, it->m_target_index, false, false, range_weapon_used, triggers_execution);

		//Execution?
		if (triggers_execution)
		{
			Execution(it->m_attack->m_owner->m_owner->m_owner);
		}
	}

	//End of the Attack resolution phases
	if ((*CurrentGame).m_phase == Phase_AttackResolution_1)
	{
		//Updating placement (depends on the last attack of the turn)
		(*CurrentGame).m_distance = (m_robots[0].m_grabbed != NULL || m_robots[1].m_grabbed != NULL) ? Distance_Close : (*CurrentGame).m_distance_temp;

		//Clearing the actions list
		(*CurrentGame).m_attacks_list.clear();
	}

	m_robots[0].m_ready_to_change_phase = true;
	m_robots[1].m_ready_to_change_phase = true;
}

void InGameState::EffectsResolution()
{
	//Resolve effects of robot 0, then robot 1
	for (int r = 0; r < 2; r++)
	{
		for (vector<ActionEffect>::iterator it = (*CurrentGame).m_effects_list.begin(); it != (*CurrentGame).m_effects_list.end(); it++)
		{
			if (it->m_effect->m_owner_equipment != NULL && it->m_effect->m_owner_equipment->m_owner->m_owner->m_index != r)
			{
				continue;
			}
			if (it->m_effect->m_owner_module != NULL && it->m_effect->m_owner_module->m_owner->m_owner->m_index != r)
			{
				continue;
			}

			printf("Effect resolution.\n");

			Robot* robot = it->m_effect->m_owner_module != NULL ? it->m_effect->m_owner_module->m_owner->m_owner : it->m_effect->m_owner_equipment->m_owner->m_owner;
			RobotSlot& robot_slot = it->m_effect->m_owner_module != NULL ? *it->m_effect->m_owner_module->m_owner : *it->m_effect->m_owner_equipment->m_owner;

			Module* module = it->m_effect->m_owner_module != NULL ? it->m_effect->m_owner_module : NULL;
			Equipment* equipment = it->m_effect->m_owner_equipment != NULL ? it->m_effect->m_owner_equipment : NULL;
			EquipmentEffect* effect = it->m_effect;

			Robot* opponent = &m_robots[robot->m_index + 1 % 2];
			RobotSlot& target_slot = opponent->m_slots[it->m_target_index];
			Module* target_module = target_slot.m_module;

			switch (effect->m_type)
			{
				case Effect_EMP:
				{
					opponent->m_energy_cells -= target_module->m_energy_cells;
					target_module->m_energy_cells = 0;
					break;
				}
				case Effect_Radar:
				{
					target_slot.m_is_revealed = true;
					break;
				}
				case Effect_JetPack:
				{
					break;
				}
			}
		}
	}

	m_robots[0].m_ready_to_change_phase = true;
	m_robots[1].m_ready_to_change_phase = true;
}

void InGameState::GrabResolution()
{
	vector<ActionAttack> old_attacks_list;
	for (vector<ActionAttack>::iterator it = (*CurrentGame).m_attacks_list.begin(); it != (*CurrentGame).m_attacks_list.end(); it++)
	{
		old_attacks_list.push_back(*it);
	}

	(*CurrentGame).m_attacks_list.clear();

	//Resolve grabs of robot 0, then robot 1
	for (int r = 0; r < 2; r++)
	{
		for (vector<ActionAttack>::iterator it = old_attacks_list.begin(); it != old_attacks_list.end(); it++)
		{
			if (it->m_attack->m_owner->m_owner->m_owner->m_index != r)
			{
				continue;
			}

			if (it->m_attack->m_type != WeaponAttack_Grab_1)
			{
				(*CurrentGame).m_attacks_list.push_back(*it);
				continue;
			}

			Robot* robot = it->m_attack->m_owner->m_owner->m_owner;
			Module* module = it->m_attack->m_owner->m_owner->m_module;

			Robot* opponent = &m_robots[robot->m_index + 1 % 2];
			Module* target_module = opponent->m_slots[it->m_target_index].m_module;

			//Resolve grab
			bool hit_success = opponent->m_shutdown_global == true || (target_module != NULL && (target_module->m_shutdown_counter > 0 || target_module->m_health == 0));

			if (hit_success)
			{
				//Grab and record which Hand did the grab, because only destroying that hand will release the grab
				opponent->m_grabbed = module;
				printf("Grab successful.\n");
			}
			else
			{
				//Counter-attack
				printf("Grab missed. Possible counter-attack.\n");
				CounterAttack(opponent);
			}
		}
	}
}

void InGameState::GuardResolution()
{
	vector<ActionAttack> old_attacks_list;
	for (vector<ActionAttack>::iterator it = (*CurrentGame).m_attacks_list.begin(); it != (*CurrentGame).m_attacks_list.end(); it++)
	{
		old_attacks_list.push_back(*it);
	}

	(*CurrentGame).m_attacks_list.clear();

	//Resolve grabs of robot 0, then robot 1
	for (int r = 0; r < 2; r++)
	{
		//Check if there is a Guard in the list
		bool guard_found = false;
		for (vector<ActionAttack>::iterator it = old_attacks_list.begin(); it != old_attacks_list.end(); it++)
		{
			if (it->m_attack->m_owner->m_owner->m_owner->m_index != r)
			{
				continue;
			}

			if (it->m_attack->m_type != WeaponAttack_Guard_1)
			{
				continue;
			}

			guard_found = true;
			break;
		}

		//If yes, keep it and clear all other attacks from this robot, else do nothing and just keep the whole list
		for (vector<ActionAttack>::iterator it = old_attacks_list.begin(); it != old_attacks_list.end(); it++)
		{
			if (it->m_attack->m_owner->m_owner->m_owner->m_index != r)
			{
				continue;
			}

			if (guard_found == false)
			{
				(*CurrentGame).m_attacks_list.push_back(*it);
			}
			else
			{
				//Discard all attacks that are not the Guard (from this robot)
				if (it->m_attack->m_type != WeaponAttack_Guard_1)
				{
					continue;
				}

				//Resolve the Guard
				Robot* robot = it->m_attack->m_owner->m_owner->m_owner;
				RobotSlot& robot_slot = *it->m_attack->m_owner->m_owner;
				Module* module = it->m_attack->m_owner->m_owner->m_module;

				Weapon* weapon = it->m_attack->m_owner;

				Robot* opponent = &m_robots[robot->m_index + 1 % 2];
				RobotSlot& target_slot = opponent->m_slots[it->m_target_index];
				Module* target_module = target_slot.m_module;

				//Get Guard speed
				printf("Choose a Guard speed between 1 and 10.\n");
				int speed = 3;//get guard speed here
				if (speed < 1 || speed > 10)
				{
					printf("Guard speed must be between 1 and 10. Choose again.\n");
				}
				
				robot->m_guard_speed = speed;
			}
		}	
	}
}