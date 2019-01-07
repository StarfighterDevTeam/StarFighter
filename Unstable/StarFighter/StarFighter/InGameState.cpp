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
	InitUI();
}

void InGameState::InitUI()
{
	//Turn display
	m_UI_turn.setPosition(sf::Vector2f(1920.f * 0.5, 50.f));
	m_UI_turn.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_UI_turn.setCharacterSize(30);
	m_UI_turn.setColor(sf::Color(255, 255, 255, 255));

	//Phase display
	m_UI_phase.setPosition(sf::Vector2f(1920.f * 0.5, 100.f));
	m_UI_phase.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_UI_phase.setCharacterSize(30);
	m_UI_phase.setColor(sf::Color(255, 255, 255, 255));
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
	robot.SetEquipment(Equipment_EnergeticWeapon, Index_ShoulderR);

	robot.SetCrewMember(Crew_Captain, Index_Head);
	robot.SetCrewMember(Crew_Pilot, Index_Head);
	robot.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot.SetCrewMember(Crew_Scientist, Index_LegR);
	robot.SetCrewMember(Crew_Engineer, Index_LegL);
	robot.SetCrewMember(Crew_Warrior, Index_HandL);

	robot.SetWeapon(Weapon_FireSword, Index_HandL);
	robot.SetWeapon(Weapon_Fist, Index_HandR);
	robot.SetWeapon(Weapon_BlasterRifle, Index_ShoulderR);

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

	robot2.SetModule(Module_Radar, Index_BodyD);

	robot2.SetModule(Module_Deflectors, Index_LegR);

	robot2.SetModule(Module_Gadget, Index_LegL);
	robot2.SetEquipment(Equipment_GadgetEMP, Index_LegL);

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
	robot2.SetCrewMember(Crew_Engineer, Index_LegL);
	robot2.SetCrewMember(Crew_Scientist, Index_BodyU);
	robot2.SetCrewMember(Crew_Gunner, Index_ShoulderR);
	robot2.SetCrewMember(Crew_Mechanic, Index_BodyM);
	robot2.SetCrewMember(Crew_Medic, Index_BodyU);
	robot2.SetCrewMember(Crew_Warrior, Index_HandR);

	robot2.SetWeapon(Weapon_LaserCannon, Index_ShoulderL);
	robot2.SetWeapon(Weapon_Gun, Index_ShoulderR);
	robot2.SetWeapon(Weapon_StunFist, Index_HandR);
	robot2.SetWeapon(Weapon_Hammer, Index_HandL);

	robot2.Initialize();

	//Opposition
	robot.m_opponent = &robot2;
	robot2.m_opponent = &robot;
}

void InGameState::UI_GetAction(int robot_index)
{
	//Get UI action
	int r1 = robot_index;
	int r2 = (r1 + 1) % 2;

	//Reset UI flags
	(*CurrentGame).m_hovered_ui = NULL;
	(*CurrentGame).m_target_ui = NULL;
	(*CurrentGame).m_play_ui = NULL;
	UI_Element* previous_selected_ui = (*CurrentGame).m_selected_ui;
	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick && (*CurrentGame).m_window_has_focus == true)
	{
		(*CurrentGame).m_selected_ui = NULL;
	}

	//Update buttons
	//slots
	for (vector<UI_Element>::iterator it = m_robots[r2].m_UI_slots.begin(); it != m_robots[r2].m_UI_slots.end(); it++)
	{
		RobotSlot* slot = (RobotSlot*)it->m_parent;
		if (slot != NULL && slot->m_module != NULL)
		{
			it->Update((*CurrentGame).m_mouse_click, robot_index);
		}
	}

	//modules
	for (vector<vector<UI_Element> >::iterator it = m_robots[r1].m_UI_modules.begin(); it != m_robots[r1].m_UI_modules.end(); it++)
	{
		for (vector<UI_Element>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			if (it2->m_parent != NULL)
			{
				it2->Update((*CurrentGame).m_mouse_click, robot_index);
			}
		}
	}

	//crew members
	int c = 0;
	for (vector<UI_Element>::iterator it = m_robots[r1].m_UI_crew.begin(); it != m_robots[r1].m_UI_crew.end(); it++)
	{
		it->Update((*CurrentGame).m_mouse_click, robot_index);
		UI_SyncSml(c, robot_index);//sync small portraits with the big ones
		c++;
	}

	//buttons
	for (vector<UI_Element>::iterator it = m_robots[r1].m_UI_buttons.begin(); it != m_robots[r1].m_UI_buttons.end(); it++)
	{
		it->Update((*CurrentGame).m_mouse_click, robot_index);
	}

	//weapon attacks
	for (vector<vector<UI_Element> >::iterator it = m_robots[r1].m_UI_focus.begin(); it != m_robots[r1].m_UI_focus.end(); it++)
	{
		for (vector<UI_Element>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			if (it2->m_type == UI_WeaponAttack)
			{
				it2->Update((*CurrentGame).m_mouse_click, robot_index);
			}
		}
	}

	//Global shutdown or grounded -> skip turn
	//if (m_robots[robot_index].m_shutdown_global == true && (*CurrentGame).m_phase == Phase_AttackPlanning)
	//{
	//	m_robots[robot_index].m_ready_to_change_phase = true;
	//	(*CurrentGame).UI_AddEventLog("Cannot plan any attack because\nhe's under a global shutdown.\n", Event_Shutdown, robot_index);
	//	return;
	//}
	//else if (m_robots[robot_index].m_grounded == true && (*CurrentGame).m_phase == Phase_AttackPlanning)
	//{
	//	m_robots[robot_index].m_ready_to_change_phase = true;
	//	(*CurrentGame).UI_AddEventLog("Cannot plan any attack because\nhe's stuck on ground (x2 damage).\n", Event_Shutdown, robot_index);
	//	return;
	//}
	
	//Do action
	//Debug shortcut
	if ((*CurrentGame).m_playerShip->m_inputs_states[Action_Firing] == Input_Tap)
	{
		m_robots[r1].m_ready_to_change_phase = true;
		(*CurrentGame).m_UI_events_log.clear();
		if ((*CurrentGame).m_selected_ui != NULL)
		{
			(*CurrentGame).m_selected_ui->m_selected = false;
			(*CurrentGame).m_selected_ui = NULL;
		}
		(*CurrentGame).UI_AddEventLog("has played.", Event_Neutral, r1);
	}

	if ((*CurrentGame).m_play_ui != NULL)
	{
		//End turn
		if ((*CurrentGame).m_play_ui->m_type == UI_EndTurn)
		{
			m_robots[r1].m_ready_to_change_phase = true;
			(*CurrentGame).m_play_ui->m_hovered = false;
			if ((*CurrentGame).m_selected_ui != NULL)
			{
				(*CurrentGame).m_selected_ui->m_selected = false;
				(*CurrentGame).m_selected_ui = NULL;
			}
			
			(*CurrentGame).m_UI_events_log.clear();
			(*CurrentGame).UI_AddEventLog(" has played.", Event_Neutral, r1);
		}	 
		else if ((*CurrentGame).m_play_ui->m_type == UI_Balance)
		{
			m_robots[r1].SetEnergyCellsOnBalance();
		}
		//Energy Cells attribution
		else if ((*CurrentGame).m_play_ui->m_type == UI_EC_Slot_Module && (*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			Module* module = (Module*)(*CurrentGame).m_play_ui->m_parent;
			m_robots[r1].SetEnergyCell(module);
		}
		else if ((*CurrentGame).m_play_ui->m_type == UI_EC_Slot_Equipment && (*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			Equipment* equipment = (Equipment*)(*CurrentGame).m_play_ui->m_parent;
			m_robots[r1].SetEnergyCell(equipment);
		}
		else if ((*CurrentGame).m_play_ui->m_type == UI_EC_Slot_Module && (*CurrentGame).m_mouse_click == Mouse_RightClick)
		{
			Module* module = (Module*)(*CurrentGame).m_play_ui->m_parent;
			m_robots[r1].RemoveEnergyCell(module);
		}
		else if ((*CurrentGame).m_play_ui->m_type == UI_EC_Slot_Equipment && (*CurrentGame).m_mouse_click == Mouse_RightClick)
		{
			Equipment* equipment = (Equipment*)(*CurrentGame).m_play_ui->m_parent;
			m_robots[r1].RemoveEnergyCell(equipment);
		}
		//Move crew member
		else if ((*CurrentGame).m_play_ui->m_type == UI_Crew && (*CurrentGame).m_target_ui != NULL)
		{
			CrewMember* crew_member = (CrewMember*)(*CurrentGame).m_play_ui->m_parent;
			RobotSlot* robot_slot = NULL;
			if ((*CurrentGame).m_target_ui->m_type == UI_Module)
			{
				Module* module = (Module*)(*CurrentGame).m_target_ui->m_parent;
				robot_slot = module != NULL ? module->m_owner : NULL;

				m_robots[r1].MoveCrewMemberToSlot(crew_member, robot_slot);
			}
			else if ((*CurrentGame).m_target_ui->m_type == UI_Equipment)
			{
				Equipment* equipment = (Equipment*)(*CurrentGame).m_target_ui->m_parent;
				robot_slot = equipment != NULL ? equipment->m_owner : NULL;

				m_robots[r1].MoveCrewMemberToSlot(crew_member, robot_slot);
			}
			//[Medic] Heal crew member
			else if ((*CurrentGame).m_target_ui->m_type == UI_Crew)
			{
				CrewMember* medic = (CrewMember*)(*CurrentGame).m_play_ui->m_parent;
				CrewMember* target = (CrewMember*)(*CurrentGame).m_target_ui->m_parent;

				m_robots[r1].SetMedicTarget(medic, target);
			}
		}
		//Weapon attack target selection
		else if ((*CurrentGame).m_target_ui != NULL && (*CurrentGame).m_target_ui->m_type == UI_Slot)
		{
			RobotSlot* target_slot = (RobotSlot*)(*CurrentGame).m_target_ui->m_parent;
			if ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui->m_type == UI_Module)
			{
				Module* module = (Module*)(*CurrentGame).m_selected_ui->m_parent;

				if (module != NULL && target_slot != NULL)
				{
					//Weapon attack
					if (module->m_owner->m_weapon != NULL)
					{
						WeaponAttack* attack = module->m_owner->m_weapon->m_selected_attack;
						SlotIndex target_index = target_slot->m_index;
						m_robots[r1].SetWeaponAttackOnSlot(attack, target_index);

						if (attack->m_nb_targets_remaining == 0)
						{
							(*CurrentGame).m_selected_ui->m_selected = false;
							(*CurrentGame).m_selected_ui = NULL;
						}
					}
					//Active ability
					else if (module->m_effect != NULL)
					{
						EquipmentEffect* effect = module->m_effect;
						SlotIndex target_index = target_slot->m_index;
						m_robots[r1].SetEquipmentEffectOnSlot(effect, target_index);

						if (module->m_used == true)
						{
							(*CurrentGame).m_selected_ui->m_selected = false;
							(*CurrentGame).m_selected_ui = NULL;
						}
					}
				}
			}
			else if ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui->m_type == UI_Equipment)
			{
				Equipment* equipment = (Equipment*)(*CurrentGame).m_selected_ui->m_parent;
				if (equipment != NULL && target_slot != NULL)
				{
					if (equipment->m_effect != NULL)
					{
						EquipmentEffect* effect = equipment->m_effect;
						SlotIndex target_index = target_slot->m_index;
						m_robots[r1].SetEquipmentEffectOnSlot(effect, target_index);

						if (equipment->m_used == true)
						{
							(*CurrentGame).m_selected_ui->m_selected = false;
							(*CurrentGame).m_selected_ui = NULL;
						}
					}
				}
			}
		}
		//Weapon attack selection
		else if ((*CurrentGame).m_play_ui->m_type == UI_WeaponAttack)
		{
			WeaponAttack* attack = (WeaponAttack*)(*CurrentGame).m_play_ui->m_parent;
			bool weapon_not_used = true;
			Weapon* weapon = attack->m_owner;
			for (vector<WeaponAttack*>::iterator it = weapon->m_attacks.begin(); it != weapon->m_attacks.end(); it++)
			{
				if ((*it)->m_nb_targets_remaining < (*it)->m_nb_targets && (*it)->m_nb_targets > 0)
				{
					weapon_not_used = false;
				}
			}

			if (weapon_not_used == true)
			{
				attack->m_owner->m_selected_attack = attack;
			}
			else
			{
				(*CurrentGame).UI_AddEventLog("This weapon already attacked during this turn.", Event_Error, r1);
			}

			//give selection back to owner weapon
			(*CurrentGame).m_selected_ui = previous_selected_ui;
			previous_selected_ui->m_selected = true;
		}
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

void InGameState::Update(sf::Time deltaTime)
{
	//Get mouse & keyboard inputs
	(*CurrentGame).GetMouseInputs(deltaTime);
	(*CurrentGame).m_playerShip->update(deltaTime);

	//Phase shift
	if (m_robots[0].m_ready_to_change_phase == true && m_robots[1].m_ready_to_change_phase == true)
	{
		(*CurrentGame).m_phase = (GamePhase)(((*CurrentGame).m_phase + 1) % NB_GAME_PHASES);
		if ((int)(*CurrentGame).m_phase == 0)
		{
			(*CurrentGame).m_turn++;
		}

		m_robots[0].m_ready_to_change_phase = false;
		m_robots[1].m_ready_to_change_phase = false;
	}

	//ROBOT
	m_robots[0].Update();
	m_robots[1].Update();

	m_robots[0].UpdateUI();
	m_robots[1].UpdateUI();

	UpdateUI(deltaTime);

	if ((*CurrentGame).m_phase == Phase_AttackPlanning)
	{
		if (m_robots[0].m_ready_to_change_phase == false)
		{
			UI_GetAction(0);
		}
		else
		{
			UI_GetAction(1);
		}
	}
	else if ((*CurrentGame).m_phase == Phase_EffectsResolution)
	{
		EffectsResolution();
		m_robots[0].m_ready_to_change_phase = true;
		m_robots[1].m_ready_to_change_phase = true;
	}
	else if ((*CurrentGame).m_phase == Phase_GrabResolution)
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
	else if ((*CurrentGame).m_phase >= Phase_AttackResolution_14 && (*CurrentGame).m_phase <= Phase_AttackResolution_1)
	{
		AttackResolution();
		if ((*CurrentGame).m_phase != Phase_Execution && (*CurrentGame).m_phase != Phase_CounterAttack)
		{
			m_robots[0].m_ready_to_change_phase = true;
			m_robots[1].m_ready_to_change_phase = true;
		}
	}
	else if ((*CurrentGame).m_phase == Phase_Execution)
	{
		int r1 = GetRobotIndexOfLastAttackResolved();
		int r2 = (r1 + 1) % 2;

		m_robots[r2].m_ready_to_change_phase = true;

		UI_GetAction(r1);

		if (m_robots[r1].m_ready_to_change_phase == true && m_robots[r2].m_ready_to_change_phase == true)
		{
			//resolve the execution if used
			if ((*CurrentGame).m_attacks_list.size() > m_nb_attacks_becore_contextual)
			{
				(*CurrentGame).UI_AddEventLog("launchs an execution (x2 damage, can't miss).", Event_Neutral, r1);
				bool a, b, c;
				ResolveAttack((*CurrentGame).m_attacks_list.back().m_attack, (*CurrentGame).m_attacks_list.back().m_target_index, true, false, a, b, c);
				(*CurrentGame).m_attacks_list.back().m_resolved = true;
			}
			else
			{
				(*CurrentGame).UI_AddEventLog("declines to launch an execution.", Event_Neutral, r1);
			}
			
			//remove all attacks already resolved from the list
			ClearAttacksListResolved();
			
			//get back to resolving the remaining attacks
			(*CurrentGame).m_phase = (GamePhase)(Phase_AttackResolution_14 - 1);
		}
	}
	else if ((*CurrentGame).m_phase == Phase_CounterAttack)
	{
		int r1 = GetRobotIndexOfLastAttackResolved();
		int r2 = (r1 + 1) % 2;

		m_robots[r2].m_ready_to_change_phase = true;

		UI_GetAction(r1);

		if (m_robots[r1].m_ready_to_change_phase == true && m_robots[r2].m_ready_to_change_phase == true)
		{
			//resolve the execution if used
			if ((*CurrentGame).m_attacks_list.size() > m_nb_attacks_becore_contextual)
			{
				(*CurrentGame).UI_AddEventLog("launchs a counter-attack.", Event_Neutral, r1);
				bool a, b, c;
				ResolveAttack((*CurrentGame).m_attacks_list.back().m_attack, (*CurrentGame).m_attacks_list.back().m_target_index, false, true, a, b, c);
				(*CurrentGame).m_attacks_list.back().m_resolved = true;
			}
			else
			{
				(*CurrentGame).UI_AddEventLog("declines to launch a counter-attack.", Event_Neutral, r1);
			}

			//remove all attacks already resolved from the list
			ClearAttacksListResolved();

			//get back to resolving the remaining attacks
			(*CurrentGame).m_phase = (GamePhase)(Phase_AttackResolution_14 - 1);
		}
	}
}

void InGameState::ClearAttacksListResolved()
{
	GamePhase phase = Phase_AttackResolution_14;

	vector<ActionAttack> old_attacks_list;
	for (vector<ActionAttack>::iterator it = (*CurrentGame).m_attacks_list.begin(); it != (*CurrentGame).m_attacks_list.end(); it++)
	{
		old_attacks_list.push_back(*it);
	}

	(*CurrentGame).m_attacks_list.clear();

	for (vector<ActionAttack>::iterator it = old_attacks_list.begin(); it != old_attacks_list.end(); it++)
	{
		if (it->m_resolved == false)
		{
			(*CurrentGame).m_attacks_list.push_back(*it);;
		}
	}
}

int InGameState::GetRobotIndexOfLastAttackResolved()
{
	int robot_index = 0;
	for (vector<ActionAttack>::iterator it = (*CurrentGame).m_attacks_list.begin(); it != (*CurrentGame).m_attacks_list.end(); it++)
	{
		if (it->m_resolved == true)
		{
			robot_index = it->m_attack->m_owner->m_owner->m_owner->m_index;
		}
		else
		{
			break;
		}
	}

	return robot_index;
}

void InGameState::UpdateUI(sf::Time deltaTime)
{
	//Turn display
	ostringstream s_turn;
	s_turn << "Turn " << (*CurrentGame).m_turn;
	m_UI_turn.setString(s_turn.str());
	m_UI_turn.setPosition(sf::Vector2f(1920.f * 0.5 - m_UI_turn.getGlobalBounds().width * 0.5f, 40.f));

	//Phase display
	ostringstream s_phase;
	s_phase << "Phase: ";
	switch ((*CurrentGame).m_phase)
	{
		case Phase_GenerateEC:
		{
			s_phase << "EC generation";
			break;
		}
		//case Phase_CrewMovement:
		//{
		//	s_phase << "Crew movement";
		//	break;
		//}
		case Phase_AttackPlanning:
		{
			s_phase << "Attack planning";
			break;
		}
		case Phase_HealCrew:
		{
			s_phase << "Crew healing";
			break;
		}
		case Phase_RepairModules:
		{
			s_phase << "Module repairing";
			break;
		}
		case Phase_EffectsResolution:
		{
			s_phase << "Effects resolution";
			break;
		}
		case Phase_GrabResolution:
		{
			s_phase << "Grab resolution";
			break;
		}
		case Phase_GuardResolution:
		{
			s_phase << "Guard resolution";
			break;
		}
		case Phase_AttackResolution_12:
		case Phase_AttackResolution_11:
		case Phase_AttackResolution_10:
		case Phase_AttackResolution_9:
		case Phase_AttackResolution_8:
		case Phase_AttackResolution_7:
		case Phase_AttackResolution_6:
		case Phase_AttackResolution_5:
		case Phase_AttackResolution_4:
		case Phase_AttackResolution_3:
		case Phase_AttackResolution_2:
		case Phase_AttackResolution_1:
		{
			s_phase << "Attacks resolution";
			break;
		}
		case Phase_CooldownResolution:
		{
			s_phase << "Cooldowns update";
			break;
		}
		case Phase_FireResolution:
		{
			s_phase << "Fire resolution";
			break;
		}
		case Phase_Execution:
		{
			s_phase << "Execution (contextual)";
			break;
		}
		case Phase_CounterAttack:
		{
			s_phase << "Counter-attack (contextual)";
			break;
		}
	}

	m_UI_phase.setString(s_phase.str());
	m_UI_phase.setPosition(sf::Vector2f(1920.f * 0.5 - m_UI_phase.getGlobalBounds().width * 0.5f, 100.f));

	//Events log update - keep only a limited number of lines (the most recents ones)
	int events_log_size = (*CurrentGame).m_UI_events_log.size();
	if (events_log_size > MAX_EVENTS_LOG_LINES)
	{
		vector<SFText> old_events_log;
		for (vector<SFText>::iterator it = (*CurrentGame).m_UI_events_log.begin(); it != (*CurrentGame).m_UI_events_log.end(); it++)
		{
			old_events_log.push_back(*it);
		}

		(*CurrentGame).m_UI_events_log.clear();

		int n = 0;
		for (vector<SFText>::iterator it = old_events_log.begin(); it != old_events_log.end(); it++)
		{
			if (n >= events_log_size - MAX_EVENTS_LOG_LINES)
			{
				(*CurrentGame).m_UI_events_log.push_back(*it);
			}
			n++;
		}
	}
	
	int l = 0;
	for (vector<SFText>::iterator it = (*CurrentGame).m_UI_events_log.begin(); it != (*CurrentGame).m_UI_events_log.end(); it++)
	{
		it->setPosition(sf::Vector2f(1920.f * 0.5f - 170.f, 180.f + (l * 40.f)));
		l++;
	}
}

void InGameState::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//Draw
	(*CurrentGame).drawScene();//background

	//ROBOT
	//Events log display
	for (vector<SFText>::iterator it = (*CurrentGame).m_UI_events_log.begin(); it != (*CurrentGame).m_UI_events_log.end(); it++)
	{
		(*CurrentGame).m_mainScreen.draw(*it);
	}

	GamePhase& phase = (*CurrentGame).m_phase;
	bool async_phase = phase == Phase_AttackPlanning || phase == Phase_Execution || phase == Phase_CounterAttack;

	int r1 = m_robots[0].m_ready_to_change_phase == true ? 1 : 0;
	int r2 = (r1 + 1) % 2;
	for (int r = 0; r < 2; r++)
	{
		//slots
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_slots.begin(); it != m_robots[r].m_UI_slots.end(); it++)
		{
			if (async_phase == false || r == r2)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}

		//modules
		//<weapon>
		//<weapon attack, cost, cost, ...>
		//<weapon attack, cost, cost, ...>
		for (vector<vector<UI_Element> >::iterator it = m_robots[r].m_UI_modules.begin(); it != m_robots[r].m_UI_modules.end(); it++)
		{
			for (vector<UI_Element>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				RobotSlot* slot = m_robots[r].GetEntityParentSlot(it2->m_parent, it2->m_type);
				if (r == r1 || (slot != NULL && slot->m_is_revealed == true))
				{
					it2->Draw((*CurrentGame).m_mainScreen);
				}
			}
		}

		//crew members
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_crew.begin(); it != m_robots[r].m_UI_crew.end(); it++)
		{
			if (async_phase == false || r == r1)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_crew_sml.begin(); it != m_robots[r].m_UI_crew_sml.end(); it++)
		{
			if (async_phase == false || r == r1)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}

		//buttons
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_buttons.begin(); it != m_robots[r].m_UI_buttons.end(); it++)
		{
			if (async_phase == false || r == r1)
			{
				if (it->m_type == UI_EndTurn)
				{
					it->Draw((*CurrentGame).m_mainScreen);
				}
				else if (it->m_type == UI_Balance && m_robots[r].m_unbalanced_counter > 0)
				{
					it->Draw((*CurrentGame).m_mainScreen);
				}
			}
		}

		//robot stats
		(*CurrentGame).m_mainScreen.draw(m_robots[r].m_UI_stats);

		//focused element details
		for (vector<vector<UI_Element> >::iterator it = m_robots[r].m_UI_focus.begin(); it != m_robots[r].m_UI_focus.end(); it++)
		{
			for (vector<UI_Element>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (async_phase == false || r == r1)
				{
					it2->Draw((*CurrentGame).m_mainScreen);
				}
			}
		}

		//EC available
		for (vector<UI_Element>::iterator it = m_robots[r].m_UI_ec_available.begin(); it != m_robots[r].m_UI_ec_available.end(); it++)
		{
			if (async_phase == false || r == r1)
			{
				it->Draw((*CurrentGame).m_mainScreen);
			}
		}
	}

	//Turn & phase display
	(*CurrentGame).m_mainScreen.draw(m_UI_turn);
	(*CurrentGame).m_mainScreen.draw(m_UI_phase);

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


bool InGameState::Execution(Robot* attacker)
{
	if (attacker->m_unbalanced_counter == 0 && attacker->m_shutdown_global == false)
	{
		if (attacker->m_index == 1)
		{
			m_robots[0].m_ready_to_change_phase = true;
		}

		UI_GetAction(attacker->m_index);

		ActionAttack execution = attacker->GetExecutionAttack();
		if (execution.m_attack != NULL)
		{
			bool range_weapon_used = false;
			bool triggers_execution = false;
			(*CurrentGame).UI_AddEventLog("is launching execution attack.", Event_Neutral, attacker->m_index);
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
*/
/*
bool InGameState::CounterAttack(Robot* attacker)
{
	if (attacker->m_unbalanced_counter == 0 && attacker->m_shutdown_global == false)
	{
		ActionAttack counter_attack = attacker->GetCounterAttack();
		if (counter_attack.m_attack != NULL)
		{
			bool range_weapon_used = false;
			bool triggers_execution = false;
			(*CurrentGame).UI_AddEventLog("is launching a counter-attack.", Event_Neutral, attacker->m_index);
			ResolveAttack(counter_attack.m_attack, counter_attack.m_target_index, false, true, range_weapon_used, triggers_execution);

			//Execution?
			if (triggers_execution)
			{
				CounterAttack(attacker);
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
*/

bool InGameState::ResolveAttack(WeaponAttack* attack, SlotIndex target_index, bool is_execution, bool is_counter_attack, bool &range_weapon_used, bool &triggers_execution, bool &triggers_counterattack)
{
	Robot* robot = attack->m_owner->m_owner->m_owner;
	RobotSlot& robot_slot = *attack->m_owner->m_owner;
	Module* module = attack->m_owner->m_owner->m_module;

	Weapon* weapon = attack->m_owner;

	Robot& opponent = m_robots[(robot->m_index + 1) % 2];
	RobotSlot& target_slot = opponent.m_slots[target_index];
	Module* target_module = target_slot.m_module;

	int speed = (int)(Phase_AttackResolution_1)-(int)(*CurrentGame).m_phase + 1;

	triggers_execution = false;
	triggers_counterattack = false;

	ostringstream s_attack;
	s_attack << "is launching attack '" << attack->m_UI_display_name << "' at speed " << speed;
	(*CurrentGame).UI_AddEventLog(s_attack.str(), Event_Neutral, robot->m_index);
	if (module->m_health == 0)
	{
		(*CurrentGame).UI_AddEventLog("Attack cancelled: weapon module has been destroyed.", Event_Neutral, robot->m_index);
		return false;
	}
	else if (module->m_shutdown_counter > 0)
	{
		(*CurrentGame).UI_AddEventLog("Attack cancelled: weapon module has been shut down.", Event_Neutral, robot->m_index);
		return false;
	}
	else if (module->m_energy_cells_max > 0 && module->m_energy_cells == 0)
	{
		(*CurrentGame).UI_AddEventLog("Attack cancelled: weapon module not powered by 1 EC.", Event_Neutral, robot->m_index);
		return false;
	}
	else if (attack->m_crew_required != NB_CREW_TYPES && robot_slot.HasCrewRequired(attack->m_crew_required) == NULL)
	{
		(*CurrentGame).UI_AddEventLog("Attack cancelled: crew member required is missing or stunned.", Event_Neutral, robot->m_index);
		return false;
	}
	else if (robot->m_unbalanced_counter > 0)
	{
		(*CurrentGame).UI_AddEventLog("Attack cancelled: robot has been unbalanced.", Event_Neutral, robot->m_index);
		return false;
	}
	else if (robot->m_shutdown_global == true)
	{
		(*CurrentGame).UI_AddEventLog("Attack cancelled: robot is undergoing a global shutdown.", Event_Neutral, robot->m_index);
		return false;
	}
	else//Attack resolution
	{
		//Guard perfect?
		if (opponent.m_guard_speed == speed && is_execution == false && is_counter_attack == false && opponent.m_shutdown_global == false && opponent.m_grounded == false && opponent.m_unbalanced_counter == 0)
		{
			//Counter attack
			(*CurrentGame).UI_AddEventLog("Attack countered by a perfect guard.\nDo you want to counter-attack?", Event_ContextualChoice, opponent.m_index);

			(*CurrentGame).m_phase = Phase_CounterAttack;
			triggers_counterattack = true;
		}
		else
		{
			//Guard redirection
			if (speed <= opponent.m_guard_speed && is_execution == false && is_counter_attack == false)
			{
				Module* shield = opponent.GetShield();
				if (shield != NULL)
				{
					target_index = shield->m_owner->m_index;
				}
				else if (target_index == Index_FootL || target_index == Index_LegL || target_index == Index_ShoulderL)
				{
					target_index = Index_HandL;
					(*CurrentGame).UI_AddEventLog("Guard redirects attack from side to hand.", Event_Neutral, opponent.m_index);
				}
				else if (target_index == Index_FootR || target_index == Index_LegR || target_index == Index_ShoulderR)
				{
					target_index = Index_HandR;
					(*CurrentGame).UI_AddEventLog("Guard redirects attack from side to hand.", Event_Neutral, opponent.m_index);
				}
				else if (target_index == Index_BodyU || target_index == Index_BodyM || target_index == Index_BodyD)
				{
					target_index = Index_HandR;

					//todo : choose hand
					(*CurrentGame).UI_AddEventLog("Guard redirects attack from torso to hand.\nChoose which hand?", Event_ContextualChoice, opponent.m_index);
				}
			}

			//if there are both a ranged and a close-combat weapon on this attack speed, the close-combat is prioritary to determine the position at the end of the turn
			if (weapon->m_ranged == false)
			{
				range_weapon_used = false;
			}

			//Randomization of resolutions (check for any slot if it's been targeted)
			for (int i = 0; i < NB_SLOT_INDEX; i++)
			{
				if (i == target_index || (attack->m_hitmode == Hit_AdjacentSlots && opponent.GetDistanceFromSlotToSlot(target_index, (SlotIndex)i) == 1))
				{
					target_slot = opponent.m_slots[i];
					target_module = target_slot.m_module;

					int gunner_bonus = robot_slot.GetGunnerRangeBonus();
					int equipment_bonus = robot_slot.GetEquipmentRangeBonus();
					bool hit_success = weapon->m_ranged == false || is_execution == true || opponent.m_grabbed != NULL ||
						RandomizeIntBetweenValues(1, 6) + gunner_bonus + equipment_bonus >= attack->m_chance_of_hit + (target_slot.m_type == Slot_Head ? 1 : 0) + opponent.m_jammer_bonus;

					bool fire_success = i != Index_Head && attack->m_chance_of_fire > 0 && RandomizeIntBetweenValues(1, 6) >= attack->m_chance_of_fire;

					bool electricity_sucess = i != Index_Head && attack->m_chance_of_electricity > 0 && RandomizeIntBetweenValues(1, 6) >= attack->m_chance_of_electricity;

					int warrior_bonus = robot_slot.GetWarriorBalanceBonus();
					int counter_attack_bonus = is_counter_attack ? 5 : 0;
					int unbalanced_score = i == target_index ? attack->m_chance_of_unbalance + warrior_bonus + counter_attack_bonus + RandomizeIntBetweenValues(1, 20) - opponent.GetBalanceScore() : 0;

					//Hit resolution
					if (hit_success)
					{
						int damage = attack->m_damage;
						//Damage on grounded robot x2
						if (opponent.m_grounded == true || is_execution == true || opponent.m_grabbed != NULL)
						{
							damage *= 2;
						}

						ostringstream s_dmg;
						s_dmg << "is successfully hit with " << damage << " damage.";
						(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);

						bool module_damaged = false;

						//Energetical damage
						if (weapon->m_energetic)
						{
							//Deflector absorption
							for (vector<RobotSlot>::iterator it = opponent.m_slots.begin(); it != opponent.m_slots.end(); it++)
							{
								if (it->m_module != NULL && it->m_module->m_type == Module_Deflectors && it->m_module->IsOperationnal())
								{
									Module* deflector = it->m_module;
									if (deflector->m_shield > 0)
									{
										//Damage absorbed
										if (deflector->m_shield >= damage)
										{
											ostringstream s_deflector;
											s_deflector << "Deflectors absorb " << damage << " damage.";

											deflector->m_shield -= damage;
											damage = 0;

											(*CurrentGame).UI_AddEventLog(s_deflector.str(), Event_Shield, opponent.m_index);
										}
										else
										{
											ostringstream s_deflector;
											s_deflector << "Deflectors absorb " << deflector->m_shield << " damage and are DISABLED.";

											damage -= deflector->m_shield;
											deflector->m_shield = 0;

											(*CurrentGame).UI_AddEventLog(s_deflector.str(), Event_Shield, opponent.m_index);
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
										ostringstream s_dmg;
										s_dmg << "Module takes " << damage << " damage.";

										target_module->m_health -= damage;

										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}
									else
									{
										ostringstream s_dmg;
										s_dmg << "Module takes " << damage << " damage and is DESTROYED.";

										target_module->m_health = 0;
										opponent.DestroySlot((SlotIndex)i);

										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}

									//Damage to robot
									if (opponent.m_health > damage)
									{
										ostringstream s_dmg;
										s_dmg << "Robot takes " << damage << " damage.";
										opponent.m_health -= damage;
										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}
									else
									{
										opponent.m_health = 0;
										(*CurrentGame).UI_AddEventLog("'s health reached 0 and is DESTROYED.\nGAME OVER.", Event_Damage, opponent.m_index);
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
												ostringstream s_dmg;
												s_dmg << "Plates take " << damage << " damage.";

												(*it)->m_health -= damage;
												damage = 0;

												(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
											}
											else
											{
												ostringstream s_dmg;
												s_dmg << "Plates take " << (*it)->m_health << " damage and are DESTROYED.";

												damage -= (*it)->m_health;
												(*it)->m_health = 0;

												(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
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
										ostringstream s_dmg;
										s_dmg << "Module takes " << damage << " damage.";

										target_module->m_health -= damage;

										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}
									else
									{
										ostringstream s_dmg;
										s_dmg << "Module takes " << damage << " damage and is destroyed.";

										target_module->m_health = 0;
										opponent.DestroySlot(target_slot.m_index);

										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}

									//Damage to robot
									if (opponent.m_health > damage)
									{
										ostringstream s_dmg;
										s_dmg << damage << " damage are inflicted to the robot.";

										opponent.m_health -= damage;

										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}
									else
									{
										ostringstream s_dmg;
										s_dmg << damage << " damage are inflicted to the robot, which is destroyed.\nGAME OVER";

										opponent.m_health = 0;

										(*CurrentGame).UI_AddEventLog(s_dmg.str(), Event_Damage, opponent.m_index);
									}
								}
							}
						}

						//ONLY IF module has been actually damaged, apply secondary effects
						if (module_damaged)
						{
							//Randomization of damage to crew if module has been damaged
							for (vector<CrewMember*>::iterator it = target_slot.m_crew.begin(); it != target_slot.m_crew.end(); it++)
							{
								bool hit_roll = RandomizeIntBetweenValues(1, 6) >= 4;
								if (hit_roll == true)
								{
									(*it)->m_health--;

									(*CurrentGame).UI_AddEventLog("1 damage to crew member in the module.", Event_Damage, opponent.m_index);
								}
							}

							opponent.UpdateCrew(target_slot.m_index);

							//Fire resolution
							if (fire_success && target_module != NULL)
							{
								if (target_module->m_fire_counter < 2)
								{
									target_module->m_fire_counter = 2;

									(*CurrentGame).UI_AddEventLog("Fire started in module.", Event_Fire, opponent.m_index);
								}
							}

							//Electric resolution
							if (electricity_sucess && target_module != NULL)
							{
								opponent.ShutdownSlot(target_slot.m_index);

								(*CurrentGame).UI_AddEventLog("Module is shutdown by the hit.", Event_Shutdown, opponent.m_index);
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

										(*CurrentGame).UI_AddEventLog("Crew member stunned by the hit.", Event_Fire, opponent.m_index);
									}
								}
							}

							//Balance resolution - this one is tested only once per hit, on the targeted module
							if (i == target_index)
							{
								if (unbalanced_score > 0)
								{
									if (opponent.m_unbalanced_counter == 0)
									{
										opponent.m_unbalanced_counter = 2;
										opponent.m_unbalanced_value = unbalanced_score;

										(*CurrentGame).UI_AddEventLog("Robot gets unbalaced.", Event_Balance, opponent.m_index);
									}
									else
									{
										opponent.m_unbalanced_counter = 2;
										opponent.m_unbalanced_value = unbalanced_score;

										if (opponent.m_grounded == false)
										{
											opponent.m_grounded = true;
											triggers_execution = true;

											(*CurrentGame).UI_AddEventLog("Robot gets unbalanced again and is put on ground.", Event_Balance, opponent.m_index);
											(*CurrentGame).UI_AddEventLog("Do you want to launch an Execution?", Event_ContextualChoice, robot->m_index);
										}
									}
								}
							}
							
							//Check global shutdown conditions
							opponent.UpdateShudownGlobal();
						}
					}
					else
					{
						ostringstream s_miss;
						s_miss << attack->m_UI_display_name << "'s attack dodged";
						(*CurrentGame).UI_AddEventLog(s_miss.str(), Event_Neutral, opponent.m_index);
					}
				}
			}
		}

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
		if (it->m_resolved == true)
		{
			continue;
		}

		//Bonus for ranged weapons if combat starts from a Ranged distance
		int speed_bonus = 0;
		if ((*CurrentGame).m_distance == Distance_Ranged && it->m_attack->m_owner->m_ranged == true)
		{
			speed_bonus = 2;
		}

		if ((*it).m_attack->m_owner->m_ranged == false)
		{
			RobotSlot* slot = (*it).m_attack->m_owner->m_owner;
			speed_bonus += slot->GetEquipmentSpeedBonus();
			speed_bonus += slot->GetPilotSpeedBonus();
		}

		if (it->m_attack->m_speed + speed_bonus != speed)
		{
			continue;
		}

		printf("Attack resolution (speed %d).\n", speed);
		bool triggers_execution = false;
		bool triggers_counterattack = false;
		ResolveAttack(it->m_attack, it->m_target_index, false, false, range_weapon_used, triggers_execution, triggers_counterattack);
		it->m_resolved = true;

		//Execution?
		if (triggers_execution == true)
		{
			(*CurrentGame).m_phase = Phase_Execution;
		}
		//Counter-attack?
		else if (triggers_counterattack == true)
		{
			(*CurrentGame).m_phase = Phase_CounterAttack;
		}

		if (triggers_execution == true || triggers_counterattack == true)
		{
			//lay down a flag that will let us know if the player chooses to launch the contextual attack or decline it
			m_nb_attacks_becore_contextual = (*CurrentGame).m_attacks_list.size();

			//reload weapons
			int robot_index = it->m_attack->m_owner->m_owner->m_owner->m_index;
			m_robots[robot_index].ReloadWeapons();

			//clear targets displayed sooner than usual
			for (int r = 0; r < 2; r++)
			{
				for (vector<UI_Element>::iterator it = m_robots[r].m_UI_slots.begin(); it != m_robots[r].m_UI_slots.end(); it++)
				{
					it->m_shape.setFillColor(sf::Color(0, 0, 0, 0));
				}
			}
			return;
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

			ostringstream s_effect;
			if (it->m_effect->m_owner_module != NULL)
			{
				s_effect << it->m_effect->m_owner_module->m_UI_display_name;
			}
			else if (it->m_effect->m_owner_equipment != NULL)
			{
				s_effect << it->m_effect->m_owner_equipment->m_UI_display_name;
			}
			s_effect << " used.";
			(*CurrentGame).UI_AddEventLog(s_effect.str(), Event_Neutral, r);

			Robot* robot = it->m_effect->m_owner_module != NULL ? it->m_effect->m_owner_module->m_owner->m_owner : it->m_effect->m_owner_equipment->m_owner->m_owner;
			RobotSlot& robot_slot = it->m_effect->m_owner_module != NULL ? *it->m_effect->m_owner_module->m_owner : *it->m_effect->m_owner_equipment->m_owner;

			Module* module = it->m_effect->m_owner_module != NULL ? it->m_effect->m_owner_module : NULL;
			Equipment* equipment = it->m_effect->m_owner_equipment != NULL ? it->m_effect->m_owner_equipment : NULL;
			EquipmentEffect* effect = it->m_effect;

			Robot* opponent = &m_robots[(robot->m_index + 1) % 2];
			RobotSlot& target_slot = opponent->m_slots[it->m_target_index];
			Module* target_module = target_slot.m_module;

			switch (effect->m_type)
			{
				case Effect_EMP:
				{
					opponent->m_energy_cells -= target_module->m_energy_cells;
					target_module->m_energy_cells = 0;
					for (vector<Equipment*>::iterator it2 = target_module->m_owner->m_equipments.begin(); it2 != target_module->m_owner->m_equipments.end(); it2++)
					{
						opponent->m_energy_cells -= (*it2)->m_energy_cells;
						(*it2)->m_energy_cells = 0;
					}
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
				case Effect_Jammer:
				{
					m_robots[r].m_jammer_bonus = 1;
					break;
				}
			}
		}
	}

	//Clearing the actions list
	(*CurrentGame).m_effects_list.clear();

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

			Robot& opponent = m_robots[(robot->m_index + 1) % 2];
			Module* target_module = opponent.m_slots[it->m_target_index].m_module;

			//Resolve grab
			bool hit_success = opponent.m_shutdown_global == true || (target_module != NULL && (target_module->m_shutdown_counter > 0 || target_module->m_health == 0));

			if (hit_success)
			{
				//Grab and record which Hand did the grab, because only destroying that hand will release the grab
				opponent.m_grabbed = module;

				(*CurrentGame).UI_AddEventLog("Grab successful. Enemy gets grabbed.\n(x2 damage and can't get away from close-combat)", Event_Grab, robot->m_index);
			}
			else if (opponent.m_shutdown_global == false && opponent.m_grounded == false && opponent.m_unbalanced_counter == 0)
			{
				//Counter-attack
				(*CurrentGame).UI_AddEventLog("Grab dodged. Do you want to counter-attack?", Event_Grab, opponent.m_index);

				(*CurrentGame).m_phase = Phase_CounterAttack;
				return;
				//CounterAttack(&opponent);
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

				Robot* opponent = &m_robots[(robot->m_index + 1) % 2];
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