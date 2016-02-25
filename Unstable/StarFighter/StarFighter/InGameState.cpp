#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	//Load knownScenes, hazard levels and current scene from save file
	if (!LoadPlayerSave(player.m_save_file).empty())
	{
		player.m_currentSceneFile = LoadPlayerSave(player.m_save_file);
	}
	else
	{
		//New game save
		player.m_currentSceneFile = "Vanguard_Hub0";
		AddToKnownScenes(player.m_currentSceneFile);
		SavePlayer(PLAYER_SAVE_FILE);
	}
	m_currentSceneSave = player.m_currentSceneFile;

	//Loading current scene
	m_currentScene = new Scene(player.m_currentSceneFile, GetSceneHazardLevelUnlocked(player.m_currentSceneFile), player.reverse_scene, true);//first_scene = true
	UpdatePortalsMaxUnlockedHazardLevel(m_currentScene);

	sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_X_RATIO);
	if ((*CurrentGame).m_direction != Directions::NO_DIRECTION)
	{
		m_IG_State = InGameStateMachine::SCROLLING;
		ship_pos = GameObject::getPosition_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO));
	}
	else
	{
		m_IG_State = InGameStateMachine::HUB_ROAMING;
	}
	
	//creating new ship
	m_playerShip = FileLoader::LoadShipConfig("default");
	m_playerShip->setPosition(ship_pos);
	(*CurrentGame).SetPlayerShip(m_playerShip);
	m_playerShip->m_respawnSceneName = m_currentScene->m_name;

	//Load saved items
	if (!Ship::LoadPlayerItems(ITEMS_SAVE_FILE, m_playerShip))
	{
		//or create a new save file
		Ship::SaveItems(ITEMS_SAVE_FILE, m_playerShip);
	}
	//load money
	if (!Ship::LoadPlayerMoney(MONEY_SAVE_FILE, m_playerShip))
	{
		//or create a new save file
		Ship::SavePlayerMoney(MONEY_SAVE_FILE, m_playerShip);
	}
	m_playerShip->Init();
	m_playerShip->ResplenishHealth();
	LOGGER_WRITE(Logger::Priority::DEBUG, "Playership loaded\n");

	//initalizing equipment in HUD
	LOGGER_WRITE(Logger::Priority::DEBUG, "Initializing equipment in HUD...");
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_playerShip->m_equipment[i] != NULL)
		{
			GameObject* capsule = Ship::CloneEquipmentIntoGameObject(m_playerShip->m_equipment[i]);
			if (!(*CurrentGame).InsertObjectInShipGrid(*capsule, i))
			{
				LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: could not initialize an equipment item from the ship config to the HUD Ship grid\n");
			}
		}
	}
	
	if (m_playerShip->m_weapon != NULL)
	{
		GameObject* capsule = Ship::CloneWeaponIntoGameObject(m_playerShip->m_weapon);
		if (!(*CurrentGame).InsertObjectInShipGrid(*capsule, NBVAL_Equipment))
		{
			LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: could not initialize a weapon item from the ship config to the HUD Ship grid\n");
		}
	}
	LOGGER_WRITE(Logger::Priority::DEBUG, "HUD initialization completed\n");

	//ship
	(*CurrentGame).playerShip->GenerateBots((*CurrentGame).playerShip);
	if ((*CurrentGame).m_direction != Directions::NO_DIRECTION)
	{
		(*CurrentGame).playerShip->m_disabledHyperspeed = false;
	}
	else
	{
		(*CurrentGame).playerShip->m_disable_fire = true;
		(*CurrentGame).playerShip->m_disabledHyperspeed = true;
	}
	(*CurrentGame).playerShip->GenerateFakeShip((*CurrentGame).playerShip);
	(*CurrentGame).SetLayerRotation(LayerType::FakeShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).SetLayerRotation(LayerType::BotLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).SetLayerRotation(LayerType::FeedbacksLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);
}

void InGameState::Update(Time deltaTime)
{
	//automatic respawn if dead
	if (!(*CurrentGame).playerShip->m_visible)
	{
		RespawnInLastHub();
	}

	InGameStateMachineCheck(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), m_currentScene->getSceneBeastScore());

	(*CurrentGame).updateScene(deltaTime);

	//synchronizing shop interface with HUD interface
	GameObject* obj = (*CurrentGame).UpdateInteractionPanel((*CurrentGame).playerShip->m_previouslyCollidingWithInteractiveObject, (*CurrentGame).playerShip->GetFocusedPortalMaxUnlockedHazardLevel(), deltaTime);
	if (obj)
	{
		if (obj->m_weapon_loot)
		{
			(*CurrentGame).m_hud.focused_item = (*CurrentGame).m_hud.shipGrid.getCellPointerFromIntIndex(NBVAL_Equipment);
			(*CurrentGame).m_hud.hud_cursor->setPosition((*CurrentGame).m_hud.fakeShipGrid.getCellPointerFromIntIndex(NBVAL_Equipment)->getPosition());
			//displaying stats of focused item in shop
			SendFocusedItemDataToHintPanel(obj, deltaTime);
			(*CurrentGame).m_hud.has_focus = true;
		}
		else if (obj->m_equipment_loot)
		{
			(*CurrentGame).m_hud.focused_item = (*CurrentGame).m_hud.shipGrid.getCellPointerFromIntIndex(obj->m_equipment_loot->m_equipmentType);
			(*CurrentGame).m_hud.hud_cursor->setPosition((*CurrentGame).m_hud.fakeShipGrid.getCellPointerFromIntIndex(obj->m_equipment_loot->m_equipmentType)->getPosition());
			//displaying stats of focused item in shop
			SendFocusedItemDataToHintPanel(obj, deltaTime);
			(*CurrentGame).m_hud.has_focus = true;
		}
	}
	//if we are not in the "sell" menu then we might be in the "buy" menu where those values should be reset
	else if (!(*CurrentGame).playerShip->m_is_sell_available && (*CurrentGame).GetShopMenu() == ShopBuyMenu)
	{
		(*CurrentGame).m_hud.focused_item = NULL;
		(*CurrentGame).m_hud.has_focus = false;
	}

	//displaying stats of focused item in the HUD...
	if ((*CurrentGame).getHudFocusedItem())
	{
		SendFocusedItemDataToHUD((*CurrentGame).getHudFocusedItem(), deltaTime);
	}
	else //...else not bothering with it
	{
		(*CurrentGame).updateHud((*CurrentGame).playerShip->m_armor, (*CurrentGame).playerShip->m_armor_max, (*CurrentGame).playerShip->m_shield, (*CurrentGame).playerShip->m_shield_max, (*CurrentGame).playerShip->m_money,
			(*CurrentGame).playerShip->m_graze_count, m_currentScene->getSceneHazardLevelValue(), m_currentScene->m_bg->m_display_name, (*CurrentGame).playerShip->m_level, (*CurrentGame).playerShip->m_level_max, (*CurrentGame).playerShip->m_xp, (*CurrentGame).playerShip->m_xp_max, deltaTime, m_currentScene->m_direction == NO_DIRECTION);
	}

	this->mainWindow->clear();
}

void InGameState::SendFocusedItemDataToHintPanel(GameObject* focused_item, Time deltaTime)
{
	//displaying stats of focused item in the HUD...
	GameObject* tmp_ptr = focused_item;
	int equip_index_ = NBVAL_Equipment;
	if (tmp_ptr->getEquipmentLoot() != NULL)
	{
		equip_index_ = tmp_ptr->getEquipmentLoot()->m_equipmentType;
	}

	if (equip_index_ == NBVAL_Equipment)
	{
		Weapon* tmp_weapon = tmp_ptr->getWeaponLoot();

		InteractionPanel::UpdateItemStatsText(&(*CurrentGame).m_interactionPanel->m_itemStatsText, equip_index_, tmp_weapon->m_display_name, tmp_weapon->m_level, tmp_weapon->m_credits, -1, -1, -1, -1, -1, tmp_weapon->m_ammunition->m_damage, false, tmp_weapon->m_ammunition->m_speed.y, tmp_weapon->m_ammunition->m_Pattern.currentPattern, tmp_weapon->m_multishot,
			tmp_weapon->m_xspread, tmp_weapon->m_rate_of_fire, tmp_weapon->m_shot_mode, tmp_weapon->m_dispersion, tmp_weapon->m_rafale, tmp_weapon->m_rafale_cooldown, tmp_weapon->m_target_seaking);

		tmp_weapon = NULL;
	}
	else
	{
		Equipment* tmp_equipment = tmp_ptr->getEquipmentLoot();

		if (!tmp_equipment->m_bot)
		{
			InteractionPanel::UpdateItemStatsText(&(*CurrentGame).m_interactionPanel->m_itemStatsText, equip_index_, tmp_equipment->m_display_name, tmp_equipment->m_level, tmp_equipment->m_credits, tmp_equipment->m_max_speed, tmp_equipment->m_hyperspeed, tmp_equipment->m_armor,
				tmp_equipment->m_shield, tmp_equipment->m_shield_regen, tmp_equipment->m_damage, tmp_equipment->m_bot);
		}
		else
		{
			//todo : clean getEquipmentHyperspeed() etc...
			InteractionPanel::UpdateItemStatsText(&(*CurrentGame).m_interactionPanel->m_itemStatsText, equip_index_, tmp_equipment->m_display_name, tmp_equipment->m_level, tmp_equipment->m_credits, tmp_equipment->m_bot->m_Pattern.patternSpeed, tmp_equipment->m_hyperspeed, tmp_equipment->m_bot->m_armor_max,
				tmp_equipment->m_bot->m_shield_max, tmp_equipment->m_bot->m_shield_regen, tmp_equipment->m_bot->m_weapon->m_ammunition->m_damage, tmp_equipment->m_bot, tmp_equipment->m_bot->m_weapon->m_ammunition->m_speed.y,
				tmp_equipment->m_bot->m_weapon->m_ammunition->m_Pattern.currentPattern, tmp_equipment->m_bot->m_weapon->m_multishot, tmp_equipment->m_bot->m_weapon->m_xspread, tmp_equipment->m_bot->m_weapon->m_rate_of_fire,
				tmp_equipment->m_bot->m_weapon->m_shot_mode, tmp_equipment->m_bot->m_weapon->m_dispersion, tmp_equipment->m_bot->m_weapon->m_rafale, tmp_equipment->m_bot->m_weapon->m_rafale_cooldown, tmp_equipment->m_bot->m_weapon->m_target_seaking);
		}

		tmp_equipment = NULL;
	}

	tmp_ptr = NULL;
}

void InGameState::SendFocusedItemDataToHUD(GameObject* focused_item, Time deltaTime)
{
	//displaying stats of focused item in the HUD...
	GameObject* tmp_ptr = focused_item;
	int equip_index_ = NBVAL_Equipment;
	if (tmp_ptr->getEquipmentLoot() != NULL)
	{
		equip_index_ = tmp_ptr->getEquipmentLoot()->m_equipmentType;
	}

	if (equip_index_ == NBVAL_Equipment)
	{
		Weapon* tmp_weapon = tmp_ptr->getWeaponLoot();

		(*CurrentGame).updateHud((*CurrentGame).playerShip->m_armor, (*CurrentGame).playerShip->m_armor_max, (*CurrentGame).playerShip->m_shield, (*CurrentGame).playerShip->m_shield_max, (*CurrentGame).playerShip->m_money, (*CurrentGame).playerShip->m_graze_count,
			m_currentScene->getSceneHazardLevelValue(), m_currentScene->m_bg->m_display_name, (*CurrentGame).playerShip->m_level, (*CurrentGame).playerShip->m_level_max, (*CurrentGame).playerShip->m_xp, (*CurrentGame).playerShip->m_xp_max, deltaTime, m_currentScene->m_direction == NO_DIRECTION,
			equip_index_, tmp_weapon->m_display_name, tmp_weapon->m_level, tmp_weapon->m_credits, -1, -1, -1, -1, -1, tmp_weapon->m_ammunition->m_damage, false, tmp_weapon->m_ammunition->m_speed.y, tmp_weapon->m_ammunition->m_Pattern.currentPattern, tmp_weapon->m_multishot,
			tmp_weapon->m_xspread, tmp_weapon->m_rate_of_fire, tmp_weapon->m_shot_mode, tmp_weapon->m_dispersion, tmp_weapon->m_rafale, tmp_weapon->m_rafale_cooldown, tmp_weapon->m_target_seaking);

		tmp_weapon = NULL;
	}
	else
	{
		Equipment* tmp_equipment = tmp_ptr->getEquipmentLoot();

		if (!tmp_equipment->m_bot)
		{
			(*CurrentGame).updateHud((*CurrentGame).playerShip->m_armor, (*CurrentGame).playerShip->m_armor_max, (*CurrentGame).playerShip->m_shield, (*CurrentGame).playerShip->m_shield_max, (*CurrentGame).playerShip->m_money,
				(*CurrentGame).playerShip->m_graze_count, m_currentScene->getSceneHazardLevelValue(), m_currentScene->m_bg->m_display_name, (*CurrentGame).playerShip->m_level, (*CurrentGame).playerShip->m_level_max, (*CurrentGame).playerShip->m_xp, (*CurrentGame).playerShip->m_xp_max, deltaTime, m_currentScene->m_direction == NO_DIRECTION,
				equip_index_, tmp_equipment->m_display_name, tmp_equipment->m_level, tmp_equipment->m_credits, tmp_equipment->m_max_speed, tmp_equipment->m_hyperspeed, tmp_equipment->m_armor,
				tmp_equipment->m_shield, tmp_equipment->m_shield_regen, tmp_equipment->m_damage, tmp_equipment->m_bot);
		}
		else
		{
			//todo : clean getEquipmentHyperspeed() etc...
			(*CurrentGame).updateHud((*CurrentGame).playerShip->m_armor, (*CurrentGame).playerShip->m_armor_max, (*CurrentGame).playerShip->m_shield, (*CurrentGame).playerShip->m_shield_max, (*CurrentGame).playerShip->m_money,
				(*CurrentGame).playerShip->m_graze_count, m_currentScene->getSceneHazardLevelValue(), m_currentScene->m_bg->m_display_name, (*CurrentGame).playerShip->m_level, (*CurrentGame).playerShip->m_level_max, (*CurrentGame).playerShip->m_xp, (*CurrentGame).playerShip->m_xp_max, deltaTime, m_currentScene->m_direction == NO_DIRECTION,
				equip_index_, tmp_equipment->m_display_name, tmp_equipment->m_level, tmp_equipment->m_credits, tmp_equipment->m_bot->m_Pattern.patternSpeed, tmp_equipment->m_hyperspeed, tmp_equipment->m_bot->m_armor_max,
				tmp_equipment->m_bot->m_shield_max, tmp_equipment->m_bot->m_shield_regen, tmp_equipment->m_bot->m_weapon->m_ammunition->m_damage, tmp_equipment->m_bot, tmp_equipment->m_bot->m_weapon->m_ammunition->m_speed.y,
				tmp_equipment->m_bot->m_weapon->m_ammunition->m_Pattern.currentPattern, tmp_equipment->m_bot->m_weapon->m_multishot, tmp_equipment->m_bot->m_weapon->m_xspread, tmp_equipment->m_bot->m_weapon->m_rate_of_fire,
				tmp_equipment->m_bot->m_weapon->m_shot_mode, tmp_equipment->m_bot->m_weapon->m_dispersion, tmp_equipment->m_bot->m_weapon->m_rafale, tmp_equipment->m_bot->m_weapon->m_rafale_cooldown, tmp_equipment->m_bot->m_weapon->m_target_seaking);
		}

		tmp_equipment = NULL;
	}

	tmp_ptr = NULL;
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
	(*CurrentGame).drawHud();
}

void InGameState::Release()
{
	//TODO
}

bool InGameState::AddToKnownScenes(string scene_name)
{
	m_currentSceneSave = scene_name;
	map<string, int>::iterator it = m_knownScenes.find(scene_name);

	//if scene not already known
	if (it == m_knownScenes.end())
	{
		//add it to the map of known scenes
		m_knownScenes.insert(pair<string, int>(scene_name, 0));
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Adding '%s' to known scenes.\n", (char*)scene_name.c_str()));
		return true;
	}
	return false;
}

void InGameState::SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level)
{
	map<string, int>::iterator it = m_knownScenes.find(scene_name);
	if (hazard_level > NB_HAZARD_LEVELS - 1)
	{
		hazard_level = NB_HAZARD_LEVELS - 1;
	}
	if (it != m_knownScenes.end())
	{
		m_knownScenes[scene_name] = hazard_level;
	}
}

int InGameState::GetSceneHazardLevelUnlocked(string scene_name)
{
	map<string, int>::iterator it = m_knownScenes.find(scene_name);

	//if scene is known
	if (it != m_knownScenes.end())
	{
		return m_knownScenes[scene_name];
	}
	//else
	return 0;
}

int InGameState::SavePlayer(string file)
{
	LOGGER_WRITE(Logger::Priority::DEBUG, "Saving known scenes and current scene in profile.\n");

	ofstream data(file.c_str(), ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (map<string, int>::iterator it = m_knownScenes.begin(); it != m_knownScenes.end(); it++)
		{
			data << it->first.c_str() << " " << it->second;
			if (it->first.c_str() == m_currentSceneSave)
			{
				data << " " << "!";
			}
			data << endl;
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "Failed to open PLAYER SAVE FILE !" << endl;
	}

	return 0;
}

string InGameState::LoadPlayerSave(string file)
{
	string return_current_scene;

	std::ifstream  data(file, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			string scene;
			int level;
			std::istringstream(line) >> scene >> level;
			string current_scene;
				
			std::istringstream(line) >> scene >> level >> current_scene;

			m_knownScenes.insert(std::pair<string, int>(scene, level));
			if (current_scene.compare("!") == 0)
			{
				return_current_scene = scene;
			}
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "Failed to open PLAYER SAVE FILE !" << endl;
	}

	return return_current_scene;
}

void InGameState::InGameStateMachineCheck(sf::Time deltaTime)
{
	float w = m_currentScene->m_bg->m_size.x;
	float h = m_currentScene->m_bg->m_size.y;
	float w_ = (*CurrentGame).playerShip->m_size.x;
	float h_ = (*CurrentGame).playerShip->m_size.y;

	switch (m_IG_State)
	{
		case InGameStateMachine::SCROLLING:
		{
			if (m_currentScene->m_generating_enemies)
			{
				m_currentScene->GenerateEnemiesv2(deltaTime);
			}
			//Scrolling until the background reaches its end
			if ((*CurrentGame).m_direction == Directions::NO_DIRECTION)
			{
				m_IG_State = InGameStateMachine::LAST_SCREEN;
			}
			else
			{
				//when last screen is reached
				if (m_currentScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2)) == FloatCompare::GREATHER_THAN
					|| m_currentScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2)) == FloatCompare::EQUAL_TO)
				{
					//Correct the position
					m_currentScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2));
					m_currentScene->m_bg->m_speed = sf::Vector2f(0, 0);
					(*CurrentGame).m_vspeed = 0;

					//Disable hyperspeed capacity
					(*CurrentGame).playerShip->m_disabledHyperspeed = true;

					//Stop spawning enemies
					m_currentScene->m_generating_enemies = false;
					if (m_currentScene->m_generating_boss)
					{
						m_bossSpawnCountdown.restart();
					}
					
					m_hasDisplayedDestructionRatio = false;
					m_IG_State = InGameStateMachine::LAST_SCREEN;

					//Wipe out enemies that were spawned offscren
					(*CurrentGame).garbageLayer(LayerType::EnemyObjectLayer, true);
				}
			}

			break;
		}

		case InGameStateMachine::LAST_SCREEN:
		{	
			//When enemies, loots and enemy bullets on scene are dead, we can start the transition to the next scene
			if ((*CurrentGame).isLastEnemyDead())
			{
				//scene generates boss? (or boss is dead, eitherway)
				if (m_currentScene->m_generating_boss)
				{
					m_currentScene->m_bg->SetPortalsState(PortalState::PortalInvisible);
					if (m_bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
					{
						m_currentScene->GenerateBoss();
						m_IG_State = InGameStateMachine::BOSS_FIGHT;
					}
				}
				else
				{
					if (!m_hasDisplayedDestructionRatio)
					{
						//is the scene capable of hazard break? (= last scene before hub)
						if (m_currentScene->m_canHazardBreak)
						{
							//what is our destruction ratio? (displaying score). 100% = Hazard break
							if ((*CurrentGame).getHazard() - (*CurrentGame).m_hazardSpawned == 0)
							{
								m_currentScene->HazardBreak();
								m_currentScene->DisplayDestructions(true);
							}
							else
							{
								m_currentScene->DisplayDestructions();
							}

							//eitherway, reset the score
							(*CurrentGame).resetHazard();
						}

						m_currentScene->m_bg->SetPortalsState(PortalState::PortalOpen);

						m_hasDisplayedDestructionRatio = true;
					}
				}

				//player takes exit?
				if ((*CurrentGame).playerShip->m_interactionType == PortalInteraction)
				{
					m_currentScene->m_bg->SetPortalsState(PortalState::PortalGhost);

					(*CurrentGame).removeFromFeedbacks(&(m_currentScene->m_textHazardBreak));

					bool reverse = false;
					if ((*CurrentGame).playerShip->m_targetPortal->m_direction == Directions::DIRECTION_DOWN || (*CurrentGame).playerShip->m_targetPortal->m_direction == Directions::DIRECTION_LEFT)
					{
						reverse = true;
					}

					string nextScene_filename = (*CurrentGame).playerShip->m_targetPortal->m_destination_name;
					m_nextScene = new Scene(nextScene_filename, (*CurrentGame).m_interactionPanel->m_selected_index, reverse, false);
					//remembering linked scenes to hazard break later
					if (!m_currentScene->m_canHazardBreak)
					{
						m_nextScene->m_scenesLinkedToUpdate.push_back(m_currentScene->m_name);
					}
					UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);

					m_nextScene->m_bg->m_speed = sf::Vector2f(0, 0);

					//Putting the player on rails
					(*CurrentGame).playerShip->m_disable_inputs = true;
					(*CurrentGame).playerShip->m_disable_fire = true;
					(*CurrentGame).playerShip->m_speed = -GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_UP);

					m_IG_State = InGameStateMachine::TRANSITION_PHASE1_2;
				}
			}
			//clearing enemies that have spawned out of the scene size
			else
			{
				if (m_currentScene->m_generating_boss)
				{
					m_bossSpawnCountdown.restart();
				}
				m_currentScene->m_bg->SetPortalsState(PortalState::PortalClose);
			}

			break;
		}

		case InGameStateMachine::BOSS_FIGHT:
		{
			//is boss dead?
			if ((*CurrentGame).isLastEnemyDead())
			{
				m_currentScene->m_generating_boss = false;
				m_IG_State = InGameStateMachine::LAST_SCREEN;
			}

			break;
		}

		case InGameStateMachine::TRANSITION_PHASE1_2:
		{
			//When the playership reaches the scene border, we can start the swapping of scenes, while replacing him on the right starting position for the next scene
			if ((*CurrentGame).playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2)) == FloatCompare::LESSER_THAN
				|| (*CurrentGame).playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2)) == FloatCompare::EQUAL_TO)
			{
				//Correction of playership position
				(*CurrentGame).playerShip->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2));

				(*CurrentGame).playerShip->m_speed = GameObject::getSpeed_to_LocationWhileSceneSwap((*CurrentGame).m_direction, m_nextScene->m_direction, ENDSCENE_TRANSITION_SPEED_DOWN,
					(*CurrentGame).playerShip->getPosition());

				m_currentScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).m_vspeed = ENDSCENE_TRANSITION_SPEED_DOWN;
				m_nextScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
				(*CurrentGame).garbageLayer(LayerType::LootLayer);
				if (m_nextScene->m_direction == Directions::NO_DIRECTION)
				{
					(*CurrentGame).garbageLayer(LayerType::BotLayer);
				}

				m_IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
			}

			break;
		}

		case InGameStateMachine::TRANSITION_PHASE2_2:
		{
			float wn = m_nextScene->m_bg->m_size.x;
			float hn = m_nextScene->m_bg->m_size.y;
			//When the new scene is completely swapped, we can wrap up the replacement and restart scrolling (or do what the Hubs do if the scene is a Hub)
			if (m_nextScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == FloatCompare::GREATHER_THAN
				|| m_nextScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == FloatCompare::EQUAL_TO)
			{

				//Correction of the scenes position
				m_currentScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f((w / 2), (h / 2)));

				m_nextScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2)));
				m_nextScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling(m_nextScene->m_direction, m_nextScene->m_vspeed);
				(*CurrentGame).m_vspeed = m_nextScene->m_vspeed;

				if (m_nextScene->m_direction == Directions::NO_DIRECTION)
				{
					m_IG_State = InGameStateMachine::HUB_ROAMING;
					(*CurrentGame).playerShip->m_disabledHyperspeed = true;
				}
				else
				{
					m_IG_State = InGameStateMachine::SCROLLING;
					(*CurrentGame).playerShip->m_disable_fire = false;
					(*CurrentGame).playerShip->m_disabledHyperspeed = false;
					//if (m_currentScene->m_direction == Directions::NO_DIRECTION)
					//{
					//	(*CurrentGame).playerShip->GenerateBots((*CurrentGame).playerShip);
					//}
					(*CurrentGame).SetLayerRotation(LayerType::PlayerShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					(*CurrentGame).SetLayerRotation(LayerType::FakeShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					(*CurrentGame).SetLayerRotation(LayerType::BotLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					(*CurrentGame).SetLayerRotation(LayerType::FeedbacksLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
				}

				//Saving the hazard level change
				if (m_currentScene->m_canHazardBreak)
				{
					SaveSceneHazardLevelUnlocked(m_currentScene->m_name, m_currentScene->getSceneHazardLevelUnlockedValue());
					for (vector <string>::iterator it = m_currentScene->m_scenesLinkedToUpdate.begin(); it != m_currentScene->m_scenesLinkedToUpdate.end(); it++)
					{
						SaveSceneHazardLevelUnlocked((*it), m_currentScene->getSceneHazardLevelUnlockedValue());
					}
					m_currentScene->m_scenesLinkedToUpdate.clear();
					//transmitting the info to the next scene so it can update its portals
					UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);
				}
				else
				{
					//if the next scene is not a hub, then we want to remember the past scenes so we can update them later in case of hazard break
					for (vector <string>::iterator it = m_currentScene->m_scenesLinkedToUpdate.begin(); it != m_currentScene->m_scenesLinkedToUpdate.end(); it++)
					{
						m_nextScene->m_scenesLinkedToUpdate.push_back((*it));
					}
				}

				//Wiping the previous background and swapping with the new one
				m_currentScene->DestroyScene();
				delete m_currentScene;
				m_currentScene = m_nextScene;
				m_nextScene = NULL;
				(*CurrentGame).m_direction = m_currentScene->m_direction;

				//Save scenes
				AddToKnownScenes(m_currentScene->m_name);
				if (m_currentScene->m_direction == Directions::NO_DIRECTION)
				{
					SavePlayer(PLAYER_SAVE_FILE);
					m_playerShip->m_respawnSceneName = m_currentScene->m_name;
				}

				//Giving control back to the player
				(*CurrentGame).playerShip->m_disable_inputs = false;
				(*CurrentGame).playerShip->m_speed = sf::Vector2f(0, 0);
			}

			break;
		}

		case InGameStateMachine::HUB_ROAMING:
		{
			m_currentScene->m_bg->SetPortalsState(PortalState::PortalOpen);
			//player takes exit?
			if ((*CurrentGame).playerShip->m_interactionType == PortalInteraction)
			{
				m_currentScene->m_bg->SetPortalsState(PortalState::PortalGhost);
				bool reverse = false;
				if ((*CurrentGame).playerShip->m_targetPortal->m_direction == Directions::DIRECTION_DOWN || (*CurrentGame).playerShip->m_targetPortal->m_direction == Directions::DIRECTION_LEFT)
				{
					reverse = true;
				}
				string nextScene_filename = (*CurrentGame).playerShip->m_targetPortal->m_destination_name;
				(*CurrentGame).m_direction = (*CurrentGame).playerShip->m_targetPortal->m_direction;
				m_nextScene = new Scene(nextScene_filename, (*CurrentGame).m_interactionPanel->m_selected_index, reverse, false);
				(*CurrentGame).playerShip->m_last_hazard_level_played = (*CurrentGame).m_interactionPanel->m_selected_index;
				UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);
				m_nextScene->m_bg->m_speed = sf::Vector2f(0, 0);

				//Putting the player on rails
				(*CurrentGame).playerShip->m_disable_inputs = true;
				(*CurrentGame).playerShip->m_disable_fire = true;
				(*CurrentGame).playerShip->m_speed = -GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_UP);

				m_IG_State = InGameStateMachine::TRANSITION_PHASE1_2;
			}

			break;
		}
	}
}

void InGameState::UpdatePortalsMaxUnlockedHazardLevel(Scene* scene_)
{
	//getting the max hazard value for the upcoming scene
	map<string, int>::iterator it = m_knownScenes.find(scene_->m_name);
	if (it != m_knownScenes.end())
	{
		scene_->m_hazard_level_unlocked = m_knownScenes[scene_->m_name];
	}
	else
	{
		//destination is not know yet -> default max hazard value
		scene_->m_hazard_level_unlocked = 0;
	}

	//loading the scene's portals with the info about their respective max hazard values
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (scene_->m_bg->m_portals[(Directions)i] != NULL)
		{
			map<string, int>::iterator it = m_knownScenes.find(scene_->m_bg->m_portals[(Directions)i]->m_destination_name);
			if (it != m_knownScenes.end())
			{
				scene_->m_bg->m_portals[(Directions)i]->m_max_unlocked_hazard_level = m_knownScenes[scene_->m_bg->m_portals[(Directions)i]->m_destination_name];
			}
			else
			{
				//destination is not know yet -> default max hazard value
				scene_->m_bg->m_portals[(Directions)i]->m_max_unlocked_hazard_level = 0;
			}
		}
	}
}

void InGameState::RespawnInLastHub()
{
	//cleaning layers
	(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
	(*CurrentGame).garbageLayer(LayerType::EnemyFireLayer);
	(*CurrentGame).garbageLayer(LayerType::EnemyObjectLayer);
	(*CurrentGame).garbageLayer(LayerType::ExplosionLayer);
	(*CurrentGame).garbageLayer(LayerType::BotLayer);
	(*CurrentGame).garbageLayer(LayerType::LootLayer);

	//loading last visited hub
	string nextScene_filename = (*CurrentGame).playerShip->m_respawnSceneName;
	m_nextScene = new Scene(nextScene_filename, 0, false, true);
	UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);

	//Wiping the previous background and swapping with the new one
	m_currentScene->DestroyScene();
	delete m_currentScene;
	m_currentScene = m_nextScene;
	m_nextScene = NULL;
	(*CurrentGame).m_direction = m_currentScene->m_direction;

	//Applying hub modifiers to gameplay
	(*CurrentGame).playerShip->m_disabledHyperspeed = true;
	(*CurrentGame).playerShip->m_disable_fire = true;
	m_IG_State = HUB_ROAMING;

	//resetting ship
	(*CurrentGame).playerShip->Respawn();
	(*CurrentGame).playerShip->setPosition(sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_X_RATIO));
}