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
	m_playerShip->m_HUD_SFPanel = (SFPanel*)(new SFHUDPanel(sf::Vector2f(SCENE_SIZE_X / 3, SCENE_SIZE_Y), m_playerShip));
	(*CurrentGame).addToFeedbacks(m_playerShip->m_HUD_SFPanel);

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
	LOGGER_WRITE(Logger::DEBUG, "Playership loaded\n");

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_playerShip->m_equipment[i] != NULL)
		{
			GameObject* capsule = Ship::CloneEquipmentIntoGameObject(m_playerShip->m_equipment[i]);
			m_playerShip->m_HUD_SFPanel->GetGrid()->setCellPointerForIntIndex(i, capsule);
		}
	}
	if (m_playerShip->m_weapon)
	{
		GameObject* capsule = Ship::CloneWeaponIntoGameObject(m_playerShip->m_weapon);
		m_playerShip->m_HUD_SFPanel->GetGrid()->setCellPointerForIntIndex(NBVAL_Equipment, capsule);
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
		player.m_currentSceneFile = "Vanguard_Hub0";
		AddToKnownScenes(player.m_currentSceneFile);
		SavePlayer(PLAYER_SAVE_FILE);
	}

	m_playerShip->m_currentScene_name = player.m_currentSceneFile;
	m_currentScene = NULL;

	//Loading current scene
	SpawnInScene(m_playerShip->m_currentScene_name);

	if ((*CurrentGame).m_direction != NO_DIRECTION)
	{
		(*CurrentGame).playerShip->m_disable_fire = false;
		(*CurrentGame).playerShip->m_disabledHyperspeed = false;
		(*CurrentGame).playerShip->m_disable_bots = false;
		(*CurrentGame).playerShip->SetBotsVisibility(true);
	}
	else
	{
		(*CurrentGame).playerShip->m_disable_fire = true;
		(*CurrentGame).playerShip->m_disabledHyperspeed = true;
		(*CurrentGame).playerShip->m_disable_bots = true;
		(*CurrentGame).playerShip->SetBotsVisibility(false);
	}
	//(*CurrentGame).playerShip->GenerateFakeShip((*CurrentGame).playerShip);
	(*CurrentGame).SetLayerRotation(FakeShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).SetLayerRotation(BotLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).SetLayerRotation(FeedbacksLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
	(*CurrentGame).addToScene((*CurrentGame).playerShip, PlayerShipLayer, PlayerShip);
}

void InGameState::Update(Time deltaTime)
{
	//automatic respawn if dead
	if (!(*CurrentGame).playerShip->m_visible)
	{
		RespawnInLastHub();
		//SpawnInScene((*CurrentGame).playerShip->m_respawnSceneName);
	}
	if (!(*CurrentGame).playerShip->m_is_asking_teleportation.empty())
	{
		SpawnInScene((*CurrentGame).playerShip->m_is_asking_teleportation);
		(*CurrentGame).playerShip->m_is_asking_teleportation = "";
	}

	InGameStateMachineCheck(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), m_currentScene->getSceneBeastScore());

	(*CurrentGame).updateScene(deltaTime);

	//Create and destroy HUD panels
	//case 1: destroying a panel
	if ((*CurrentGame).playerShip->m_is_asking_SFPanel == SFPanel_None && (*CurrentGame).playerShip->m_SFPanel)
	{
		DestroySFPanel((*CurrentGame).playerShip);
	}
	else if ((*CurrentGame).playerShip->m_is_asking_SFPanel != SFPanel_None)
	{
		//case 2: creating a panel
		if (!(*CurrentGame).playerShip->m_SFPanel)
		{
			CreateSFPanel((*CurrentGame).playerShip->m_is_asking_SFPanel, (*CurrentGame).playerShip);
		}
		//case 3: changing panel
		else if ((*CurrentGame).playerShip->m_SFPanel->m_panel_type != (*CurrentGame).playerShip->m_is_asking_SFPanel)
		{
			DestroySFPanel((*CurrentGame).playerShip);
			CreateSFPanel((*CurrentGame).playerShip->m_is_asking_SFPanel, (*CurrentGame).playerShip);
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
			if ((*CurrentGame).m_direction == NO_DIRECTION)
			{
				m_IG_State = InGameStateMachine::LAST_SCREEN;
			}
			else
			{
				//when last screen is reached
				if (m_currentScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2)) == GREATHER_THAN
					|| m_currentScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w / 2, h / 2)) == EQUAL_TO)
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
					(*CurrentGame).garbageLayer(EnemyObjectLayer, true);
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
					m_currentScene->m_bg->SetPortalsState(PortalInvisible);
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

						m_currentScene->m_bg->SetPortalsState(PortalOpen);

						m_hasDisplayedDestructionRatio = true;
					}
				}

				//player takes exit?
				if ((*CurrentGame).playerShip->m_is_asking_scene_transition)
				{
					m_currentScene->m_bg->SetPortalsState(PortalGhost);

					(*CurrentGame).removeFromFeedbacks(&(m_currentScene->m_textHazardBreak));

					bool reverse = false;
					if ((*CurrentGame).playerShip->m_targetPortal->m_direction == DIRECTION_DOWN || (*CurrentGame).playerShip->m_targetPortal->m_direction == DIRECTION_LEFT)
					{
						reverse = true;
					}

					string nextScene_filename = (*CurrentGame).playerShip->m_targetPortal->m_destination_name;
					m_nextScene = new Scene(nextScene_filename, m_currentScene->getSceneHazardLevelValue(), reverse, false);
					
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
				m_currentScene->m_bg->SetPortalsState(PortalClose);
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
			if ((*CurrentGame).playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2)) == LESSER_THAN
				|| (*CurrentGame).playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2)) == EQUAL_TO)
			{
				//Correction of playership position
				(*CurrentGame).playerShip->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(w_ / 2, h_ / 2));

				(*CurrentGame).playerShip->m_speed = GameObject::getSpeed_to_LocationWhileSceneSwap((*CurrentGame).m_direction, m_nextScene->m_direction, ENDSCENE_TRANSITION_SPEED_DOWN,
					(*CurrentGame).playerShip->getPosition());

				m_currentScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).m_vspeed = ENDSCENE_TRANSITION_SPEED_DOWN;
				m_nextScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling((*CurrentGame).m_direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).garbageLayer(FriendlyFireLayer);
				(*CurrentGame).garbageLayer(LootLayer);

				m_IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
			}

			break;
		}

		case InGameStateMachine::TRANSITION_PHASE2_2:
		{
			float wn = m_nextScene->m_bg->m_size.x;
			float hn = m_nextScene->m_bg->m_size.y;
			//When the new scene is completely swapped, we can wrap up the replacement and restart scrolling (or do what the Hubs do if the scene is a Hub)
			if (m_nextScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == GREATHER_THAN
				|| m_nextScene->m_bg->compare_posY_withTarget_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == EQUAL_TO)
			{

				//Correction of the scenes position
				m_currentScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f((w / 2), (h / 2)));

				m_nextScene->m_bg->setPosition_Y_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2)));
				m_nextScene->m_bg->m_speed = GameObject::getSpeed_for_Scrolling(m_nextScene->m_direction, m_nextScene->m_vspeed);
				(*CurrentGame).m_vspeed = m_nextScene->m_vspeed;

				if (m_nextScene->m_direction == NO_DIRECTION)
				{
					m_IG_State = InGameStateMachine::HUB_ROAMING;
					(*CurrentGame).playerShip->m_disabledHyperspeed = true;
					(*CurrentGame).playerShip->m_disable_bots = true;
					(*CurrentGame).playerShip->SetBotsVisibility(false);
				}
				else
				{
					m_IG_State = InGameStateMachine::SCROLLING;
					(*CurrentGame).playerShip->m_disable_fire = false;
					(*CurrentGame).playerShip->m_disabledHyperspeed = false;
					(*CurrentGame).playerShip->m_disable_bots = false;
					(*CurrentGame).playerShip->SetBotsVisibility(true);
					
					(*CurrentGame).SetLayerRotation(PlayerShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					(*CurrentGame).SetLayerRotation(FakeShipLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					(*CurrentGame).SetLayerRotation(BotLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
					(*CurrentGame).SetLayerRotation(FeedbacksLayer, GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
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
				delete m_currentScene;
				m_currentScene = m_nextScene;
				m_nextScene = NULL;
				(*CurrentGame).m_direction = m_currentScene->m_direction;
				m_playerShip->m_currentScene_name = m_currentScene->m_name;
				m_playerShip->m_currentScene_hazard = m_currentScene->getSceneHazardLevelValue();

				//Save scenes
				AddToKnownScenes(m_currentScene->m_name);
				if (m_currentScene->m_direction == NO_DIRECTION)
				{
					SavePlayer(PLAYER_SAVE_FILE);
					m_playerShip->m_respawnSceneName = m_currentScene->m_name;
				}
				else
				{
					SavePlayer(PLAYER_SAVE_FILE);
				}

				//Giving control back to the player
				(*CurrentGame).playerShip->m_disable_inputs = false;
				(*CurrentGame).playerShip->m_speed = sf::Vector2f(0, 0);
			}

			break;
		}

		case InGameStateMachine::HUB_ROAMING:
		{
			m_currentScene->m_bg->SetPortalsState(PortalOpen);
			//player takes exit?
			if ((*CurrentGame).playerShip->m_is_asking_scene_transition)
			{
				m_currentScene->m_bg->SetPortalsState(PortalGhost);
				bool reverse = false;
				if ((*CurrentGame).playerShip->m_targetPortal->m_direction == DIRECTION_DOWN || (*CurrentGame).playerShip->m_targetPortal->m_direction == DIRECTION_LEFT)
				{
					reverse = true;
				}
				string nextScene_filename = (*CurrentGame).playerShip->m_targetPortal->m_destination_name;

				(*CurrentGame).m_direction = (*CurrentGame).playerShip->m_targetPortal->m_direction;
				m_nextScene = new Scene(nextScene_filename, (*CurrentGame).playerShip->m_SFPanel->GetSelectedOptionIndex(), reverse, false);
				(*CurrentGame).playerShip->m_last_hazard_level_played = (*CurrentGame).playerShip->m_SFPanel->GetSelectedOptionIndex();
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

void InGameState::UpdatePortalsMaxUnlockedHazardLevel(Scene* scene, Ship* playerShip)
{
	//getting the max hazard value for the upcoming scene
	map<string, int>::iterator it = playerShip->m_knownScenes.find(scene->m_name);
	if (it != playerShip->m_knownScenes.end())
	{
		scene->m_hazard_level_unlocked = playerShip->m_knownScenes[scene->m_name];
	}
	else
	{
		//destination is not know yet -> default max hazard value
		scene->m_hazard_level_unlocked = 0;
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

void InGameState::RespawnInLastHub()
{
	//cleaning layers
	(*CurrentGame).garbageLayer(FriendlyFireLayer);
	(*CurrentGame).garbageLayer(EnemyFireLayer);
	(*CurrentGame).garbageLayer(EnemyObjectLayer);
	(*CurrentGame).garbageLayer(ExplosionLayer);
	(*CurrentGame).garbageLayer(LootLayer);

	//loading last visited hub
	SpawnInScene((*CurrentGame).playerShip->m_respawnSceneName);

	//Applying hub modifiers to gameplay
	(*CurrentGame).playerShip->m_disabledHyperspeed = true;
	(*CurrentGame).playerShip->m_disable_fire = true;
	m_IG_State = HUB_ROAMING;

	//resetting ship
	(*CurrentGame).playerShip->Respawn();
}

void InGameState::DestroySFPanel(Ship* playerShip)
{
	if (playerShip->m_SFPanel)
	{
		(*CurrentGame).removeFromFeedbacks(playerShip->m_SFPanel);
		delete playerShip->m_SFPanel;
		playerShip->m_SFPanel = NULL;
	}
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Inventory:
		{
			playerShip->m_SFPanel = new SFInventoryPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INVENTORY_PANEL_HEIGHT), playerShip, SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS, false, true);
			break;
		}
		case SFPanel_Action:
		{
			playerShip->m_SFPanel = new SFOneActionPanel(sf::Vector2f(ACTION_PANEL_WIDTH, ACTION_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_Portal:
		{
			playerShip->m_SFPanel = new SFPortalPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INTERACTION_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_Shop:
		{
			playerShip->m_SFPanel = new SFShopPanel(sf::Vector2f(INTERACTION_PANEL_WIDTH, INTERACTION_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_DialogNext:
		{
			playerShip->m_is_asking_SFPanel = SFPanel_Dialog;
		}
		case SFPanel_Dialog:
		{
			playerShip->m_SFPanel = new SFDialogPanel(sf::Vector2f(DIALOG_PANEL_WIDTH, DIALOG_PANEL_HEIGHT), playerShip);
			break;
		}
		case SFPanel_Map:
		{
			playerShip->m_SFPanel = new SFMapPanel(sf::Vector2f(STELLARMAP_PANEL_WIDTH, STELLARMAP_PANEL_HEIGHT), playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).playerShip->m_SFPanel);
}

void InGameState::SpawnInScene(string scene_name, Ship* playerShip)
{
	if (playerShip)
	{
		if (m_currentScene)
		{
			delete m_currentScene;
		}
		(*CurrentGame).garbageLayer(FeedbacksLayer);
		m_currentScene = new Scene(scene_name, 0, false, true);
		playerShip->m_currentScene_name = m_currentScene->m_name;
		playerShip->m_currentScene_hazard = m_currentScene->getSceneHazardLevelValue();

		//position
		sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_X_RATIO);
		if ((*CurrentGame).m_direction != NO_DIRECTION)
		{
			m_IG_State = InGameStateMachine::SCROLLING;
			ship_pos = GameObject::getPosition_for_Direction((*CurrentGame).m_direction, sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO));
		}
		else
		{
			m_IG_State = InGameStateMachine::HUB_ROAMING;
			playerShip->m_respawnSceneName = m_currentScene->m_name;
		}
		m_playerShip->setPosition(ship_pos);

		UpdatePortalsMaxUnlockedHazardLevel(m_currentScene);

		SavePlayer(PLAYER_SAVE_FILE);
	}
}