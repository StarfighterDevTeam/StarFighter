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
		this->currentSceneSave = player.m_currentSceneFile;
	}
	else
	{
		//New game save
		player.m_currentSceneFile = "Vanguard_Hub0";
		this->currentSceneSave = player.m_currentSceneFile;
		AddToKnownScenes(player.m_currentSceneFile);
		SavePlayer(PLAYER_SAVE_FILE);
	}

	//Loading current scene
	this->currentScene = new Scene(this->currentSceneSave, GetSceneHazardLevelUnlocked(this->currentSceneSave), player.reverse_scene, true);//first_scene = true
	UpdatePortalsMaxUnlockedHazardLevel(this->currentScene);

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
	
	//creating new ship
	this->playerShip = new Ship(ship_pos, *FileLoader::LoadShipConfig("default"));
	this->playerShip->ResplenishHealth();
	this->playerShip->respawnSceneName = this->currentScene->m_name;

	//transmitting player level to the game
	(*CurrentGame).level = this->playerShip->level;
	(*CurrentGame).enemy_base_stat_multiplier = (*CurrentGame).GetEnemiesStatsMultiplierForLevel((*CurrentGame).level);
	(*CurrentGame).loot_on_par_stat_multiplier = (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel((*CurrentGame).level);

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
		(*CurrentGame).playerShip->disabledHyperspeed = false;
	}
	else
	{
		(*CurrentGame).playerShip->disable_fire = true;
		(*CurrentGame).playerShip->disabledHyperspeed = true;
	}
	(*CurrentGame).playerShip->ship_config.GenerateFakeShip((*CurrentGame).playerShip);
	(*CurrentGame).SetLayerRotation(LayerType::FakeShipLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
	(*CurrentGame).SetLayerRotation(LayerType::BotLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
	(*CurrentGame).SetLayerRotation(LayerType::FeedbacksLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, IndependantType::PlayerShip);
}

void InGameState::Update(Time deltaTime)
{
	//automatic respawn if dead
	if (!(*CurrentGame).playerShip->visible)
	{
		RespawnInLastHub();
	}

	InGameStateMachineCheck(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), this->currentScene->getSceneBeastScore());

	(*CurrentGame).updateScene(deltaTime);

	(*CurrentGame).UpdateInteractionPanel((*CurrentGame).playerShip->previouslyCollindingWithInteractiveObject, (*CurrentGame).playerShip->GetFocusedPortalMaxUnlockedHazardLevel(), deltaTime);

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

			(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->armor_max, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->shield_max, (*CurrentGame).playerShip->getMoney(), (*CurrentGame).playerShip->graze_count,
				this->currentScene->getSceneHazardLevelValue(), this->currentScene->bg->display_name, (*CurrentGame).playerShip->level, (*CurrentGame).playerShip->level_max, (*CurrentGame).playerShip->xp, (*CurrentGame).playerShip->xp_max, deltaTime, this->currentScene->direction == NO_DIRECTION,
				equip_index_, tmp_weapon->display_name, -1, -1, -1, -1, -1, -1, -1, -1, -1, tmp_weapon->ammunition->damage, tmp_weapon->bonus_damage, false, tmp_weapon->ammunition->speed.y, tmp_weapon->ammunition->Pattern.currentPattern, tmp_weapon->multishot, tmp_weapon->bonus_multishot, 
				tmp_weapon->xspread, tmp_weapon->rate_of_fire, tmp_weapon->bonus_rate_of_fire, tmp_weapon->shot_mode, tmp_weapon->dispersion, tmp_weapon->rafale, tmp_weapon->rafale_cooldown, tmp_weapon->target_seaking);

			tmp_weapon = NULL;
		}
		else
		{
			Equipment* tmp_equipment = tmp_ptr->getEquipmentLoot();

			if (!tmp_equipment->hasBot)
			{
				(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->armor_max, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->shield_max, (*CurrentGame).playerShip->getMoney(),
					(*CurrentGame).playerShip->graze_count, this->currentScene->getSceneHazardLevelValue(), this->currentScene->bg->display_name, (*CurrentGame).playerShip->level, (*CurrentGame).playerShip->level_max, (*CurrentGame).playerShip->xp, (*CurrentGame).playerShip->xp_max, deltaTime, this->currentScene->direction == NO_DIRECTION,
					equip_index_, tmp_equipment->display_name, tmp_equipment->getEquipmentMaxSpeed().x, (*CurrentGame).playerShip->ship_config.getShipConfigHyperspeed(), tmp_equipment->bonus_hyperspeed, (*CurrentGame).playerShip->ship_config.getShipConfigArmor(), tmp_equipment->bonus_armor,
					(*CurrentGame).playerShip->ship_config.getShipConfigShield(), tmp_equipment->bonus_shield, (*CurrentGame).playerShip->ship_config.getShipConfigShieldRegen(), tmp_equipment->bonus_shield_regen, (*CurrentGame).playerShip->ship_config.getShipConfigDamage(), tmp_equipment->bonus_damage, tmp_equipment->hasBot);
			}
			else
			{
				//todo : clean getEquipmentHyperspeed() etc...
				(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->armor_max, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->shield_max, (*CurrentGame).playerShip->getMoney(),
					(*CurrentGame).playerShip->graze_count, this->currentScene->getSceneHazardLevelValue(), this->currentScene->bg->display_name, (*CurrentGame).playerShip->level, (*CurrentGame).playerShip->level_max, (*CurrentGame).playerShip->xp, (*CurrentGame).playerShip->xp_max, deltaTime, this->currentScene->direction == NO_DIRECTION,
					equip_index_, tmp_equipment->display_name, tmp_equipment->bot->Pattern.patternSpeed, tmp_equipment->getEquipmentHyperspeed(), tmp_equipment->bonus_hyperspeed, tmp_equipment->bot->armor_max, tmp_equipment->bonus_armor,
					tmp_equipment->bot->shield_max, tmp_equipment->bonus_shield, tmp_equipment->bot->shield_regen, tmp_equipment->bonus_shield_regen, tmp_equipment->bot->weapon->ammunition->damage, tmp_equipment->bot->weapon->bonus_damage, tmp_equipment->hasBot, tmp_equipment->bot->weapon->ammunition->speed.y,
					tmp_equipment->bot->weapon->ammunition->Pattern.currentPattern, tmp_equipment->bot->weapon->multishot, tmp_equipment->bot->weapon->bonus_multishot, tmp_equipment->bot->weapon->xspread, tmp_equipment->bot->weapon->rate_of_fire, tmp_equipment->bot->weapon->bonus_rate_of_fire,
					tmp_equipment->bot->weapon->shot_mode, tmp_equipment->bot->weapon->dispersion, tmp_equipment->bot->weapon->rafale, tmp_equipment->bot->weapon->rafale_cooldown, tmp_equipment->bot->weapon->target_seaking);
			}
			
			tmp_equipment = NULL;
		}

		tmp_ptr = NULL;
	}
	else //...else not bothering with it
	{
		(*CurrentGame).updateHud((*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->armor_max, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->shield_max, (*CurrentGame).playerShip->getMoney(),
			(*CurrentGame).playerShip->graze_count, this->currentScene->getSceneHazardLevelValue(), this->currentScene->bg->display_name, (*CurrentGame).playerShip->level, (*CurrentGame).playerShip->level_max, (*CurrentGame).playerShip->xp, (*CurrentGame).playerShip->xp_max, deltaTime, this->currentScene->direction == NO_DIRECTION);
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
	this->currentSceneSave = scene_name;
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

void InGameState::SaveSceneHazardLevelUnlocked(string scene_name, int hazard_level)
{
	map<string, int>::iterator it = this->knownScenes.find(scene_name);
	if (hazard_level > NB_HAZARD_LEVELS - 1)
	{
		hazard_level = NB_HAZARD_LEVELS - 1;
	}
	if (it != this->knownScenes.end())
	{
		this->knownScenes[scene_name] = hazard_level;
	}
}

int InGameState::GetSceneHazardLevelUnlocked(string scene_name)
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
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (map<string, int>::iterator it = this->knownScenes.begin(); it != this->knownScenes.end(); it++)
		{
			data << it->first.c_str() << " " << it->second;
			if (it->first.c_str() == this->currentSceneSave)
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

			this->knownScenes.insert(std::pair<string, int>(scene, level));
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
					(*CurrentGame).vspeed = 0;

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
				//scene generates boss? (or boss is dead, eitherway)
				if (this->currentScene->generating_boss)
				{
					this->currentScene->bg->SetPortalsState(PortalState::PortalInvisible);
					if (bossSpawnCountdown.getElapsedTime() > sf::seconds(TIME_BEFORE_BOSS_SPAWN))
					{
						this->currentScene->GenerateBoss();
						this->IG_State = InGameStateMachine::BOSS_FIGHT;
					}
				}
				else
				{
					if (!this->hasDisplayedDestructionRatio)
					{
						//is the scene capable of hazard break? (= last scene before hub)
						if (this->currentScene->canHazardBreak)
						{
							//what is our destruction ratio? (displaying score). 100% = Hazard break
							if ((*CurrentGame).getHazard() - (*CurrentGame).hazardSpawned == 0)
							{
								this->currentScene->HazardBreak();
								this->currentScene->DisplayDestructions(true);
							}
							else
							{
								this->currentScene->DisplayDestructions();
							}

							//eitherway, reset the score
							(*CurrentGame).resetHazard();
						}

						this->currentScene->bg->SetPortalsState(PortalState::PortalOpen);

						hasDisplayedDestructionRatio = true;
					}
				}

				//player takes exit?
				if ((*CurrentGame).playerShip->m_interactionType == PortalInteraction)
				{
					this->currentScene->bg->SetPortalsState(PortalState::PortalGhost);

					(*CurrentGame).removeFromFeedbacks(&(this->currentScene->m_textHazardBreak));

					bool reverse = false;
					if ((*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_DOWN || (*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_LEFT)
					{
						reverse = true;
					}

					string nextScene_filename = (*CurrentGame).playerShip->targetPortal->destination_name;
					this->nextScene = new Scene(nextScene_filename, (*CurrentGame).m_interactionPanel->m_selected_index, reverse, false);
					//remembering linked scenes to hazard break later
					if (!this->currentScene->canHazardBreak)
					{
						this->nextScene->scenesLinkedToUpdate.push_back(this->currentScene->m_name);
					}

					UpdatePortalsMaxUnlockedHazardLevel(this->nextScene);

					this->nextScene->bg->speed = sf::Vector2f(0, 0);

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
				(*CurrentGame).vspeed = ENDSCENE_TRANSITION_SPEED_DOWN;
				this->nextScene->bg->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_DOWN);
				(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
				(*CurrentGame).garbageLayer(LayerType::LootLayer);
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
				(*CurrentGame).vspeed = this->nextScene->vspeed;

				if (this->nextScene->direction == Directions::NO_DIRECTION)
				{
					this->IG_State = InGameStateMachine::HUB_ROAMING;
					(*CurrentGame).playerShip->disabledHyperspeed = true;
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
					(*CurrentGame).SetLayerRotation(LayerType::FeedbacksLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
				}

				//Saving the hazard level change
				if (this->currentScene->canHazardBreak)
				{
					SaveSceneHazardLevelUnlocked(this->currentScene->m_name, this->currentScene->getSceneHazardLevelUnlockedValue());
					for (vector <string>::iterator it = this->currentScene->scenesLinkedToUpdate.begin(); it != this->currentScene->scenesLinkedToUpdate.end(); it++)
					{
						SaveSceneHazardLevelUnlocked((*it), this->currentScene->getSceneHazardLevelUnlockedValue());
					}
					this->currentScene->scenesLinkedToUpdate.clear();
					//transmitting the info to the next scene so it can update her portals
					UpdatePortalsMaxUnlockedHazardLevel(this->nextScene);
				}
				else
				{
					//if the next scene is not a hub, then we want to remember the past scenes so we can update them later in case of hazard break
					for (vector <string>::iterator it = this->currentScene->scenesLinkedToUpdate.begin(); it != this->currentScene->scenesLinkedToUpdate.end(); it++)
					{
						this->nextScene->scenesLinkedToUpdate.push_back((*it));
					}
				}

				//Wiping the previous background and swapping with the new one
				this->currentScene->DestroyScene();
				delete this->currentScene;
				this->currentScene = this->nextScene;
				this->nextScene = NULL;
				(*CurrentGame).direction = this->currentScene->direction;

				//Save scenes
				AddToKnownScenes(this->currentScene->m_name);
				if (this->currentScene->direction == Directions::NO_DIRECTION)
				{
					this->SavePlayer(PLAYER_SAVE_FILE);
					this->playerShip->respawnSceneName = this->currentScene->m_name;
				}

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
			if ((*CurrentGame).playerShip->m_interactionType == PortalInteraction)
			{
				this->currentScene->bg->SetPortalsState(PortalState::PortalGhost);
				bool reverse = false;
				if ((*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_DOWN || (*CurrentGame).playerShip->targetPortal->direction == Directions::DIRECTION_LEFT)
				{
					reverse = true;
				}
				string nextScene_filename = (*CurrentGame).playerShip->targetPortal->destination_name;
				(*CurrentGame).direction = (*CurrentGame).playerShip->targetPortal->direction;
				this->nextScene = new Scene(nextScene_filename, (*CurrentGame).m_interactionPanel->m_selected_index, reverse, false);
				UpdatePortalsMaxUnlockedHazardLevel(this->nextScene);
				this->nextScene->bg->speed = sf::Vector2f(0, 0);

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



void InGameState::UpdatePortalsMaxUnlockedHazardLevel(Scene* scene_)
{
	//getting the max hazard value for the upcoming scene
	map<string, int>::iterator it = this->knownScenes.find(scene_->m_name);
	if (it != this->knownScenes.end())
	{
		scene_->m_hazard_level_unlocked = this->knownScenes[scene_->m_name];
	}
	else
	{
		//destination is not know yet -> default max hazard value
		scene_->m_hazard_level_unlocked = 0;
	}

	//loading the scene's portals with the info about their respective max hazard values
	for (int i = 0; i < Directions::NO_DIRECTION; i++)
	{
		if (scene_->bg->portals[(Directions)i] != NULL)
		{
			map<string, int>::iterator it = this->knownScenes.find(scene_->bg->portals[(Directions)i]->destination_name);
			if (it != this->knownScenes.end())
			{
				scene_->bg->portals[(Directions)i]->max_unlocked_hazard_level = this->knownScenes[scene_->bg->portals[(Directions)i]->destination_name];
			}
			else
			{
				//destination is not know yet -> default max hazard value
				scene_->bg->portals[(Directions)i]->max_unlocked_hazard_level = 0;
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
	string nextScene_filename = (*CurrentGame).playerShip->respawnSceneName;
	this->nextScene = new Scene(nextScene_filename, 0, false, true);
	UpdatePortalsMaxUnlockedHazardLevel(this->nextScene);

	//Wiping the previous background and swapping with the new one
	this->currentScene->DestroyScene();
	delete this->currentScene;
	this->currentScene = this->nextScene;
	this->nextScene = NULL;
	(*CurrentGame).direction = this->currentScene->direction;

	//Applying hub modifiers to gameplay
	(*CurrentGame).playerShip->disabledHyperspeed = true;
	(*CurrentGame).playerShip->disable_fire = true;
	this->IG_State = HUB_ROAMING;

	//resetting ship
	(*CurrentGame).playerShip->Respawn();
	(*CurrentGame).playerShip->setPosition(sf::Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_X_RATIO));
}