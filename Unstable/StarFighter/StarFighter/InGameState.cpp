#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	this->nextScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//allows to set the right direction for the first current scene
	(*CurrentGame).direction = this->nextScene->direction;
	this->currentScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//first_scene = true
	(*CurrentGame).direction = this->currentScene->direction;
	if ((*CurrentGame).direction != Directions::NO_DIRECTION)
	{
		this->IG_State = InGameStateMachine::SCROLLING;
	}
	else
	{
		this->IG_State = InGameStateMachine::HUB_ROAMING;
	}

	sf::Vector2f ship_pos = Independant::getCoordinates_for_Spawn(false, (*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X*STARTSCENE_Y_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO), false, true);

	this->playerShip = new Ship(ship_pos, *FileLoader::LoadShipConfig("default"));
	(*CurrentGame).SetPlayerShip(this->playerShip);

	LOGGER_WRITE(Logger::Priority::DEBUG, "Playership loaded\n");

	//bg
	(*CurrentGame).addToScene(this->currentScene->bg, LayerType::BackgroundLayer, IndependantType::Background);

	//ship
	(*CurrentGame).playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
	(*CurrentGame).playerShip->ship_config.GenerateFakeShip((*CurrentGame).playerShip);
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, IndependantType::PlayerShip);

	this->hud = new PlayerHud();
	this->hud->Init((*CurrentGame).playerShip->ship_config.getShipConfigArmor(), (*CurrentGame).playerShip->ship_config.getShipConfigShield());
}

void InGameState::Update(Time deltaTime)
{
	this->currentScene->Update(deltaTime);
	InGameStateMachineCheck();

	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), this->currentScene->getSceneBeastScore());
	(*CurrentGame).updateScene(deltaTime);
	this->mainWindow->clear();

	this->hud->Update(this->mainWindow, (*CurrentGame).playerShip->armor, (*CurrentGame).playerShip->shield, (*CurrentGame).playerShip->getMoney(),
		(*CurrentGame).playerShip->graze_count, (*CurrentGame).getHazard(), this->currentScene->getSceneHazardBreakValue(), deltaTime);
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
	this->hud->Draw(this->mainWindow);
}

void InGameState::Release()
{
	//TODO
}

void InGameState::InGameStateMachineCheck()
{
	float w = this->currentScene->bg->m_size.x;
	float h = this->currentScene->bg->m_size.y;
	float wn = this->nextScene->bg->m_size.x;
	float hn = this->nextScene->bg->m_size.y;
	float w_ = (*CurrentGame).playerShip->m_size.x;
	float h_ = (*CurrentGame).playerShip->m_size.y;

	switch (this->IG_State)
	{
	case InGameStateMachine::SCROLLING:
	{
										  //Scrolling until the background reaches its end
										  if ((*CurrentGame).direction == Directions::NO_DIRECTION)
										  {
											  this->IG_State = InGameStateMachine::LAST_SCREEN;
										  }
										  else
										  {
											  if (Independant::isPositionPastDistance_to_ScreenBorder((*CurrentGame).direction, sf::Vector2f(w / 2, h / 2), this->currentScene->bg->getPosition(),
												  false))
											  {
												  //Correct the position
												  this->currentScene->bg->setPosition(Independant::getCoordinates_for_Spawn(false, (*CurrentGame).direction, sf::Vector2f(w / 2, h / 2), false, true));
												  //Load next scene
												  if (this->currentScene->links[(*CurrentGame).direction].compare("0") != 0)
												  {
													  this->nextScene->LoadSceneFromFile(this->currentScene->links[(*CurrentGame).direction], false, false);
													  this->currentScene->bg->speed = sf::Vector2f(0, 0);
													  this->currentScene->generating_enemies = false;

													  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  }
												  //else: do nothing. The game is likely to be ended.
											  }
										  }

										  break;
	}

	case InGameStateMachine::LAST_SCREEN:
	{
											//When enemies, loots and enemy bullets on scene are dead, we can start the transition to the next scene
											if ((*CurrentGame).isLastEnemyDead())
											{
												//Putting the player on rails
												(*CurrentGame).playerShip->disable_inputs = true;
												(*CurrentGame).playerShip->disable_fire = true;
												(*CurrentGame).playerShip->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_UP, true);

												this->IG_State = InGameStateMachine::TRANSITION_PHASE1_2;
											}
											break;

	}
	case InGameStateMachine::TRANSITION_PHASE1_2:
	{
													//When the playership reaches the scene border, we can start the swapping of scenes, while replacing him on the right starting position for the next scene
													if (Independant::isPositionPastDistance_to_ScreenBorder((*CurrentGame).direction, sf::Vector2f(w_ / 2, h_ / 2), (*CurrentGame).playerShip->getPosition(), false, true))
													{
														//Correction of playership position
														(*CurrentGame).playerShip->setPosition(Independant::getCoordinates_for_Spawn(false, (*CurrentGame).direction, sf::Vector2f(w_ / 2, h_ / 2), false, true,
															true, (*CurrentGame).playerShip->getPosition()));

														(*CurrentGame).playerShip->speed = Independant::getSpeed_to_LocationWhileSceneSwap((*CurrentGame).direction, this->nextScene->direction, ENDSCENE_TRANSITION_SPEED_DOWN,
															(*CurrentGame).playerShip->getPosition());

														this->currentScene->bg->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_DOWN, false);
														this->nextScene->bg->speed = Independant::getSpeed_for_Scrolling((*CurrentGame).direction, ENDSCENE_TRANSITION_SPEED_DOWN, false);
														(*CurrentGame).addToScene(this->nextScene->bg, LayerType::BackgroundLayer, IndependantType::Background);
														(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
														(*CurrentGame).garbageLayer(LayerType::BotLayer);

														this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
													}
													break;
	}

	case InGameStateMachine::TRANSITION_PHASE2_2:
	{
													//When the new scene is completely swapped, we can wrap up the replacement and restart scrolling (or do what the Hubs do if the scene is a Hub)
													if (Independant::isPositionPastDistance_to_ScreenBorder((*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X + (w / 2), SCENE_SIZE_Y + (h / 2)), this->currentScene->bg->getPosition(), false))
													{
														//Correction of the scenes position
														this->currentScene->bg->setPosition(Independant::getCoordinates_for_Spawn(false, (*CurrentGame).direction, sf::Vector2f(SCENE_SIZE_X + (w / 2), SCENE_SIZE_Y + (h / 2)), false, true));

														this->nextScene->bg->setPosition(Independant::getCoordinates_for_Spawn(false, (*CurrentGame).direction, sf::Vector2f((wn / 2) - SCENE_SIZE_X, (hn / 2) - SCENE_SIZE_Y),
															true, true));

														this->nextScene->bg->speed = Independant::getSpeed_for_Scrolling(this->nextScene->direction, this->nextScene->vspeed, false);

														if (this->nextScene->direction == Directions::NO_DIRECTION)
														{
															this->IG_State = InGameStateMachine::HUB_ROAMING;
														}
														else
														{
															this->IG_State = InGameStateMachine::SCROLLING;
															(*CurrentGame).playerShip->disable_fire = false;
															(*CurrentGame).playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
															(*CurrentGame).SetLayerRotation(LayerType::FakeShipLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
															(*CurrentGame).SetLayerRotation(LayerType::BotLayer, Independant::getRotation_for_Direction((*CurrentGame).direction));
														}
														//Wiping the previous background and swapping with the new one
														this->currentScene->bg->GarbageMe = true;
														(*CurrentGame).resetHazard();
														*this->currentScene = *this->nextScene;
														(*CurrentGame).direction = this->currentScene->direction;

														//Giving control back to the player
														(*CurrentGame).playerShip->disable_inputs = false;
														(*CurrentGame).playerShip->speed = sf::Vector2f(0, 0);
													}

													break;
	}

	case InGameStateMachine::HUB_ROAMING:
	{
											float x = playerShip->getPosition().x;
											float y = playerShip->getPosition().y;

											float X_min = SCENE_SIZE_X*HUB_EXIT_X_MIN_RATIO;
											float X_max = SCENE_SIZE_X*HUB_EXIT_X_MAX_RATIO;
											float Y_min = SCENE_SIZE_Y*HUB_EXIT_Y_MIN_RATIO;
											float Y_max = SCENE_SIZE_Y*HUB_EXIT_Y_MAX_RATIO;

											if ((x > X_min && x < X_max && y > Y_min && y < Y_max) || (x<X_min && y<Y_min) || (x<X_min && y >Y_max) || (x>X_max && y >Y_max) || (x>X_max && y < Y_min))
											{
												sf::Time timer = clockHubExit.restart();
											}
											else
											{
												sf::Time timer = clockHubExit.getElapsedTime();
												bool reverse = false;

												if (y<Y_min && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go UP
													(*CurrentGame).direction = Directions::DIRECTION_UP;

												}

												else if (y > Y_max && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go DOWN
													(*CurrentGame).direction = Directions::DIRECTION_DOWN;
													reverse = true;
												}

												else if (x > X_max && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go RIGHT
													(*CurrentGame).direction = Directions::DIRECTION_RIGHT;
												}

												else if (x<X_min && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go LEFT
													(*CurrentGame).direction = Directions::DIRECTION_LEFT;
													reverse = true;
												}

												if ((*CurrentGame).direction != Directions::NO_DIRECTION)
												{
													if (this->currentScene->links[(*CurrentGame).direction].compare("0") != 0)
													{
														this->nextScene->LoadSceneFromFile(this->currentScene->links[(*CurrentGame).direction], reverse, false);
														this->IG_State = InGameStateMachine::LAST_SCREEN;
													}
												}
											}

											break;
	}

	default:
	{
			   break;
	}
	}
}