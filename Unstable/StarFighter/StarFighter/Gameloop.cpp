#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	m_currentScene = NULL;
	m_nextScene = NULL;
	m_playership = NULL;
}

void Gameloop::Initialize(Player player)
{
	mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//Loading all scenes
	LoadAllScenes(SCENES_FILE);
	//Loading all FX
	LoadAllFX(FX_FILE);
	//Loading all enemies
	LoadAllEnemies(ENEMY_FILE);
	//Loading all upgrades
	LoadAllUpgrades(UPGRADES_FILE);

	//creating new ship
	m_playership = FileLoader::LoadShipConfig("default");
	(*CurrentGame).SetPlayerShip(m_playership);

	//initializing HUD
	LOGGER_WRITE(Logger::DEBUG, "Initializing HUD...");
	m_playership->m_SFHudPanel = (SFPanel*)(new SFHUDPanel(sf::Vector2f(SCENE_SIZE_X / 3, SCENE_SIZE_Y), m_playership));
	(*CurrentGame).addToPanels(m_playership->m_SFHudPanel);
	LOGGER_WRITE(Logger::DEBUG, "HUD initialization completed\n");

	//Initialisation of player ship
	m_playership->Init();
	m_playership->ResplenishHealth();
	LOGGER_WRITE(Logger::DEBUG, "Playership loaded\n");

	//Loading save files or creating new ones
	m_playership->m_respawnSceneName = Ship::LoadPlayerScenes(m_playership);
	if (m_playership->m_respawnSceneName.empty() == false)
	{
		if (!Ship::LoadPlayerUpgrades(m_playership))
			Ship::SavePlayerUpgrades(m_playership);

		if (!Ship::LoadPlayerMoneyAndHealth(m_playership))
			Ship::SavePlayerMoneyAndHealth(m_playership);

		SpawnInScene(m_playership->m_respawnSceneName, m_playership, false);
	}
	else
	{
		//New game save
		(*CurrentGame).m_playership->Respawn(true);

		//Loading starting scene
		SpawnInScene(STARTING_SCENE, (*CurrentGame).m_playership, false);
	}

	(*CurrentGame).addToScene((*CurrentGame).m_playership, false);

	//DEBUG
	//SpawnInScene("Sandbox", (*CurrentGame).m_playership);
}

void Gameloop::UpdateShipConfig(Ship* ship, string config_name)
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
				ship->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->ClearGrid();

				//Loading equipment
				for (int i = 0; i < NBVAL_Equipment; i++)
				{
					if ((*it)[i + 1].compare("0") != 0)
					{
						ship->setShipEquipment(FileLoader::LoadEquipment((*it)[i + 1]), true);

						GameObject* capsule = Enemy::CloneEquipmentIntoGameObject(ship->m_equipment[i]);
						ship->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(capsule, i, false);
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
					ship->setShipWeapon(FileLoader::LoadWeapon((*it)[SHIPCONFIG_WEAPON], -1), true);

					GameObject* capsule = Enemy::CloneWeaponIntoGameObject(ship->m_weapon);
					ship->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->InsertObject(capsule, NBVAL_Equipment, false);
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

void Gameloop::Update(Time deltaTime)
{
	//automatic respawn if dead
	if ((*CurrentGame).m_playership->m_visible == false && (*CurrentGame).m_playership->m_collision_timer <= 0)
	{
		//resetting ship
		(*CurrentGame).m_playership->Respawn(false);

		//Loading starting scene
		SpawnInScene(STARTING_SCENE, (*CurrentGame).m_playership, false);
	}
	if (!(*CurrentGame).m_playership->m_is_asking_teleportation.empty())
	{
		SpawnInScene((*CurrentGame).m_playership->m_is_asking_teleportation, (*CurrentGame).m_playership, true);
		(*CurrentGame).m_playership->m_is_asking_teleportation = "";
	}

	//Scene dialogs
	if (m_currentScene && !m_currentScene->m_dialogs.empty())
	{
		CheckScriptedDialogs();
	}

	//Get "beast score" bonus
	//(*CurrentGame).GetBeastScoreBonus((*CurrentGame).m_playership->m_combo_level * 0.01 * MAX_BEAST_SCALE, m_currentScene->getSceneBeastScore(m_currentScene->getSceneHazardLevelValue()));
	//(*CurrentGame).GetBeastScoreBonus((*CurrentGame).m_playership->getShipBeastScore(), m_currentScene->getSceneBeastScore(m_currentScene->getSceneHazardLevelValue()));

	//Update scene
	(*CurrentGame).updateScene(deltaTime);

	//State machine
	InGameStateMachineCheck(deltaTime);

	//Create and destroy HUD panels
	//case 1: destroying a panel
	if ((*CurrentGame).m_playership->m_is_asking_SFPanel == SFPanel_None && (*CurrentGame).m_playership->m_SFTargetPanel)
	{
		DestroySFPanel((*CurrentGame).m_playership);
	}
	else if ((*CurrentGame).m_playership->m_is_asking_SFPanel != SFPanel_None)
	{
		//case 2: creating a panel
		if (!(*CurrentGame).m_playership->m_SFTargetPanel)
		{
			CreateSFPanel((*CurrentGame).m_playership->m_is_asking_SFPanel, (*CurrentGame).m_playership);
		}
		//case 3: changing panel
		else if ((*CurrentGame).m_playership->m_SFTargetPanel->m_panel_type != (*CurrentGame).m_playership->m_is_asking_SFPanel)
		{
			DestroySFPanel((*CurrentGame).m_playership);
			CreateSFPanel((*CurrentGame).m_playership->m_is_asking_SFPanel, (*CurrentGame).m_playership);
		}
	}

	//debug command
#ifndef NDEBUG
	//F6: spawn in Sandbox scene
	if (InputGuy::spawnInSandbox() && (*CurrentGame).m_waiting_for_dialog_validation == false)
	{
		SpawnInScene("Sandbox", (*CurrentGame).m_playership, false);
		(*CurrentGame).m_playership->ResplenishHealth();
	}

	//F7: relad saved items and enemy config
	if (InputGuy::reloadCSVs())
	{
		(*CurrentGame).m_enemiesConfig.clear();
		LoadAllEnemies(ENEMY_FILE);

		Ship::LoadPlayerItems(m_playership);
		Ship::LoadPlayerMoneyAndHealth(m_playership);
	}
#endif
	
	this->mainWindow->clear();
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
	//(*CurrentGame).drawHud();
}

void Gameloop::Release()
{
	//TODO
}

bool Gameloop::AddToKnownScenes(string scene_name, Ship* playership)
{
	if (playership == NULL)
		return false;

	playership->m_currentScene_name = scene_name;
	map<string, int>::iterator it = playership->m_knownScenes.find(scene_name);

	//if scene not already known
	if (it == playership->m_knownScenes.end())
	{
		//add it to the map of known scenes
		playership->m_knownScenes.insert(pair<string, int>(scene_name, 0));
		LOGGER_WRITE(Logger::DEBUG, TextUtils::format("Adding '%s' to known scenes.\n", (char*)scene_name.c_str()));
		return true;
	}
	return false;
}

void Gameloop::SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level, Ship* playership)
{
	if (!playership)
	{
		return;
	}

	map<string, int>::iterator it = playership->m_knownScenes.find(scene_name);
	if (hazard_level > NB_HAZARD_LEVELS - 1)
	{
		hazard_level = NB_HAZARD_LEVELS - 1;
	}
	if (it != playership->m_knownScenes.end())
	{
		playership->m_knownScenes[scene_name] = hazard_level;
	}
}

int Gameloop::GetSceneHazardLevelUnlocked(string scene_name, Ship* playership)
{
	if (!playership)
	{
		return -1;
	}

	map<string, int>::iterator it = playership->m_knownScenes.find(scene_name);

	//if scene is known
	if (it != playership->m_knownScenes.end())
	{
		return playership->m_knownScenes[scene_name];
	}
	//else
	return 0;
}

void Gameloop::InGameStateMachineCheck(sf::Time deltaTime)
{
	float w = m_currentScene->m_bg->m_size.x;
	float h = m_currentScene->m_bg->m_size.y;

	if (m_IG_State == SCROLLING)
	{
		if (m_currentScene->m_generating_enemies == true)
			m_currentScene->GenerateEnemiesv2(deltaTime);
			
		//When last screen is reached
		if (m_currentScene->m_bg->getPosition().y >= 0.5 * h)
		{
			//Correct the position
			m_currentScene->m_bg->setPosition(sf::Vector2f(m_currentScene->m_bg->getPosition().x, 0.5 * h));
			m_currentScene->m_bg->m_speed = sf::Vector2f(0, 0);
			(*CurrentGame).m_vspeed = 0;

			//Disable hyperspeed capacity
			(*CurrentGame).m_playership->m_disableHyperspeed = true;

			//Stop spawning enemies
			m_currentScene->m_generating_enemies = false;
			if (m_currentScene->m_generating_boss == true)
				m_bossSpawnCountdown.restart();
					
			m_hasDisplayedDestructionRatio = false;

			//Wipe out enemies that were spawned offscren
			(*CurrentGame).garbageLayer(EnemyObjectLayer, true, true);

			//Switch state
			m_IG_State = LAST_SCREEN;
		}
	}
	
	if (m_IG_State == LAST_SCREEN)
	{	
		//Optional script to skip boss procedures, for scripted missions
		if (m_currentScene->m_scripts[SceneScript_PortalOpenDuringBoss] == true)
		{
			if (m_currentScene->m_generating_boss == true)
			{
				if (m_bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
				{
					m_currentScene->SpawnBoss();
					m_currentScene->m_generating_boss = false;
					(*CurrentGame).m_curMusic_type = Music_Boss;//but we don't actually play Music_Boss so the scripted music is not overriden, because it's not really a boss, since you can pass it
				}
			}
				
			if ((*CurrentGame).isLastEnemyDead() == true)
			{
				//(*CurrentGame).m_playership->RegenHealthFast(deltaTime, false, true, false);
				(*CurrentGame).m_playership->m_disableSlowmotion = true;
			}

			//player takes exit?
			if ((*CurrentGame).m_playership->m_is_asking_scene_transition == true)
				PlayerTakesExit();
		}
		//When enemies, loots and enemy bullets on scene are dead, we can start the transition to the next scene
		else if ((*CurrentGame).isLastEnemyDead() == true)
		{
			//scene generates boss? (or boss is dead, eitherway)
			if (m_currentScene->m_generating_boss == true)
			{
				m_currentScene->m_bg->SetPortalsState(PortalInvisible);
				if (m_bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
				{
					(*CurrentGame).PlayMusic(Music_Boss);
					m_currentScene->SpawnBoss();
					m_IG_State = BOSS_FIGHT;
				}
			}
			else
			{
				//(*CurrentGame).m_playership->RegenHealthFast(deltaTime, false, true, false);
				(*CurrentGame).m_playership->m_disableSlowmotion = true;

				if (m_hasDisplayedDestructionRatio == false)
				{
					//is the scene capable of hazard break? (= last scene before hub)
					if (m_currentScene->m_canHazardBreak == true)
					{
						//what is our destruction ratio? (displaying score). 100% = Hazard break
						if (m_currentScene->CheckHazardBreakConditions() == true && m_currentScene->getSceneHazardLevelUnlockedValue() == m_currentScene->getSceneHazardLevelValue())
						{
							m_currentScene->DisplayScore(true);
							m_currentScene->HazardBreak();
						}
						else
							m_currentScene->DisplayScore();
					}

					m_currentScene->m_bg->SetPortalsState(PortalOpen);

					m_hasDisplayedDestructionRatio = true;
				}
			}

			//player takes exit?
			if ((*CurrentGame).m_playership->m_is_asking_scene_transition == true)
				PlayerTakesExit();
		}
		//clearing enemies that have spawned out of the scene size
		else
		{
			if (m_currentScene->m_generating_boss == true)
				m_bossSpawnCountdown.restart();

			m_currentScene->m_bg->SetPortalsState(PortalClose);
		}
	}

	if (m_IG_State == BOSS_FIGHT)
	{
		//is boss dead?
		if ((*CurrentGame).isLastEnemyDead() == true)
		{
			m_currentScene->m_generating_boss = false;
			m_IG_State = LAST_SCREEN;
		}
	}

	if (m_IG_State == HUB_ROAMING)
	{
		m_currentScene->m_bg->SetPortalsState(PortalOpen);
			
		//(*CurrentGame).m_playership->RegenHealthFast(deltaTime, true, true, true);

		//player takes exit?
		if ((*CurrentGame).m_playership->m_is_asking_scene_transition == true)
		{
			PlayerTakesExit();
			/*
			m_currentScene->m_bg->SetPortalsState(PortalGhost);

			(*CurrentGame).PlaySFX(SFX_EnteringPortal);

			bool reverse = false;
			if ((*CurrentGame).m_playership->m_targetPortal->m_direction == DIRECTION_DOWN || (*CurrentGame).m_playership->m_targetPortal->m_direction == DIRECTION_LEFT)
				reverse = true;
				
			string nextScene_filename = (*CurrentGame).m_playership->m_targetPortal->m_destination_name;

			m_nextScene = new Scene(nextScene_filename, (*CurrentGame).m_playership->m_SFTargetPanel->GetSelectedOptionIndex(), reverse, false);
			(*CurrentGame).m_playership->m_last_hazard_level_played = (*CurrentGame).m_playership->m_SFTargetPanel->GetSelectedOptionIndex();
			UpdatePortalsMaxUnlockedHazardLevel(m_nextScene);
			m_nextScene->m_bg->m_speed = sf::Vector2f(0, 0);

			//Putting the player on rails
			(*CurrentGame).m_playership->m_disable_inputs = true;
			(*CurrentGame).m_playership->m_disable_fire = true;
			(*CurrentGame).m_playership->m_speed = -GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_UP);
				
			m_IG_State = SCROLLING;
			*/
		}
	}
}

void Gameloop::UpdatePortalsMaxUnlockedHazardLevel(Scene* scene, Ship* playership)
{
	//getting the max hazard value for the upcoming scene
	map<string, int>::iterator it = playership->m_knownScenes.find(scene->m_name);
	if (it != playership->m_knownScenes.end())
		scene->setSceneHazardLevelUnlockedValue(playership->m_knownScenes[scene->m_name]);
	else//destination is not know yet -> default max hazard value
		scene->setSceneHazardLevelUnlockedValue(0);
	

	//loading the scene's portals with the info about their respective max hazard values
	for (int i = 0; i < NO_DIRECTION; i++)
	{
		if (scene->m_bg->m_portals[(Directions)i] != NULL)
		{
			map<string, int>::iterator it = playership->m_knownScenes.find(scene->m_bg->m_portals[(Directions)i]->m_destination_name);
			if (it != playership->m_knownScenes.end())
				scene->m_bg->m_portals[(Directions)i]->m_max_unlocked_hazard_level = playership->m_knownScenes[scene->m_bg->m_portals[(Directions)i]->m_destination_name];
			else//destination is not know yet -> default max hazard value
				scene->m_bg->m_portals[(Directions)i]->m_max_unlocked_hazard_level = 0;
		}
	}
}

void Gameloop::DestroySFPanel(Ship* playership)
{
	if (playership->m_SFTargetPanel != NULL)
	{
		playership->m_SFTargetPanel->GarbageMe();
		playership->m_SFTargetPanel = NULL;
	}
}

void Gameloop::CreateSFPanel(SFPanelTypes panel_type, Ship* playership)
{
	switch (panel_type)
	{
		case SFPanel_Trade:
		{
			playership->m_SFTargetPanel = new SFTradePanel(sf::Vector2f(TRADE_PANEL_WIDTH, TRADE_PANEL_HEIGHT), playership);//new SFInventoryPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INVENTORY_PANEL_HEIGHT), playership, SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS, false, true);
			break;
		}
		case SFPanel_Action:
		{
			playership->m_SFTargetPanel = new SFOneActionPanel(sf::Vector2f(ACTION_PANEL_WIDTH, ACTION_PANEL_HEIGHT), playership);
			break;
		}
		case SFPanel_Portal:
		{
 			playership->m_SFTargetPanel = new SFPortalPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INTERACTION_PANEL_HEIGHT), playership);
			break;
		}
		case SFPanel_Shop:
		{
			playership->m_SFTargetPanel = new SFShopPanel(sf::Vector2f(SHOP_PANEL_WIDTH, SHOP_PANEL_HEIGHT), playership);
			break;
		}
		case SFPanel_DialogNext:
		{
			playership->m_is_asking_SFPanel = SFPanel_Dialog;
		}
		case SFPanel_Dialog:
		{
			playership->m_SFTargetPanel = new SFDialogPanel(sf::Vector2f(DIALOG_PANEL_WIDTH, DIALOG_PANEL_HEIGHT), playership);
			break;
		}
		case SFPanel_Map:
		{
			playership->m_SFTargetPanel = new SFMapPanel(sf::Vector2f(STELLARMAP_PANEL_WIDTH, STELLARMAP_PANEL_HEIGHT), playership);
			break;
		}
		case SFPanel_Upgrades:
		{
			playership->m_SFTargetPanel = new SFUpgradesPanel(sf::Vector2f(UPGRADES_PANEL_WIDTH, UPGRADES_PANEL_HEIGHT), playership);
			break;
		}
	}

	(*CurrentGame).addToPanels((*CurrentGame).m_playership->m_SFTargetPanel);
}

void Gameloop::SpawnInScene(string scene_name, Ship* playership, bool display_scene_name)
{
	if (playership == NULL)
		return;
	
	//Level up if next scene is a Hub
	bool next_scene_is_hub = (bool)stoi((*CurrentGame).m_generalScenesConfig[scene_name][SCENE_IS_HUB]);
	if (next_scene_is_hub == true && scene_name.compare(playership->m_respawnSceneName) != 0)
		m_playership->m_level++;

	//Load next scene and swapping with current scene
	//(*CurrentGame).m_playership->m_disable_inputs = true;
	//(*CurrentGame).m_waiting_for_scene_transition = true;
	//(*CurrentGame).m_playership->m_immune = true;//
	delete m_nextScene;
	m_nextScene = new Scene(scene_name);
	delete m_currentScene;
	m_currentScene = m_nextScene;
	m_nextScene = NULL;

	(*CurrentGame).m_vspeed = m_currentScene->m_vspeed;
	(*CurrentGame).m_is_in_hub = m_currentScene->m_is_hub;
	m_playership->m_currentScene_name = m_currentScene->m_name;

	(*CurrentGame).m_playership->m_HUD_state = HUD_Idle;

	//cleaning layers
	(*CurrentGame).garbageLayer(FriendlyFireLayer, false, true);
	(*CurrentGame).garbageLayer(EnemyFireLayer, false, true);
	(*CurrentGame).garbageLayer(EnemyObjectLayer, false, true);
	(*CurrentGame).garbageLayer(ExplosionLayer, false, true);
	(*CurrentGame).garbageLayer(LootLayer, false, true);
	(*CurrentGame).garbageLayer(FeedbacksLayer, false, true);

	for (Dialog* dialog : m_playership->m_targetDialogs)
		delete dialog;
	m_playership->m_targetDialogs.clear();

	//enabling/disabling abilities depending if next scene is a Hub or not
	sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X * STARTSCENE_X_RATIO, SCENE_SIZE_Y * STARTSCENE_Y_RATIO);
	if (next_scene_is_hub == true)
	{
		(*CurrentGame).m_playership->m_disableHyperspeed = true;
		(*CurrentGame).m_playership->m_disableJump = true;
		(*CurrentGame).m_playership->m_disableSlowmotion = true;
		(*CurrentGame).m_playership->m_disable_bots = true;
		(*CurrentGame).m_playership->m_disableRecall = true;
		(*CurrentGame).m_playership->SetBotsVisibility(false);
		ship_pos = sf::Vector2f(SCENE_SIZE_X * STARTSCENE_X_RATIO, SCENE_SIZE_Y * STARTSCENE_X_RATIO);

		//Saving
		if (scene_name.compare(m_playership->m_respawnSceneName) != 0)
		{
			AddToKnownScenes(m_currentScene->m_name);
			Ship::SavePlayerScenes(m_playership);
			Ship::SavePlayerMoneyAndHealth(m_playership);
		}

		m_playership->m_respawnSceneName = scene_name;

		m_IG_State = HUB_ROAMING;
	}
	else
	{
		(*CurrentGame).m_playership->m_disable_fire = false;
		(*CurrentGame).m_playership->m_disableHyperspeed = false;
		(*CurrentGame).m_playership->m_disableJump = false;
		(*CurrentGame).m_playership->m_disableSlowmotion = false;
		(*CurrentGame).m_playership->m_disable_bots = false;
		(*CurrentGame).m_playership->m_disableRecall = false;
		(*CurrentGame).m_playership->SetBotsVisibility(true);

		m_IG_State = SCROLLING;
	}

	//player position
	(*CurrentGame).m_playership->setPosition(ship_pos);

	//Resetting counting of hits taken for scene score
	if (m_currentScene->m_is_hub == true)
		(*CurrentGame).m_playership->m_hits_taken = 0;

	//(*CurrentGame).m_waiting_for_scene_transition = false;
	(*CurrentGame).m_playership->m_immune = false;
	(*CurrentGame).m_playership->m_is_asking_scene_transition = false;

	//Play scene title feedback if we come from a Hub or getting to a Hub
	if (display_scene_name == true || m_currentScene->m_is_hub == true)
		m_currentScene->PlayTitleFeedback();

	//Music changes
	if (m_currentScene->m_scene_music.empty() == false)
		(*CurrentGame).PlayMusic(Music_Scene, m_currentScene->m_scene_music);
	else if (m_currentScene->m_is_hub == true)
		(*CurrentGame).PlayMusic(Music_Hub);
	else
		(*CurrentGame).PlayMusic(Music_Scene);

	/*
	//cleaning layers
	(*CurrentGame).garbageLayer(FriendlyFireLayer, false, true);
	(*CurrentGame).garbageLayer(EnemyFireLayer, false, true);
	(*CurrentGame).garbageLayer(EnemyObjectLayer, false, true);
	(*CurrentGame).garbageLayer(ExplosionLayer, false, true);
	(*CurrentGame).garbageLayer(LootLayer, false, true);
	(*CurrentGame).garbageLayer(FeedbacksLayer, false, true);

	//delete current scene <!> to do after cleaning layers, otherwise Enemy class will be left with their vector<Phase*> m_phases and FX* m_FX_Death pointing to destroyed elemnts
	delete m_currentScene;
	delete m_nextScene;

	//set new scene
	m_currentScene = new Scene(scene_name);
	playership->m_currentScene_name = m_currentScene->m_name;
	playership->m_currentScene_hazard = m_currentScene->getSceneHazardLevelValue();
		
	//direction
	//(*CurrentGame).m_direction = m_currentScene->m_direction;

	//speed
	(*CurrentGame).m_vspeed = m_currentScene->m_vspeed;

	//position
	sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X * STARTSCENE_X_RATIO, SCENE_SIZE_Y * STARTSCENE_Y_RATIO);
	if (m_currentScene->m_is_hub == true)
	{
		(*CurrentGame).m_playership->m_disable_fire = false;
		(*CurrentGame).m_playership->m_disableHyperspeed = false;
		(*CurrentGame).m_playership->m_disableJump = false;
		(*CurrentGame).m_playership->m_disableSlowmotion = false;
		(*CurrentGame).m_playership->m_disable_bots = false;
		(*CurrentGame).m_playership->m_disableRecall = false;
		(*CurrentGame).m_playership->SetBotsVisibility(true);
		m_IG_State = SCROLLING;
		ship_pos = sf::Vector2f(SCENE_SIZE_X * STARTSCENE_X_RATIO, SCENE_SIZE_Y * STARTSCENE_X_RATIO);

		if (!m_currentScene->m_scene_music.empty())
			(*CurrentGame).PlayMusic(Music_Scene, m_currentScene->m_scene_music);
		else
			(*CurrentGame).PlayMusic(Music_Scene);
	}
	else
	{
		(*CurrentGame).m_playership->m_disable_fire = true;
		(*CurrentGame).m_playership->m_disableHyperspeed = true;
		(*CurrentGame).m_playership->m_disableJump = true;
		(*CurrentGame).m_playership->m_disableSlowmotion = true;
		(*CurrentGame).m_playership->m_disable_bots = true;
		(*CurrentGame).m_playership->m_disableRecall = true;
		(*CurrentGame).m_playership->SetBotsVisibility(false);
		m_IG_State = HUB_ROAMING;
		playership->m_respawnSceneName = m_currentScene->m_name;

		(*CurrentGame).resetHazard();
		(*CurrentGame).m_playership->m_hits_taken = 0;

		if (m_currentScene->m_scene_music.empty() == false)
			(*CurrentGame).PlayMusic(Music_Hub, m_currentScene->m_scene_music);
		else
			(*CurrentGame).PlayMusic(Music_Hub);
	}

	m_playership->setPosition(ship_pos);

	UpdatePortalsMaxUnlockedHazardLevel(m_currentScene);

	for (Dialog* dialog : m_playership->m_targetDialogs)
		delete dialog;
	m_playership->m_targetDialogs.clear();

	Ship::SavePlayerScenes(m_playership);
	*/
}

void Gameloop::LoadAllScenes(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all scenes scripts.");

	vector<vector<string> > generalScenesConfig = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allScenesVectorSize = generalScenesConfig.size();
	for (size_t i = 0; i < allScenesVectorSize; i++)
	{
		(*CurrentGame).m_generalScenesConfig.insert(std::map<string, vector<string> >::value_type(generalScenesConfig[i][SCENE_NAME], generalScenesConfig[i]));
		TextureLoader *loader = TextureLoader::getInstance();
		loader->loadTexture(generalScenesConfig[i][SCENE_BACKGROUND_FILENAME], stoi(generalScenesConfig[i][SCENE_BACKGROUND_WIDTH]), stoi(generalScenesConfig[i][SCENE_BACKGROUND_HEIGHT]));
	}

	for (size_t j = 0; j < allScenesVectorSize; j++)
		if (!generalScenesConfig[j][SCENE_FILENAME].empty() && generalScenesConfig[j][SCENE_FILENAME].compare("0") != 0)
			(*CurrentGame).m_sceneConfigs.insert(std::map<string, vector<vector<string> > >::value_type(generalScenesConfig[j][SCENE_NAME], *(FileLoaderUtils::FileLoader(generalScenesConfig[j][SCENE_FILENAME]))));

	generalScenesConfig.clear();

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllEnemies(string enemies_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all enemies.");

	vector<vector<string> > enemiesConfig = *(FileLoaderUtils::FileLoader(enemies_file));
	size_t allEnemiesVectorSize = enemiesConfig.size();
	for (size_t i = 0; i < allEnemiesVectorSize; i++)
		(*CurrentGame).m_enemiesConfig.insert(std::map<string, vector<string> >::value_type(enemiesConfig[i][ENEMY_NAME], enemiesConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllFX(string FX_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all FX.");

	vector<vector<string> > FXConfig = *(FileLoaderUtils::FileLoader(FX_file));
	size_t allFXVectorSize = FXConfig.size();
	for (size_t i = 0; i < allFXVectorSize; i++)
		(*CurrentGame).m_FXConfig.insert(std::map<string, vector<string> >::value_type(FXConfig[i][FX_NAME], FXConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllUpgrades(string upgrades_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all upgrades.");

	vector<vector<string> > UpgradesConfig = *(FileLoaderUtils::FileLoader(upgrades_file));
	size_t allFXVectorSize = UpgradesConfig.size();
	for (size_t i = 0; i < allFXVectorSize; i++)
		(*CurrentGame).m_upgradesConfig.insert(std::map<string, vector<string> >::value_type(UpgradesConfig[i][UPGRADE_NAME], UpgradesConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::PlayerTakesExit()
{
	m_currentScene->m_bg->SetPortalsState(PortalGhost);
	(*CurrentGame).PlaySFX(SFX_EnteringPortal);

	string nextScene_filename = (*CurrentGame).m_playership->m_targetPortal->m_destination_name;

	SpawnInScene(nextScene_filename, (*CurrentGame).m_playership, m_currentScene->m_is_hub);
}

void Gameloop::CheckScriptedDialogs()
{
	size_t dialogsVectorSize = m_currentScene->m_dialogs.size();
	for (size_t i = 0; i < dialogsVectorSize; i++)
	{
		if (m_currentScene->m_dialogs[i].first > 0 && m_currentScene->m_scene_clock.getElapsedTime().asSeconds() > m_currentScene->m_dialogs[i].first)
		{
			size_t chainedDialogsVectorSize = m_currentScene->m_dialogs[i].second.size();
			for (size_t j = 0; j < chainedDialogsVectorSize; j++)
				(*CurrentGame).m_playership->m_targetDialogs.push_back(m_currentScene->m_dialogs[i].second[j]->Clone());

			m_currentScene->m_dialogs[i].first = -1;//flag it as read
			break;
		}
	}
}