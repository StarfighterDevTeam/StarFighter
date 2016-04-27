#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//creating new ship
	m_playerShip = FileLoader::LoadShipConfig("default");
	(*CurrentGame).SetPlayerShip(m_playerShip);

	//initializing HUD
	LOGGER_WRITE(Logger::DEBUG, "Initializing HUD...");
	m_playerShip->m_SFHudPanel = (SFPanel*)(new SFHUDPanel(sf::Vector2f(SCENE_SIZE_X / 3, SCENE_SIZE_Y), m_playerShip));
	(*CurrentGame).addToFeedbacks(m_playerShip->m_SFHudPanel);

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
		m_playerShip->m_money = STARTING_MONEY;
		Ship::SavePlayerMoney(MONEY_SAVE_FILE, m_playerShip);
	}

	m_playerShip->Init();
	m_playerShip->ResplenishHealth();
	LOGGER_WRITE(Logger::DEBUG, "Playership loaded\n");

	//Update HUD items
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_playerShip->m_equipment[i] != NULL)
		{
			GameObject* capsule = Ship::CloneEquipmentIntoGameObject(m_playerShip->m_equipment[i]);
			m_playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->setCellPointerForIntIndex(i, capsule);
		}
	}
	if (m_playerShip->m_weapon)
	{
		GameObject* capsule = Ship::CloneWeaponIntoGameObject(m_playerShip->m_weapon);
		m_playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->setCellPointerForIntIndex(NBVAL_Equipment, capsule);
	}
	LOGGER_WRITE(Logger::DEBUG, "HUD initialization completed\n");

	//Load knownScenes, hazard levels and current scene from save file
	if (!LoadPlayerSave(player.m_save_file).empty())
	{
		player.m_currentSceneFile = LoadPlayerSave(player.m_save_file);
	}
	else
	{
		//New game save
		player.m_currentSceneFile = STARTING_SCENE;
		AddToKnownScenes(player.m_currentSceneFile);
		SavePlayer(PLAYER_SAVE_FILE);
		//UpdateShipConfig(m_playerShip, "intro");
		m_playerShip->m_SFHudPanel->GetGrid(false, Trade_StashGrid)->ClearGrid();
		Ship::SaveItems(ITEMS_SAVE_FILE, m_playerShip);
	}

	m_playerShip->m_currentScene_name = player.m_currentSceneFile;
	m_currentScene = NULL;

	//Loading all scenes
	LoadAllScenes(SCENES_FILE);

	//Loading all enemies
	LoadAllEnemies(ENEMY_FILE);

	//Creating current scene
	m_nextScene = NULL;
	SpawnInScene(m_playerShip->m_currentScene_name);

	(*CurrentGame).SetLayerRotation(FakeShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).SetLayerRotation(BotLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).SetLayerRotation(FeedbacksLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);
}

void InGameState::UpdateShipConfig(Ship* ship, string config_name)
{
	if (!ship || config_name.empty())
		return;

	LOGGER_WRITE(Logger::DEBUG, "Loading ship config file");
	try
	{
		vector<vector<string> > shipConfig = *(FileLoaderUtils::FileLoader(SHIP_FILE));

		for (std::vector<vector<string> >::iterator it = (shipConfig).begin(); it != (shipConfig).end(); it++)
		{
			if ((*it)[SHIPCONFIG_NAME].compare(config_name) == 0)
			{
				//Clear equipped items
				ship->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->ClearGrid();

				//Loading equipment
				for (int i = 0; i < NBVAL_Equipment; i++)
				{
					if ((*it)[i + 1].compare("0") != 0)
					{
						ship->setShipEquipment(FileLoader::LoadEquipment((*it)[i + 1]), true);

						GameObject* capsule = Ship::CloneEquipmentIntoGameObject(ship->m_equipment[i]);
						ship->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->insertObject(*capsule, i);
					}
					else if (ship->m_equipment[i])
					{
						delete ship->m_equipment[i];
						ship->m_equipment[i] = NULL;
					}
				}

				//Loading weapon
				if ((*it)[SHIPCONFIG_WEAPON].compare("0") != 0)
				{
					ship->setShipWeapon(FileLoader::LoadWeapon((*it)[SHIPCONFIG_WEAPON], -1, FileLoader::LoadAmmo((*it)[SHIPCONFIG_AMMO])), true);

					GameObject* capsule = Ship::CloneWeaponIntoGameObject(ship->m_weapon);
					ship->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->insertObject(*capsule, NBVAL_Equipment);
				}
				else if (ship->m_weapon)
				{
					delete ship->m_weapon;
					ship->m_weapon = NULL;
				}

				return;
			}
		}
	}
	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::LERROR, ex.what());
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ship config '%s'. Please check the config file", (char*)config_name.c_str()));
}

void InGameState::Update(Time deltaTime)
{
	//debug command
	#ifndef NDEBUG
		if (InputGuy::spawnInSandbox() && !(*CurrentGame).m_waiting_for_dialog_validation)
		{
			SpawnInScene("Sandbox", (*CurrentGame).m_playerShip);
		}

		if (InputGuy::reloadCSVs())
		{
			(*CurrentGame).m_enemiesConfig.clear();
			LoadAllEnemies(ENEMY_FILE);
		}
	#endif

	//automatic respawn if dead
	if (!(*CurrentGame).m_playerShip->m_visible)
	{
		RespawnInLastSafePoint();
		//SpawnInScene((*CurrentGame).m_playerShip->m_respawnSceneName);
	}
	if (!(*CurrentGame).m_playerShip->m_is_asking_teleportation.empty())
	{
		SpawnInScene((*CurrentGame).m_playerShip->m_is_asking_teleportation);
		(*CurrentGame).m_playerShip->m_is_asking_teleportation = "";
	}

	//Scene dialogs
	if (m_currentScene && !m_currentScene->m_dialogs.empty())
	{
		CheckScriptedDialogs();
	}

	//State machine
	InGameStateMachineCheck(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).m_playerShip->getShipBeastScore(), m_currentScene->getSceneBeastScore(m_currentScene->getSceneHazardLevelValue()));

	(*CurrentGame).updateScene(deltaTime);

	//Create and destroy HUD panels
	//case 1: destroying a panel
	if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel == SFPanel_None && (*CurrentGame).m_playerShip->m_SFTargetPanel)
	{
		DestroySFPanel((*CurrentGame).m_playerShip);
	}
	else if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel != SFPanel_None)
	{
		//case 2: creating a panel
		if (!(*CurrentGame).m_playerShip->m_SFTargetPanel)
		{
			CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
		}
		//case 3: changing panel
		else if ((*CurrentGame).m_playerShip->m_SFTargetPanel->m_panel_type != (*CurrentGame).m_playerShip->m_is_asking_SFPanel)
		{
			DestroySFPanel((*CurrentGame).m_playerShip);
			CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
		}
	}
	
	this->mainWindow->clear();
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
	//(*CurrentGame).drawHud();
}

void InGameState::Release()
{
	//TODO
}

bool InGameState::AddToKnownScenes(string scene_name, Ship* playerShip)
{
	if (!playerShip)
	{
		return false;
	}

	playerShip->m_currentScene_name = scene_name;
	map<string, int>::iterator it = playerShip->m_knownScenes.find(scene_name);

	//if scene not already known
	if (it == playerShip->m_knownScenes.end())
	{
		//add it to the map of known scenes
		playerShip->m_knownScenes.insert(pair<string, int>(scene_name, 0));
		LOGGER_WRITE(Logger::DEBUG, TextUtils::format("Adding '%s' to known scenes.\n", (char*)scene_name.c_str()));
		return true;
	}
	return false;
}

void InGameState::SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level, Ship* playerShip)
{
	if (!playerShip)
	{
		return;
	}

	map<string, int>::iterator it = playerShip->m_knownScenes.find(scene_name);
	if (hazard_level > NB_HAZARD_LEVELS - 1)
	{
		hazard_level = NB_HAZARD_LEVELS - 1;
	}
	if (it != playerShip->m_knownScenes.end())
	{
		playerShip->m_knownScenes[scene_name] = hazard_level;
	}
}

int InGameState::GetSceneHazardLevelUnlocked(string scene_name, Ship* playerShip)
{
	if (!playerShip)
	{
		return -1;
	}

	map<string, int>::iterator it = playerShip->m_knownScenes.find(scene_name);

	//if scene is known
	if (it != playerShip->m_knownScenes.end())
	{
		return playerShip->m_knownScenes[scene_name];
	}
	//else
	return 0;
}

int InGameState::SavePlayer(string file, Ship* playerShip)
{
	if (!playerShip)
	{
		return -1;
	}

	LOGGER_WRITE(Logger::DEBUG, "Saving known scenes and current scene in profile.\n");

	ofstream data(file.c_str(), ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (map<string, int>::iterator it = playerShip->m_knownScenes.begin(); it != playerShip->m_knownScenes.end(); it++)
		{
			data << it->first.c_str() << " " << it->second;
			if (it->first.c_str() == playerShip->m_respawnSceneName)
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

string InGameState::LoadPlayerSave(string file, Ship* playerShip)
{
	if (!playerShip)
	{
		return "";
	}

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

			playerShip->m_knownScenes.insert(std::pair<string, int>(scene, level));
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
	float w_ = (*CurrentGame).m_playerShip->m_size.x;
	float h_ = (*CurrentGame).m_playerShip->m_size.y;

	switch (m_IG_State)
	{
		case SCROLLING:
		{
			if (m_currentScene->m_generating_enemies)
			{
				m_currentScene->GenerateEnemiesv2(deltaTime);
			}
			//Scrolling until the background reaches its end
			if ((*CurrentGame).m_direction == NO_DIRECTION)
			{
				m_IG_State = LAST_SCREEN;
			}
			else
			{
				//when last screen is reached
				if (m_currentScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2)) == GREATER_THAN
					|| m_currentScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2)) == EQUAL_TO)
				{
					//Correct the position
					m_currentScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2));
					m_currentScene->m_bg->m_speed = sf::Vector2f(0, 0);
					(*CurrentGame).m_vspeed = 0;

					//Disable hyperspeed capacity
					(*CurrentGame).m_playerShip->m_disableHyperspeed = true;
					(*CurrentGame).m_playerShip->m_disableRecall = true;

					//Stop spawning enemies
					m_currentScene->m_generating_enemies = false;
					if (m_currentScene->m_generating_boss)
					{
						m_bossSpawnCountdown.restart();
					}
					
					m_hasDisplayedDestructionRatio = false;
					m_IG_State = LAST_SCREEN;

					//Wipe out enemies that were spawned offscren
					(*CurrentGame).garbageLayer(EnemyObjectLayer, true);
				}
			}

			break;
		}

		case LAST_SCREEN:
		{	
			//Optional script to skip boss procedures, for scripted missions
			if (m_currentScene->m_scripts[SceneScript_PortalOpenDuringBoss])
			{
				if (m_currentScene->m_generating_boss)
				{
					if (m_bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
					{
						m_currentScene->GenerateBoss();
						m_currentScene->m_generating_boss = false;
						(*CurrentGame).m_curMusic_type = Music_Boss;//but we don't actually play Music_Boss so the scripted music is not overriden, because it's not really a boss, since you can pass it
					}
				}
				
				if ((*CurrentGame).isLastEnemyDead())
				{
					(*CurrentGame).m_playerShip->RegenHealthFast(deltaTime, false, true, false);
					(*CurrentGame).m_playerShip->m_disableSlowmotion = true;
					(*CurrentGame).m_playerShip->m_disableRecall = false;
				}

				//player takes exit?
				if ((*CurrentGame).m_playerShip->m_is_asking_scene_transition)
				{
					PlayerTakesExit();
				}
			}

			//When enemies, loots and enemy bullets on scene are dead, we can start the transition to the next scene
			else if ((*CurrentGame).isLastEnemyDead())
			{
				//scene generates boss? (or boss is dead, eitherway)
				if (m_currentScene->m_generating_boss)
				{
					m_currentScene->m_bg->SetPortalsState(PortalInvisible);
					if (m_bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
					{
						(*CurrentGame).PlayMusic(Music_Boss);
						m_currentScene->GenerateBoss();
						m_IG_State = BOSS_FIGHT;
					}
				}
				else
				{
					(*CurrentGame).m_playerShip->RegenHealthFast(deltaTime, false, true, false);
					(*CurrentGame).m_playerShip->m_disableSlowmotion = true;
					(*CurrentGame).m_playerShip->m_disableRecall = false;

					if (!m_hasDisplayedDestructionRatio)
					{
						//is the scene capable of hazard break? (= last scene before hub)
						if (m_currentScene->m_canHazardBreak)
						{
							//what is our destruction ratio? (displaying score). 100% = Hazard break
							if (m_currentScene->CheckHazardBreakConditions() && m_currentScene->getSceneHazardLevelUnlockedValue() == m_currentScene->getSceneHazardLevelValue())
							{
								m_currentScene->DisplayScore(true);
								m_currentScene->HazardBreak();
							}
							else
							{
								m_currentScene->DisplayScore();
							}
						}

						m_currentScene->m_bg->SetPortalsState(PortalOpen);

						m_hasDisplayedDestructionRatio = true;
					}
				}

				//player takes exit?
				if ((*CurrentGame).m_playerShip->m_is_asking_scene_transition)
				{
					PlayerTakesExit();
				}
			}
			//clearing enemies that have spawned out of the scene size
			else
			{
				if (m_currentScene->m_generating_boss)
				{
					m_bossSpawnCountdown.restart();
				}
				m_currentScene->m_bg->SetPortalsState(PortalClose);
			}

			break;
		}

		case BOSS_FIGHT:
		{
			//is boss dead?
			if ((*CurrentGame).isLastEnemyDead())
			{
				m_currentScene->m_generating_boss = false;
				m_IG_State = LAST_SCREEN;
			}

			break;
		}

		case TRANSITION_PHASE1_2:
		{
			//When the playership reaches the scene border, we can start the swapping of scenes, while replacing him on the right starting position for the next scene
			if ((*CurrentGame).m_playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2)) == LESSER_THAN
				|| (*CurrentGame).m_playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2)) == EQUAL_TO)
			{
				//Correction of playership position
				(*CurrentGame).m_playerShip->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2));

				(*CurrentGame).m_playerShip->m_speed = GameObject::getSpeed_to_LocationWhileSceneSwap((*CurrentGame).m_direction, m_nextScene->m_direction, ENDSCENE_TRANSITION_SPEED_DOWN,
					(*CurrentGame).m_playerShip->getPosition());

				m_currentScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).m_vspeed = ENDSCENE_TRANSITION_SPEED_DOWN;
				m_nextScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).garbageLayer(FriendlyFireLayer);
				(*CurrentGame).garbageLayer(LootLayer);
				(*CurrentGame).garbageLayer(FeedbacksLayer);

				//Optional script to skip boss procedures, for scripted missions, so they may still be alive at this point
				if (m_currentScene->m_scripts[SceneScript_PortalOpenDuringBoss])
				{
					(*CurrentGame).SetLayerSpeed(EnemyObjectLayer, m_nextScene->m_bg->m_speed);
				}

				m_IG_State = TRANSITION_PHASE2_2;
			}

			break;
		}

		case TRANSITION_PHASE2_2:
		{
			float wn = m_nextScene->m_bg->m_size.x;
			float hn = m_nextScene->m_bg->m_size.y;
			//When the new scene is completely swapped, we can wrap up the replacement and restart scrolling (or do what the Hubs do if the scene is a Hub)
			if (m_nextScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == GREATER_THAN
				|| m_nextScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == EQUAL_TO)
			{

				//Correction of the scenes position
				m_currentScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f((w / 2), (h / 2)));

				m_nextScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2)));
				m_nextScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling(m_nextScene->m_direction, m_nextScene->m_vspeed);
				(*CurrentGame).m_vspeed = m_nextScene->m_vspeed;

				if (m_nextScene->m_direction == NO_DIRECTION)
				{
					m_IG_State = HUB_ROAMING;
					(*CurrentGame).m_playerShip->m_disableHyperspeed = true;
					(*CurrentGame).m_playerShip->m_disableSlowmotion = true;
					(*CurrentGame).m_playerShip->m_disable_bots = true;
					(*CurrentGame).m_playerShip->m_disableRecall = true;
					(*CurrentGame).m_playerShip->SetBotsVisibility(false);

					(*CurrentGame).resetHazard();
				}
				else
				{
					m_IG_State = SCROLLING;
					(*CurrentGame).m_playerShip->m_disable_fire = false;
					(*CurrentGame).m_playerShip->m_disableHyperspeed = false;
					(*CurrentGame).m_playerShip->m_disableSlowmotion = false;
					(*CurrentGame).m_playerShip->m_disable_bots = false;
					(*CurrentGame).m_playerShip->m_disableRecall = false;
					(*CurrentGame).m_playerShip->SetBotsVisibility(true);
					(*CurrentGame).m_playerShip->RotateShip(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));

					//(*CurrentGame).SetLayerRotation(BotLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					//(*CurrentGame).SetLayerRotation(FeedbacksLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
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
				Directions previous_direction = m_currentScene->m_direction;
				delete m_currentScene;
				m_currentScene = m_nextScene;
				m_nextScene = NULL;
				(*CurrentGame).m_direction = m_currentScene->m_direction;
				(*CurrentGame).m_vspeed = m_currentScene->m_vspeed;
				m_playerShip->m_currentScene_name = m_currentScene->m_name;
				m_playerShip->m_currentScene_hazard = m_currentScene->getSceneHazardLevelValue();

				//Save scenes
				AddToKnownScenes(m_currentScene->m_name);
				if (m_currentScene->m_direction == NO_DIRECTION)
				{
					m_playerShip->m_respawnSceneName = m_currentScene->m_name;
					SavePlayer(PLAYER_SAVE_FILE);
				}
				else
				{
					SavePlayer(PLAYER_SAVE_FILE);
				}

				//Giving control back to the player
				(*CurrentGame).m_playerShip->m_disable_inputs = false;
				(*CurrentGame).m_playerShip->m_speed = sf::Vector2f(0, 0);

				(*CurrentGame).m_waiting_for_scene_transition = false;
				(*CurrentGame).m_playerShip->m_immune = false;

				//Play scene title feedback if we come from a hub + music changes
				if (previous_direction == NO_DIRECTION || (*CurrentGame).m_curMusic_type == Music_Hub)
				{
					m_currentScene->PlayTitleFeedback();

					if (!m_currentScene->m_scene_music.empty())
					{
						(*CurrentGame).PlayMusic(Music_Scene, m_currentScene->m_scene_music);
					}
					else
					{
						(*CurrentGame).PlayMusic(Music_Scene);
					}
				}
				if (m_currentScene->m_direction == NO_DIRECTION)
				{
					(*CurrentGame).PlayMusic(Music_Hub);
				}
			}

			break;
		}

		case HUB_ROAMING:
		{
			m_currentScene->m_bg->SetPortalsState(PortalOpen);
			
			(*CurrentGame).m_playerShip->RegenHealthFast(deltaTime, true, true, true);

			//player takes exit?
			if ((*CurrentGame).m_playerShip->m_is_asking_scene_transition)
			{
				m_currentScene->m_bg->SetPortalsState(PortalGhost);

				(*CurrentGame).PlaySFX(SFX_EnteringPortal);

				bool reverse = false;
				if ((*CurrentGame).m_playerShip->m_targetPortal->m_direction == DIRECTION_DOWN || (*CurrentGame).m_playerShip->m_targetPortal->m_direction == DIRECTION_LEFT)
				{
					reverse = true;
				}
				string nextScene_filename = (*CurrentGame).m_playerShip->m_targetPortal->m_destination_name;

				(*CurrentGame).m_direction = (*CurrentGame).m_playerShip->m_targetPortal->m_direction;
				m_nextScene = new Scene(nextScene_filename, (*CurrentGame).m_playerShip->m_SFTargetPanel->GetSelectedOptionIndex(), reverse, false);
				(*CurrentGame).m_playerShip->m_last_hazard_level_played = (*CurrentGame).m_playerShip->m_SFTargetPanel->GetSelectedOptionIndex();
				UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);
				m_nextScene->m_bg->m_speed = sf::Vector2f(0, 0);

				//Putting the player on rails
				(*CurrentGame).m_playerShip->m_disable_inputs = true;
				(*CurrentGame).m_playerShip->m_disable_fire = true;
				(*CurrentGame).m_playerShip->m_speed = -GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_UP);
				
				m_IG_State = TRANSITION_PHASE1_2;
			}

			break;
		}
	}
}

void InGameState::UpdatePortalsMaxUnlockedHazardLevel(Scene* scene, Ship* playerShip)
{
	//getting the max hazard value for the upcoming scene
	map<string, int>::iterator it = playerShip->m_knownScenes.find(scene->m_name);
	if (it != playerShip->m_knownScenes.end())
	{
		scene->setSceneHazardLevelUnlockedValue(playerShip->m_knownScenes[scene->m_name]);
	}
	else
	{
		//destination is not know yet -> default max hazard value
		scene->setSceneHazardLevelUnlockedValue(0);
	}

	//loading the scene's portals with the info about their respective max hazard values
	for (int i = 0; i < NO_DIRECTION; i++)
	{
		if (scene->m_bg->m_portals[(Directions)i] != NULL)
		{
			map<string, int>::iterator it = playerShip->m_knownScenes.find(scene->m_bg->m_portals[(Directions)i]->m_destination_name);
			if (it != playerShip->m_knownScenes.end())
			{
				scene->m_bg->m_portals[(Directions)i]->m_max_unlocked_hazard_level = playerShip->m_knownScenes[scene->m_bg->m_portals[(Directions)i]->m_destination_name];
			}
			else
			{
				//destination is not know yet -> default max hazard value
				scene->m_bg->m_portals[(Directions)i]->m_max_unlocked_hazard_level = 0;
			}
		}
	}
}

void InGameState::RespawnInLastSafePoint()
{
	//cleaning layers
	(*CurrentGame).garbageLayer(FriendlyFireLayer);
	(*CurrentGame).garbageLayer(EnemyFireLayer);
	(*CurrentGame).garbageLayer(EnemyObjectLayer);
	(*CurrentGame).garbageLayer(ExplosionLayer);
	(*CurrentGame).garbageLayer(LootLayer);

	//loading last visited hub
	if ((*CurrentGame).m_playerShip->m_respawnSceneName.empty())
	{
		(*CurrentGame).m_playerShip->m_respawnSceneName = STARTING_SCENE;
	}
	SpawnInScene((*CurrentGame).m_playerShip->m_respawnSceneName);

	//resetting ship
	(*CurrentGame).m_playerShip->Respawn();
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
		case SFPanel_Trade:
		{
			playerShip->m_SFTargetPanel = new SFTradePanel(sf::Vector2f(TRADE_PANEL_WIDTH, TRADE_PANEL_HEIGHT), playerShip);//new SFInventoryPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INVENTORY_PANEL_HEIGHT), playerShip, SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS, false, true);
			break;
		}
		case SFPanel_Action:
		{
			playerShip->m_SFTargetPanel = new SFOneActionPanel(sf::Vector2f(ACTION_PANEL_WIDTH, ACTION_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_Portal:
		{
			playerShip->m_SFTargetPanel = new SFPortalPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INTERACTION_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_Shop:
		{
			playerShip->m_SFTargetPanel = new SFShopPanel(sf::Vector2f(SHOP_PANEL_WIDTH, SHOP_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_DialogNext:
		{
			playerShip->m_is_asking_SFPanel = SFPanel_Dialog;
		}
		case SFPanel_Dialog:
		{
			playerShip->m_SFTargetPanel = new SFDialogPanel(sf::Vector2f(DIALOG_PANEL_WIDTH, DIALOG_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_Map:
		{
			playerShip->m_SFTargetPanel = new SFMapPanel(sf::Vector2f(STELLARMAP_PANEL_WIDTH, STELLARMAP_PANEL_HEIGHT), playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::SpawnInScene(string scene_name, Ship* playerShip)
{
	if (playerShip)
	{
		if (m_currentScene)
		{
			delete m_currentScene;
		}
		if (m_nextScene)
		{
			delete m_nextScene;
			m_nextScene = NULL;
		}
		//cleaning layers
		(*CurrentGame).garbageLayer(FriendlyFireLayer);
		(*CurrentGame).garbageLayer(EnemyFireLayer);
		(*CurrentGame).garbageLayer(EnemyObjectLayer);
		(*CurrentGame).garbageLayer(ExplosionLayer);
		(*CurrentGame).garbageLayer(LootLayer);
		(*CurrentGame).garbageLayer(FeedbacksLayer);
		m_currentScene = new Scene(scene_name, 0, false, true);
		playerShip->m_currentScene_name = m_currentScene->m_name;
		playerShip->m_currentScene_hazard = m_currentScene->getSceneHazardLevelValue();
		
		//speed
		(*CurrentGame).m_vspeed = m_currentScene->m_vspeed;

		//rotation
		(*CurrentGame).m_playerShip->RotateShip(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));

		//position
		sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_X_RATIO);
		if ((*CurrentGame).m_direction != NO_DIRECTION)
		{
			(*CurrentGame).m_playerShip->m_disable_fire = false;
			(*CurrentGame).m_playerShip->m_disableHyperspeed = false;
			(*CurrentGame).m_playerShip->m_disableSlowmotion = false;
			(*CurrentGame).m_playerShip->m_disable_bots = false;
			(*CurrentGame).m_playerShip->m_disableRecall = false;
			(*CurrentGame).m_playerShip->SetBotsVisibility(true);
			m_IG_State = SCROLLING;
			ship_pos = GameObject::getPosition_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO));

			if (!m_currentScene->m_scene_music.empty())
			{
				(*CurrentGame).PlayMusic(Music_Scene, m_currentScene->m_scene_music);
			}
			else
			{
				(*CurrentGame).PlayMusic(Music_Scene);
			}
		}
		else
		{
			(*CurrentGame).m_playerShip->m_disable_fire = true;
			(*CurrentGame).m_playerShip->m_disableHyperspeed = true;
			(*CurrentGame).m_playerShip->m_disableSlowmotion = true;
			(*CurrentGame).m_playerShip->m_disable_bots = true;
			(*CurrentGame).m_playerShip->m_disableRecall = true;
			(*CurrentGame).m_playerShip->SetBotsVisibility(false);
			m_IG_State = HUB_ROAMING;
			playerShip->m_respawnSceneName = m_currentScene->m_name;

			(*CurrentGame).resetHazard();

			if (!m_currentScene->m_scene_music.empty())
			{
				(*CurrentGame).PlayMusic(Music_Hub, m_currentScene->m_scene_music);
			}
			else
			{
				(*CurrentGame).PlayMusic(Music_Hub);
			}
		}
		m_playerShip->setPosition(ship_pos);

		UpdatePortalsMaxUnlockedHazardLevel(m_currentScene);

		SavePlayer(PLAYER_SAVE_FILE);
	}
}

void InGameState::LoadAllScenes(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all scenes scripts.");

	vector<vector<string> > generalScenesConfig = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allScenesVectorSize = generalScenesConfig.size();
	for (size_t i = 0; i < allScenesVectorSize; i++)
	{
		(*CurrentGame).m_generalScenesConfig.insert(std::map<string, vector<string> >::value_type(generalScenesConfig[i][SCENE_NAME], generalScenesConfig[i]));
	}

	for (size_t j = 0; j < allScenesVectorSize; j++)
	{
		if (!generalScenesConfig[j][SCENE_FILENAME].empty() && generalScenesConfig[j][SCENE_FILENAME].compare("0") != 0)
		{
			(*CurrentGame).m_sceneConfigs.insert(std::map<string, vector<vector<string> > >::value_type(generalScenesConfig[j][SCENE_NAME], *(FileLoaderUtils::FileLoader(generalScenesConfig[j][SCENE_FILENAME]))));
		}
	}

	generalScenesConfig.clear();

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void InGameState::LoadAllEnemies(string enemies_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all enemies.");

	vector<vector<string> > enemiesConfig = *(FileLoaderUtils::FileLoader(enemies_file));
	size_t allEnemiesVectorSize = enemiesConfig.size();
	for (size_t i = 0; i < allEnemiesVectorSize; i++)
	{
		(*CurrentGame).m_enemiesConfig.insert(std::map<string, vector<string> >::value_type(enemiesConfig[i][ENEMY_NAME], enemiesConfig[i]));
	}

	enemiesConfig.clear();

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void InGameState::PlayerTakesExit()
{
	m_currentScene->m_bg->SetPortalsState(PortalGhost);

	(*CurrentGame).PlaySFX(SFX_EnteringPortal);

	bool reverse = false;
	if ((*CurrentGame).m_playerShip->m_targetPortal->m_direction == DIRECTION_DOWN || (*CurrentGame).m_playerShip->m_targetPortal->m_direction == DIRECTION_LEFT)
	{
		reverse = true;
	}

	string nextScene_filename = (*CurrentGame).m_playerShip->m_targetPortal->m_destination_name;
	m_nextScene = new Scene(nextScene_filename, m_currentScene->getSceneHazardLevelValue(), reverse, false);

	//remembering linked scenes to hazard break later
	if (!m_currentScene->m_canHazardBreak)
	{
		m_nextScene->m_scenesLinkedToUpdate.push_back(m_currentScene->m_name);
	}
	UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);

	m_nextScene->m_bg->m_speed = sf::Vector2f(0, 0);

	//Putting the player on rails
	(*CurrentGame).m_playerShip->m_disable_inputs = true;
	(*CurrentGame).m_playerShip->m_disable_fire = true;
	(*CurrentGame).m_playerShip->m_disableSlowmotion = true;
	(*CurrentGame).m_playerShip->m_speed = -GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_UP);
	
	(*CurrentGame).m_waiting_for_scene_transition = true;

	m_IG_State = TRANSITION_PHASE1_2;
}

void InGameState::CheckScriptedDialogs()
{
	size_t dialogsVectorSize = m_currentScene->m_dialogs.size();
	for (size_t i = 0; i < dialogsVectorSize; i++)
	{
		if (m_currentScene->m_dialogs[i].first > 0 && m_currentScene->m_scene_clock.getElapsedTime().asSeconds() > m_currentScene->m_dialogs[i].first)
		{
			size_t chainedDialogsVectorSize = m_currentScene->m_dialogs[i].second.size();
			for (size_t j = 0; j < chainedDialogsVectorSize; j++)
			{
				(*CurrentGame).m_playerShip->m_targetDialogs.push_back(m_currentScene->m_dialogs[i].second[j]->Clone());
			}

			m_currentScene->m_dialogs[i].first = -1;//flag it as read
			break;
		}
	}
}