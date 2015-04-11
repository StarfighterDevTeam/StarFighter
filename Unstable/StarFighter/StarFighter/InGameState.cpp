#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	LoadPlayerSave(player.m_save_file);//update knownScenes and hazard levels from save file
	
	//Loading current scene
	this->currentScene = new Scene(player.m_currentSceneFile, GetSceneHazardLevel(player.m_currentSceneFile), player.reverse_scene, true);//first_scene = true
	AddToKnownScenes(this->currentScene->m_name);

	sf::Vector2f ship_pos = sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_X_RATIO);
	if ((*CurrentGame).direction != Directions::NO_DIRECTION)
	{
		this->IG_State = InGameStateMachine::SCROLLING;
		ship_pos = Independant::getPosition_for_Direction((*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO));
	}
	else
	{
		this->IG_State = InGameStateMachine::HUB_ROAMING;
	}

	this->playerShip = new Ship(ship_pos, *FileLoader::LoadShipConfig("default"));

	//initalizing equipment in HUD
	LOGGER_WRITE(Logger::Priority::DEBUG, "Initializing equipment in HUD...");
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (this->playerShip->ship_config.equipment[i] != NULL)
		{
			Independant* capsule = Ship::CloneEquipmentIntoIndependant(this->playerShip->ship_config.equipment[i]);
			if (!(*CurrentGame).InsertObjectInShipGrid(*capsule, i))
			{
				LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: could not initialize an equipment item from the ship config to the HUD Ship grid\n");
			}
		}
	}
	
	if (this->playerShip->ship_config.weapon != NULL)
	{
		Independant* capsule = Ship::CloneWeaponIntoIndependant(this->playerShip->ship_config.weapon);
		if (!(*CurrentGame).InsertObjectInShipGrid(*capsule, NBVAL_Equipment))
		{
			LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: could not initialize a weapon item from the ship config to the HUD Ship grid\n");
		}
	}
	LOGGER_WRITE(Logger::Priority::DEBUG, "HUD initialization completed\n");

	//transmitting a global reference to the ship
	(*CurrentGame).SetPlayerShip(this->playerShip);

	LOGGER_WRITE(Logger::Priority::DEBUG, "Playership loaded\n");

	//ship
	if ((*CurrentGame).direction != Directions::NO_DIRECTION)
	{
		(*CurrentGame).playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
	}
	else
	{
		(*CurrentGame).playerShip->disable_fire = true;
	}
	(*CurrentGame).playerShip->ship_config.GenerateFakeShip((*CurrentGame).playerShip);
	(*CurrentGame).SetLayerRotation(LayerType::FakeShipLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
	(*CurrentGame).SetLayerRotation(LayerType::BotLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, IndependantType::PlayerShip);
}

void InGameState::Update(Time deltaTime)
{
	InGameStateMachineCheck(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), this->currentScene->getSceneBeastScore());

	(*CurrentGame).updateScene(deltaTime);

	//displaying stats of focused item in the HUD...
	if ((*CurrentGame).getHudFocusedItem() != NULL)
	{
		Independant* tmp_ptr = (*CurrentGame).getHudFocusedItem();
		int equip_index_ = NBVAL_Equipment;
		if (tmp_ptr->getEquipmentLoot() != NULL)
		{
			equip_index_ = tmp_ptr->getEquipmentLoot()->equipmentType;
		}

		if (equip_index_ == NBVAL_Equipment)
		{
			Weapon* tmp_weapon = tmp_ptr->getWeaponLoot();

			(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->getMoney(),
				(*CurrentGame).playerShip->graze_count, this->GetSceneHazardLevel(this->currentScene->m_name), this->currentScene->bg->display_name, deltaTime, this->currentScene->direction == NO_DIRECTION,
				equip_index_, tmp_weapon->display_name, -1, -1, -1, -1, -1, -1, tmp_weapon->ammunition->damage, false, tmp_weapon->multishot, tmp_weapon->xspread, tmp_weapon->rate_of_fire, tmp_weapon->shot_mode,
				tmp_weapon->dispersion, tmp_weapon->rafale, tmp_weapon->rafale_cooldown, tmp_weapon->target_seaking);

			tmp_weapon = NULL;
		}
		else
		{
			Equipment* tmp_equipment = tmp_ptr->getEquipmentLoot();

			(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->getMoney(),
				(*CurrentGame).playerShip->graze_count, this->GetSceneHazardLevel(this->currentScene->m_name), this->currentScene->bg->display_name, deltaTime, this->currentScene->direction == NO_DIRECTION,
				equip_index_, tmp_equipment->display_name, tmp_equipment->getEquipmentMaxSpeed().x, tmp_equipment->getEquipmentDecceleration(), tmp_equipment->getEquipmentHyperspeed(), tmp_equipment->getEquipmentArmor(), 
				tmp_equipment->getEquipmentShield(), tmp_equipment->getEquipmentShieldRegen(), tmp_equipment->getEquipmentDamage(), tmp_equipment->hasBot);

			tmp_equipment = NULL;
		}

		tmp_ptr = NULL;
	}
	else //...else not bothering with it
	{
		(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->getMoney(),
			(*CurrentGame).playerShip->graze_count, this->GetSceneHazardLevel(this->currentScene->m_name), this->currentScene->bg->display_name, deltaTime, this->currentScene->direction == NO_DIRECTION);
	}

	this->mainWindow->clear();
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
	map<string, int>::iterator it = this->knownScenes.find(scene_name);

	//if scene not already known
	if (it == this->knownScenes.end())
	{
		//add it to the map of known scenes
		this->knownScenes.insert(pair<string, int>(scene_name, 0));
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Adding '%s' to known scenes.\n", (char*)scene_name.c_str()));
		return true;
	}
	return false;
}

void InGameState::SetSceneHazardLevel(string scene_name, int hazard_level)
{
	map<string, int>::iterator it = this->knownScenes.find(scene_name);
	if (hazard_level > NB_HAZARD_LEVELS - 1)
	{
		hazard_level = NB_HAZARD_LEVELS - 1;
	}
	this->knownScenes[scene_name] = hazard_level;
	
}

int InGameState::GetSceneHazardLevel(string scene_name)
{
	map<string, int>::iterator it = this->knownScenes.find(scene_name);

	//if scene is known
	if (it != this->knownScenes.end())
	{
		return this->knownScenes[scene_name];
	}
	//else
	return 0;
}

int InGameState::SavePlayer(string file)
{
	ofstream data(file.c_str(), ios::in | ios::trunc);
	if (data)  // si l'ouverture a r�ussi
	{
		// instructions
		for (map<string, int>::iterator it = this->knownScenes.begin(); it != this->knownScenes.end(); it++)
		{
			data << it->first.c_str() << " " << it->second << endl;
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "Failed to open PLAYER SAVE FILE !" << endl;
	}

	return 0;
}

int InGameState::LoadPlayerSave(string file)
{
	std::ifstream  data(file, ios::in);

	if (data) // si ouverture du fichier r�ussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			string scene;
			int level;
			std::istringstream(line) >> scene >> level;
			if (level > NB_HAZARD_LEVELS - 1)
			{
				level = NB_HAZARD_LEVELS - 1;
			}
			this->knownScenes.insert(std::pair<string, int>(scene, level));
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "Failed to open PLAYER SAVE FILE !" << endl;
	}

	return 0;
}

void InGameState::InGameStateMachineCheck(sf::Time deltaTime)
{
	float w = this->currentScene->bg->m_size.x;
	float h = this->currentScene->bg->m_size.y;
	float w_ = (*CurrentGame).playerShip->m_size.x;
	float h_ = (*CurrentGame).playerShip->m_size.y;
	if ((*CurrentGame).playerShip->ship_config.ship_model->hasFake)
	{
		w_ = (*CurrentGame).playerShip->ship_config.ship_model->fake_size.x;
		h_ = (*CurrentGame).playerShip->ship_config.ship_model->fake_size.y;
	}

	switch (this->IG_State)
	{
		case InGameStateMachine::SCROLLING:
		{
			if (this->currentScene->generating_enemies)
			{
				this->currentScene->GenerateEnemiesv2(deltaTime);
			}
			//Scrolling until the background reaches its end
			if ((*CurrentGame).direction == Directions::NO_DIRECTION)
			{
				this->IG_State = InGameStateMachine::LAST_SCREEN;
			}
			else
			{
				//when last screen is reached
				if (this->currentScene->bg->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f(w / 2, h / 2)) == FloatCompare::GREATHER_THAN
					|| this->currentScene->bg->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f(w / 2, h / 2)) == FloatCompare::EQUAL_TO)
				{
					//Correct the position
					this->currentScene->bg->setPosition_Y_for_Direction((*CurrentGame).direction, sf::Vector2f(w / 2, h / 2));
					this->currentScene->bg->speed = sf::Vector2f(0, 0);

					//Disable hyperspeed capacity
					(*CurrentGame).playerShip->disabledHyperspeed = true;

					//Stop spawning enemies
					this->currentScene->generating_enemies = false;
					if (this->currentScene->generating_boss)
					{
						this->bossSpawnCountdown.restart();
					}
					
					this->hasDisplayedDestructionRatio = false;
					this->IG_State = InGameStateMachine::LAST_SCREEN;
				}
			}

			break;
		}

		case InGameStateMachine::LAST_SCREEN:
		{	
			//When enemies, loots and enemy bullets on scene are dead, we can start the transition to the next scene
			if ((*CurrentGame).isLastEnemyDead())
			{
				//trigger hazard break event (spawning boss) if destructions are 100.00% and we reached the last scene before a hub
				if (!this->currentScene->m_hazardbreak_has_occurred && !this->hasDisplayedDestructionRatio && this->currentScene->generating_boss)
				{
					//displaying the xx.xx% of destruction
					//int pourcentage = 100.0f * (*CurrentGame).getHazard() / (*CurrentGame).hazardSpawned;
					printf("Destructions: %d / %d [%.2f%%]. ", (*CurrentGame).getHazard(), (*CurrentGame).hazardSpawned, roundf(100.0f * (*CurrentGame).getHazard() / (*CurrentGame).hazardSpawned));
					this->hasDisplayedDestructionRatio = true;
					if ((*CurrentGame).getHazard() - (*CurrentGame).hazardSpawned == 0)
					{
						this->currentScene->m_hazardbreak_has_occurred = true;
						printf("SPAWNING BOSS...\n");
					}
					else
					{
						printf("No hazard break event\n");
					}

					(*CurrentGame).resetHazard();
				}
				else if (!this->hasDisplayedDestructionRatio)
				{
					printf("No boss to spawn.\n");
					this->hasDisplayedDestructionRatio = true;
				}
				
				this->currentScene->bg->SetPortalsState(PortalState::PortalOpen);

				if (this->currentScene->m_hazardbreak_has_occurred && this->currentScene->generating_boss)
				{
					if (bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
					{
						this->currentScene->GenerateBoss();
						this->currentScene->bg->SetPortalsState(PortalState::PortalInvisible);
						this->IG_State = InGameStateMachine::BOSS_FIGHT;
					}
				}

				//player takes exit?
				if ((*CurrentGame).playerShip->isUsingPortal)
				{
					this->currentScene->bg->SetPortalsState(PortalState::PortalGhost);
					bool reverse = false;
					if ((*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_DOWN || (*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_LEFT)
					{
						reverse = true;
					}

					string nextScene_filename = (*CurrentGame).playerShip->targetPortal->destination_name;
					this->nextScene = new Scene(nextScene_filename, GetSceneHazardLevel(nextScene_filename), reverse, false);
					this->nextScene->bg->speed = sf::Vector2f(0, 0);
					AddToKnownScenes(nextScene_filename);

					//Putting the player on rails
					(*CurrentGame).playerShip->disable_inputs = true;
					(*CurrentGame).playerShip->disable_fire = true;
					(*CurrentGame).playerShip->speed = -Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_UP);

					this->IG_State = InGameStateMachine::TRANSITION_PHASE1_2;
				}
			}
			//clearing enemies that have spawned out of the scene size
			else
			{
				(*CurrentGame).garbageLayer(LayerType::EnemyObjectLayer, true);
				if (this->currentScene->generating_boss)
				{
					this->bossSpawnCountdown.restart();
				}
				this->currentScene->bg->SetPortalsState(PortalState::PortalClose);
			}

			break;
		}

		case InGameStateMachine::BOSS_FIGHT:
		{
			//is boss dead?
			if ((*CurrentGame).isLastEnemyDead())
			{
				this->currentScene->generating_boss = false;
				this->currentScene->HazardBreak();
				this->IG_State = InGameStateMachine::LAST_SCREEN;
			}

			break;
		}

		case InGameStateMachine::TRANSITION_PHASE1_2:
		{
			//When the playership reaches the scene border, we can start the swapping of scenes, while replacing him on the right starting position for the next scene
			if ((*CurrentGame).playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f(w_ / 2, h_ / 2)) == FloatCompare::LESSER_THAN
				|| (*CurrentGame).playerShip->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f(w_ / 2, h_ / 2)) == FloatCompare::EQUAL_TO)
			{
				//Correction of playership position
				(*CurrentGame).playerShip->setPosition_Y_for_Direction((*CurrentGame).direction, sf::Vector2f(w_ / 2, h_ / 2));

				(*CurrentGame).playerShip->speed = Independant::getSpeed_to_LocationWhileSceneSwap((*CurrentGame).direction, this->nextScene->direction, ENDSCENE_TRANSITION_SPEED_DOWN,
					(*CurrentGame).playerShip->getPosition());

				this->currentScene->bg->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				this->nextScene->bg->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
				if (this->nextScene->direction == Directions::NO_DIRECTION)
				{
					(*CurrentGame).garbageLayer(LayerType::BotLayer);
				}

				this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
			}

			break;
		}

		case InGameStateMachine::TRANSITION_PHASE2_2:
		{
			float wn = this->nextScene->bg->m_size.x;
			float hn = this->nextScene->bg->m_size.y;
			//When the new scene is completely swapped, we can wrap up the replacement and restart scrolling (or do what the Hubs do if the scene is a Hub)
			if (this->nextScene->bg->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == FloatCompare::GREATHER_THAN
				|| this->nextScene->bg->compare_posY_withTarget_for_Direction((*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2))) == FloatCompare::EQUAL_TO)
			{

				//Correction of the scenes position
				this->currentScene->bg->setPosition_Y_for_Direction((*CurrentGame).direction, sf::Vector2f((w / 2), (h / 2)));

				this->nextScene->bg->setPosition_Y_for_Direction((*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X - (wn / 2), SCENE_SIZE_Y - (hn / 2)));
				this->nextScene->bg->speed = Independant::getSpeed_for_Scrolling(this->nextScene->direction, this->nextScene->vspeed);

				if (this->nextScene->direction == Directions::NO_DIRECTION)
				{
					this->IG_State = InGameStateMachine::HUB_ROAMING;
				}
				else
				{
					this->IG_State = InGameStateMachine::SCROLLING;
					(*CurrentGame).playerShip->disable_fire = false;
					(*CurrentGame).playerShip->disabledHyperspeed = false;
					if (this->currentScene->direction == Directions::NO_DIRECTION)
					{
						(*CurrentGame).playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
					}
					(*CurrentGame).SetLayerRotation(LayerType::PlayerShipLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
					(*CurrentGame).SetLayerRotation(LayerType::FakeShipLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
					(*CurrentGame).SetLayerRotation(LayerType::BotLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
				}

				//Saving the hazard level
				SetSceneHazardLevel(this->currentScene->m_name, this->currentScene->getSceneHazardLevelValue());

				//Wiping the previous background and swapping with the new one
				this->currentScene->DestroyScene();
				delete this->currentScene;
				this->currentScene = this->nextScene;
				this->nextScene = NULL;
				(*CurrentGame).direction = this->currentScene->direction;

				//and save the map into the player save file
				AddToKnownScenes(this->currentScene->m_name);
				this->SavePlayer(PLAYER_SAVE_FILE);

				//Giving control back to the player
				(*CurrentGame).playerShip->disable_inputs = false;
				(*CurrentGame).playerShip->speed = sf::Vector2f(0, 0);
			}

			break;
		}

		case InGameStateMachine::HUB_ROAMING:
		{
			this->currentScene->bg->SetPortalsState(PortalState::PortalOpen);
			//player takes exit?
			if ((*CurrentGame).playerShip->isUsingPortal)
			{
				this->currentScene->bg->SetPortalsState(PortalState::PortalGhost);
				bool reverse = false;
				if ((*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_DOWN || (*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_LEFT)
				{
					reverse = true;
				}
				string nextScene_filename = (*CurrentGame).playerShip->targetPortal->destination_name;
				(*CurrentGame).direction = (*CurrentGame).playerShip->targetPortal->direction;
				this->nextScene = new Scene(nextScene_filename, GetSceneHazardLevel(nextScene_filename), reverse, false);
				this->nextScene->bg->speed = sf::Vector2f(0, 0);

				AddToKnownScenes(nextScene_filename);

				//Putting the player on rails
				(*CurrentGame).playerShip->disable_inputs = true;
				(*CurrentGame).playerShip->disable_fire = true;
				(*CurrentGame).playerShip->speed = -Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_UP);

				this->IG_State = InGameStateMachine::TRANSITION_PHASE1_2;
			}

			break;
		}
	}
}