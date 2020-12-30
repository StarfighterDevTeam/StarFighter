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

	//Pre-load config files
	LoadAllScenes(SCENES_FILE);
	LoadAllFX(FX_FILE);
	LoadAllEnemies(ENEMY_FILE);
	LoadAllUpgrades(UPGRADES_FILE);
	LoadAllWeapons(WEAPONS_FILE);
	LoadAllAmmunitions(AMMO_FILE);
	LoadAllDialogs(DIALOGS_FILE);
	LoadAllEnemyPhases(PHASES_FILE);
	LoadAllBots(BOTS_FILE);
	
	PreloadAssets(Faction_Vanguard);

	//creating new ship
	m_playership = new Ship("2D/Equipment/Natalia16.png", sf::Vector2f(16, 16), "2D/Equipment/ShipModel_default.png", sf::Vector2f(70, 80), 3, NB_ShipAnimations);
	(*CurrentGame).SetPlayerShip(m_playership);

	//initializing HUD
	LOGGER_WRITE(Logger::DEBUG, "Initializing HUD...");
	m_playership->m_SFHudPanel = (SFPanel*)(new SFHUDPanel(sf::Vector2f(SCENE_SIZE_X / 3, SCENE_SIZE_Y), m_playership));
	(*CurrentGame).addToPanels(m_playership->m_SFHudPanel);
	LOGGER_WRITE(Logger::DEBUG, "HUD initialization completed\n");

	//Loading save files or creating new ones
	(*CurrentGame).m_playership->Respawn(true);
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
		Ship::SavePlayerMoneyAndHealth(m_playership);
		Ship::SavePlayerUpgrades(m_playership);
		Ship::SavePlayerScenes(m_playership);
		Shop::SaveShopUpgrades(NULL);

		//Loading starting scene
		SpawnInScene(STARTING_SCENE, (*CurrentGame).m_playership, false);
	}

	(*CurrentGame).addToScene((*CurrentGame).m_playership, false);

	//DEBUG
	//SpawnInScene("Sandbox", (*CurrentGame).m_playership, false);
}

void Gameloop::Update(Time deltaTime)
{
	//automatic respawn if dead
	if ((*CurrentGame).m_playership->m_visible == false && (*CurrentGame).m_playership->m_collision_timer <= 0)
	{
		//"Multi-segment mode": respawn at last checkpoint
		if (MULTISEGMENT_RESPAWN == true && (*CurrentGame).m_playership->m_respawnSceneName.empty() == false)
		{
			//resetting ship (without saving because we're going to load the last player save)
			(*CurrentGame).m_playership->Respawn(true);
			Ship::LoadPlayerScenes((*CurrentGame).m_playership);
			Ship::LoadPlayerUpgrades((*CurrentGame).m_playership);
			Ship::LoadPlayerMoneyAndHealth((*CurrentGame).m_playership);
			(*CurrentGame).m_playership->m_armor = (*CurrentGame).m_playership->m_armor_max;
			(*CurrentGame).m_playership->m_shield = 0;
			
			SpawnInScene(m_playership->m_respawnSceneName, m_playership, false);
		}
		else//"Single segment" or no save file found
		{
			//resetting ship
			(*CurrentGame).m_playership->Respawn(false);

			SpawnInScene(STARTING_SCENE, (*CurrentGame).m_playership, false);
		}
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
	GameloopStateMachineCheck(deltaTime);

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
		m_playership->m_armor = m_playership->m_armor_max;
		m_playership->m_shield = m_playership->m_shield_max;
	}

	//F7: relad saved items and enemy config
	if (InputGuy::reloadCSVs())
	{
		(*CurrentGame).m_enemiesConfig.clear();
		LoadAllEnemies(ENEMY_FILE);

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

void Gameloop::GameloopStateMachineCheck(sf::Time deltaTime)
{
	float w = m_currentScene->m_bg->m_size.x;
	float h = m_currentScene->m_bg->m_size.y;

	if ((*CurrentGame).m_gameloop_state == SCROLLING)
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

			//Stop spawning enemies
			m_currentScene->m_generating_enemies = false;
			if (m_currentScene->m_generating_boss == true)
				m_bossSpawnCountdown.restart();
					
			m_hasDisplayedDestructionRatio = false;

			//Wipe out enemies that were spawned offscren
			(*CurrentGame).garbageLayer(EnemyObjectLayer, true, true);

			//Switch state
			(*CurrentGame).m_gameloop_state = LAST_SCREEN;
		}
	}
	
	if ((*CurrentGame).m_gameloop_state == LAST_SCREEN)
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
					(*CurrentGame).m_gameloop_state = BOSS_FIGHT;
				}
			}
			else
			{
				//(*CurrentGame).m_playership->RegenHealthFast(deltaTime, false, true, false);
				(*CurrentGame).m_playership->m_disableSlowmotion = true;

				if (m_hasDisplayedDestructionRatio == false)
				{
					//m_currentScene->DisplayScore();
					m_hasDisplayedDestructionRatio = true;
					
					m_currentScene->m_bg->SetPortalsState(PortalOpen);
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

	if ((*CurrentGame).m_gameloop_state == BOSS_FIGHT)
	{
		//is boss dead?
		if ((*CurrentGame).isLastEnemyDead() == true)
		{
			m_currentScene->m_generating_boss = false;
			(*CurrentGame).m_gameloop_state = LAST_SCREEN;
		}
	}

	if ((*CurrentGame).m_gameloop_state == HUB_ROAMING)
	{
		m_currentScene->m_bg->SetPortalsState(PortalOpen);
			
		//(*CurrentGame).m_playership->RegenHealthFast(deltaTime, true, true, true);

		//player takes exit?
		if ((*CurrentGame).m_playership->m_is_asking_scene_transition == true)
			PlayerTakesExit();
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
	(*CurrentGame).m_background = m_currentScene->m_bg;
	(*CurrentGame).m_is_in_hub = m_currentScene->m_is_hub;
	m_playership->m_currentScene_name = m_currentScene->m_name;
	AddToKnownScenes(m_currentScene->m_name);

	(*CurrentGame).m_playership->m_HUD_state = HUD_Idle;

	//enabling/disabling abilities depending if next scene is a Hub or not
	sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X * STARTSCENE_X_RATIO, SCENE_SIZE_Y * STARTSCENE_Y_RATIO);
	if (next_scene_is_hub == true)
	{
		(*CurrentGame).m_playership->m_disable_fire = true;
		(*CurrentGame).m_playership->m_disableSpecial = true;
		(*CurrentGame).m_playership->m_disableSlowmotion = true;
		(*CurrentGame).m_playership->m_disable_bots = true;
		(*CurrentGame).m_playership->m_disableRecall = true;
		(*CurrentGame).m_playership->SetBotsVisibility(false);
		ship_pos = sf::Vector2f(SCENE_SIZE_X * STARTSCENE_X_RATIO, SCENE_SIZE_Y * STARTSCENE_X_RATIO);

		//Saving
		if (scene_name.compare(m_playership->m_respawnSceneName) != 0)
		{
			m_playership->m_respawnSceneName = scene_name;

			Ship::SavePlayerScenes(m_playership);
			Ship::SavePlayerMoneyAndHealth(m_playership);
		}

		(*CurrentGame).m_gameloop_state = HUB_ROAMING;
	}
	else
	{
		(*CurrentGame).m_playership->m_disable_fire = false;
		(*CurrentGame).m_playership->m_disableSpecial = false;
		(*CurrentGame).m_playership->m_disableSlowmotion = false;
		(*CurrentGame).m_playership->m_disable_bots = false;
		(*CurrentGame).m_playership->m_disableRecall = false;
		(*CurrentGame).m_playership->SetBotsVisibility(true);

		(*CurrentGame).m_gameloop_state = SCROLLING;
	}

	//player position
	(*CurrentGame).m_playership->setPosition(ship_pos);

	//Resetting counting of hits taken for scene score
	if (m_currentScene->m_is_hub == true)
		(*CurrentGame).m_playership->m_hits_taken = 0;

	//(*CurrentGame).m_waiting_for_scene_transition = false;
	(*CurrentGame).m_playership->m_immune = false;
	(*CurrentGame).m_playership->m_is_asking_scene_transition = false;
	(*CurrentGame).m_playership->m_collision_timer = 0;
	(*CurrentGame).m_playership->setColor(sf::Color(255, 255, 255, 255));
	if ((*CurrentGame).m_playership->m_fake_ship != NULL)
		(*CurrentGame).m_playership->m_fake_ship->setColor(sf::Color(255, 255, 255, 255));

	//Play scene title feedback if we come from a Hub or getting to a Hub
	if (display_scene_name == true || m_currentScene->m_is_hub == true)
		m_currentScene->PlayTitleFeedback();

	//Music changes
	if (m_currentScene->m_scene_music.compare("0") != 0)
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
		(*CurrentGame).m_playership->m_disableSpecial = false;
		(*CurrentGame).m_playership->m_disableSlowmotion = false;
		(*CurrentGame).m_playership->m_disable_bots = false;
		(*CurrentGame).m_playership->m_disableRecall = false;
		(*CurrentGame).m_playership->SetBotsVisibility(true);
		m_gameloop_state = SCROLLING;
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
		(*CurrentGame).m_playership->m_disableSpecial = true;
		(*CurrentGame).m_playership->m_disableSlowmotion = true;
		(*CurrentGame).m_playership->m_disable_bots = true;
		(*CurrentGame).m_playership->m_disableRecall = true;
		(*CurrentGame).m_playership->SetBotsVisibility(false);
		m_gameloop_state = HUB_ROAMING;
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
	size_t vectorSize = generalScenesConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		(*CurrentGame).m_generalScenesConfig.insert(std::map<string, vector<string> >::value_type(generalScenesConfig[i][SCENE_NAME], generalScenesConfig[i]));
		TextureLoader *loader = TextureLoader::getInstance();
		loader->loadTexture(generalScenesConfig[i][SCENE_BACKGROUND_FILENAME], stoi(generalScenesConfig[i][SCENE_BACKGROUND_WIDTH]), stoi(generalScenesConfig[i][SCENE_BACKGROUND_HEIGHT]));
	}

	for (size_t j = 0; j < vectorSize; j++)
		if (!generalScenesConfig[j][SCENE_FILENAME].empty() && generalScenesConfig[j][SCENE_FILENAME].compare("0") != 0)
			(*CurrentGame).m_sceneConfigs.insert(std::map<string, vector<vector<string> > >::value_type(generalScenesConfig[j][SCENE_NAME], *(FileLoaderUtils::FileLoader(generalScenesConfig[j][SCENE_FILENAME]))));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllEnemies(string enemies_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all enemies.");

	vector<vector<string> > enemiesConfig = *(FileLoaderUtils::FileLoader(enemies_file));
	size_t vectorSize = enemiesConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_enemiesConfig.insert(std::map<string, vector<string> >::value_type(enemiesConfig[i][ENEMY_NAME], enemiesConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllFX(string FX_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all FX.");

	vector<vector<string> > FXConfig = *(FileLoaderUtils::FileLoader(FX_file));
	size_t vectorSize = FXConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_FXConfig.insert(std::map<string, vector<string> >::value_type(FXConfig[i][FX_NAME], FXConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllUpgrades(string upgrades_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all upgrades.");

	vector<vector<string> > UpgradesConfig = *(FileLoaderUtils::FileLoader(upgrades_file));
	size_t vectorSize = UpgradesConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_upgradesConfig.insert(std::map<string, vector<string> >::value_type(UpgradesConfig[i][UPGRADE_NAME], UpgradesConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllWeapons(string weapons_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all weapons.");

	vector<vector<string> > WeaponsConfig = *(FileLoaderUtils::FileLoader(weapons_file));
	size_t vectorSize = WeaponsConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_weaponsConfig.insert(std::map<string, vector<string> >::value_type(WeaponsConfig[i][WEAPON_NAME], WeaponsConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllAmmunitions(string ammo_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all ammunitions.");

	vector<vector<string> > AmmoConfig = *(FileLoaderUtils::FileLoader(ammo_file));
	size_t vectorSize = AmmoConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_ammoConfig.insert(std::map<string, vector<string> >::value_type(AmmoConfig[i][AMMO_NAME], AmmoConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllDialogs(string dialogs_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all dialogs.");

	vector<vector<string> > DialogsConfig = *(FileLoaderUtils::FileLoader(dialogs_file));
	size_t vectorSize = DialogsConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_dialogsConfig.insert(std::map<string, vector<string> >::value_type(DialogsConfig[i][DIALOG_NAME], DialogsConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::LoadAllEnemyPhases(string phases_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all enemy phases.");

	vector<vector<string> > PhasesConfig = *(FileLoaderUtils::FileLoader(phases_file));
	size_t vectorSize = PhasesConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_phasesConfig.insert(std::map<string, vector<string> >::value_type(PhasesConfig[i][PHASE_NAME], PhasesConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}


void Gameloop::LoadAllBots(string bots_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading all bots.");

	vector<vector<string> > BotsConfig = *(FileLoaderUtils::FileLoader(bots_file));
	size_t vectorSize = BotsConfig.size();
	for (size_t i = 0; i < vectorSize; i++)
		(*CurrentGame).m_botsConfig.insert(std::map<string, vector<string> >::value_type(BotsConfig[i][BOT_NAME], BotsConfig[i]));

	LOGGER_WRITE(Logger::DEBUG, "Loading complete.");
}

void Gameloop::PreloadAssets(FactionType faction)
{
	LOGGER_WRITE(Logger::DEBUG, "Preloading assets.");
	
	TextureLoader* loader = TextureLoader::getInstance();
	
	string key;
	switch (faction)
	{
		case Faction_Vanguard:
			key = "V";
			break;
		case Faction_Royale:
			key = "R";
			break;
		case Faction_Corsair:
			key = "C";
			break;
		case Faction_Ancient:
			key = "A";
			break;
		case Faction_Swarm:
			key = "S";
			break;
	}

	for (map<string, vector<string> >::iterator it = (*CurrentGame).m_enemiesConfig.begin(); it != (*CurrentGame).m_enemiesConfig.end(); it++)
		if (it->first.substr(0, 1).compare(key) == 0)
			loader->loadTexture(it->second[ENEMY_IMAGE_NAME], stoi(it->second[ENEMY_WIDTH]) * stoi(it->second[ENEMY_FRAMES]), stoi(it->second[ENEMY_HEIGHT]) * stoi(it->second[ENEMY_NB_SKINS]));

	for (map<string, vector<string> >::iterator it = (*CurrentGame).m_ammoConfig.begin(); it != (*CurrentGame).m_ammoConfig.end(); it++)
		if (it->first.substr(0, 1).compare(key) == 0)
			loader->loadTexture(it->second[AMMO_IMAGE_NAME], stoi(it->second[AMMO_WIDTH]) * stoi(it->second[AMMO_FRAMES]), stoi(it->second[AMMO_HEIGHT]) * stoi(it->second[AMMO_NB_SKINS]));
}

void Gameloop::PlayerTakesExit()
{
	m_currentScene->m_bg->SetPortalsState(PortalGhost);
	(*CurrentGame).PlaySFX(SFX_EnteringPortal);
	(*CurrentGame).m_playership->m_release_to_fire = true;

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