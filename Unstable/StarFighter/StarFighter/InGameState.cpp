#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	this->nextScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//allows to set the right direction for the first current scene
	(*CurrentGame).SetScrollingDirection(this->nextScene->direction);
	this->currentScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//first_scene = true
	if (this->currentScene->direction != sf::Vector2i(0, 0))
	{
		this->IG_State = InGameStateMachine::SCROLLING;
	}
	else
	{
		this->IG_State = InGameStateMachine::HUB_ROAMING;
	}

	sf::Vector2f ship_pos = InGameState::StartingShipPosition(this->currentScene->direction);

	
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
										  if ((*CurrentGame).direction == sf::Vector2i(0, 1))
										  {
											  if (this->currentScene->bg->getPosition().y >= h / 2)
											  {
												  this->currentScene->bg->setPosition(sf::Vector2f(this->currentScene->bg->getPosition().x, h / 2));

												  InGameState::Scrolling_to_LastScreen();
												  if (this->currentScene->links[Directions::DIRECTION_UP].compare("0") != 0)
												  {
													  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_UP], false, false);
													  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  }

											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(0, -1))
										  {
											  if (this->currentScene->bg->getPosition().y <= (-h / 2) + SCENE_SIZE_Y)
											  {
												  this->currentScene->bg->setPosition(sf::Vector2f(this->currentScene->bg->getPosition().x, (-h / 2) + SCENE_SIZE_Y));

												  InGameState::Scrolling_to_LastScreen();
												  if (this->currentScene->links[Directions::DIRECTION_DOWN].compare("0") != 0)
												  {
													  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_DOWN], true, false);
													  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  }
											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(1, 0))
										  {
											  if (this->currentScene->bg->getPosition().x <= (-w / 2) + SCENE_SIZE_X)
											  {
												  this->currentScene->bg->setPosition(sf::Vector2f((-w / 2) + SCENE_SIZE_X, this->currentScene->bg->getPosition().y));

												  InGameState::Scrolling_to_LastScreen();
												  if (this->currentScene->links[Directions::DIRECTION_RIGHT].compare("0") != 0)
												  {
													  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_RIGHT], false, false);
													  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  }
											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(-1, 0))
										  {
											  if (this->currentScene->bg->getPosition().x >= w / 2)
											  {
												  this->currentScene->bg->setPosition(sf::Vector2f(w / 2, this->currentScene->bg->getPosition().y));

												  InGameState::Scrolling_to_LastScreen();
												  if (this->currentScene->links[Directions::DIRECTION_LEFT].compare("0") != 0)
												  {
													  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_LEFT], true, false);
													  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  }
											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(0, 0))
										  {
											  this->IG_State = InGameStateMachine::LAST_SCREEN;
										  }
										  break;
	}

	case InGameStateMachine::LAST_SCREEN:
	{
											if ((*CurrentGame).isLastEnemyDead())
											{
												InGameState::LastScreen_to_Transition();
											}
											break;

	}
	case InGameStateMachine::TRANSITION_PHASE1_2:
	{
													if ((*CurrentGame).direction == sf::Vector2i(0, 1))
													{
														if (this->playerShip->getPosition().y <= h_ / 2)
														{
															this->playerShip->setPosition(sf::Vector2f(this->playerShip->getPosition().x, h_ / 2));

															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).x) - playerShip->getPosition().x) / SCENE_SIZE_Y;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).y) - playerShip->getPosition().y) / SCENE_SIZE_Y;
															InGameState::Transition1_2();
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(0, -1))
													{
														if (this->playerShip->getPosition().y >= SCENE_SIZE_Y - (h_ / 2))
														{
															this->playerShip->setPosition(sf::Vector2f(this->playerShip->getPosition().x, SCENE_SIZE_Y - (h_ / 2)));

															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).x) - playerShip->getPosition().x) / SCENE_SIZE_Y;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).y) - playerShip->getPosition().y) / SCENE_SIZE_Y;
															InGameState::Transition1_2();
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(1, 0))
													{
														if (this->playerShip->getPosition().x >= SCENE_SIZE_X - (w_ / 2))
														{
															this->playerShip->setPosition(sf::Vector2f(SCENE_SIZE_X - (w_ / 2), this->playerShip->getPosition().y));

															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).x) - playerShip->getPosition().x) / SCENE_SIZE_X;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).y) - playerShip->getPosition().y) / SCENE_SIZE_X;
															InGameState::Transition1_2();
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(-1, 0))
													{
														if (this->playerShip->getPosition().x <= w_ / 2)
														{
															this->playerShip->setPosition(sf::Vector2f(w_ / 2, this->playerShip->getPosition().y));

															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).x) - playerShip->getPosition().x) / SCENE_SIZE_X;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((StartingShipPosition(this->nextScene->direction).y) - playerShip->getPosition().y) / SCENE_SIZE_X;
															InGameState::Transition1_2();
														}
													}
													
													break;
	}

	case InGameStateMachine::TRANSITION_PHASE2_2:
	{
													if ((*CurrentGame).direction == sf::Vector2i(0, 1))
													{
														if (this->currentScene->bg->getPosition().y >= SCENE_SIZE_Y + (h / 2))
														{
															this->currentScene->bg->setPosition(sf::Vector2f(this->currentScene->bg->getPosition().x, SCENE_SIZE_Y + (h / 2)));
															this->nextScene->bg->setPosition(sf::Vector2f(this->nextScene->bg->getPosition().x, SCENE_SIZE_Y - (hn / 2)));
															InGameState::Transition2_2();
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(0, -1))
													{
														if (this->currentScene->bg->getPosition().y <= -h / 2)
														{
															this->currentScene->bg->setPosition(sf::Vector2f(this->currentScene->bg->getPosition().x, -h / 2));
															this->nextScene->bg->setPosition(sf::Vector2f(this->nextScene->bg->getPosition().x, (hn / 2)));
															InGameState::Transition2_2();
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(1, 0))
													{
														if (this->currentScene->bg->getPosition().x <= -w / 2)
														{
															this->currentScene->bg->setPosition(sf::Vector2f(-w / 2, this->currentScene->bg->getPosition().y));
															this->nextScene->bg->setPosition(sf::Vector2f((wn / 2), this->nextScene->bg->getPosition().y));
															InGameState::Transition2_2();
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(-1, 0))
													{
														if (this->currentScene->bg->getPosition().x >= SCENE_SIZE_X + (w / 2))
														{
															this->currentScene->bg->setPosition(sf::Vector2f(SCENE_SIZE_X + (w / 2), this->currentScene->bg->getPosition().y));
															this->nextScene->bg->setPosition(sf::Vector2f(SCENE_SIZE_X - (wn / 2), this->nextScene->bg->getPosition().y));
															InGameState::Transition2_2();
														}
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

												if (y<Y_min && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go UP
													printf("DEBUG: Travel UP !\n");
													(*CurrentGame).SetScrollingDirection(sf::Vector2i(0, 1));
													this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_UP], false, false);
													this->IG_State = InGameStateMachine::LAST_SCREEN;
												}

												else if (y > Y_max && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go DOWN
													printf("DEBUG: Travel DOWN !\n");
													(*CurrentGame).SetScrollingDirection(sf::Vector2i(0, -1));
													this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_DOWN], true, false);
													this->IG_State = InGameStateMachine::LAST_SCREEN;
												}

												else if (x > X_max && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go RIGHT
													printf("DEBUG: Travel RIGHT !\n");
													(*CurrentGame).SetScrollingDirection(sf::Vector2i(1, 0));
													this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_RIGHT], false, false);
													this->IG_State = InGameStateMachine::LAST_SCREEN;
												}

												else if (x<X_min && timer.asSeconds() > HUB_EXIT_TIMER)
												{
													//go LEFT
													printf("DEBUG: Travel LEFT !\n");
													(*CurrentGame).SetScrollingDirection(sf::Vector2i(-1, 0));
													this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_LEFT], true, false);
													this->IG_State = InGameStateMachine::LAST_SCREEN;
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

void InGameState::Scrolling_to_LastScreen()
{
	this->currentScene->bg->speed = sf::Vector2f(0, 0);
	this->currentScene->generating_enemies = false;
}

void InGameState::LastScreen_to_Transition()
{
	this->IG_State = InGameStateMachine::TRANSITION_PHASE1_2;

	this->playerShip->disable_inputs = true;
	this->playerShip->disable_fire = true;

	this->playerShip->speed.x = (*CurrentGame).direction.x * ENDSCENE_TRANSITION_SPEED_UP;
	this->playerShip->speed.y = -(*CurrentGame).direction.y * ENDSCENE_TRANSITION_SPEED_UP;
}

void InGameState::Transition1_2()
{
	this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
	this->currentScene->bg->speed.x = -(*CurrentGame).direction.x * ENDSCENE_TRANSITION_SPEED_DOWN;
	this->currentScene->bg->speed.y = (*CurrentGame).direction.y * ENDSCENE_TRANSITION_SPEED_DOWN;
	this->nextScene->bg->speed.x = -(*CurrentGame).direction.x * ENDSCENE_TRANSITION_SPEED_DOWN;
	this->nextScene->bg->speed.y = (*CurrentGame).direction.y * ENDSCENE_TRANSITION_SPEED_DOWN;
	(*CurrentGame).addToScene(this->nextScene->bg, LayerType::BackgroundLayer, IndependantType::Background);
	(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
	/*if (this->nextScene->direction != sf::Vector2i(0, 0))
	{
	(*CurrentGame).SetScrollingDirection(this->nextScene->direction);
	}
	*/
}

void InGameState::Transition2_2()
{
	this->currentScene->bg->GarbageMe = true;
	this->nextScene->bg->speed.x = - this->nextScene->direction.x * this->nextScene->vspeed;
	this->nextScene->bg->speed.y = this->nextScene->direction.y * this->nextScene->vspeed;
	this->playerShip->disable_inputs = false;
	this->playerShip->speed = sf::Vector2f(0, 0);
	if (this->nextScene->direction == sf::Vector2i(0, 0))
	{
		this->IG_State = InGameStateMachine::HUB_ROAMING;
	}
	else
	{
		this->IG_State = InGameStateMachine::SCROLLING;
		this->playerShip->disable_fire = false;
	}
	*this->currentScene = *this->nextScene;
	(*CurrentGame).SetScrollingDirection(this->currentScene->direction);
}

sf::Vector2f InGameState::StartingShipPosition(sf::Vector2i direction)
{
	sf::Vector2f pos = sf::Vector2f((SCENE_SIZE_X / 2), (SCENE_SIZE_Y / 2));

	if (direction == sf::Vector2i(0, 1))
	{
		pos.x = STARTSCENE_X_RATIO*SCENE_SIZE_X;
		pos.y = STARTSCENE_Y_RATIO*SCENE_SIZE_Y;
	}

	else if (direction == sf::Vector2i(0, -1))
	{
		pos.x = STARTSCENE_X_RATIO*SCENE_SIZE_X;
		pos.y = (1 - STARTSCENE_Y_RATIO)*SCENE_SIZE_Y;

	}
	else if (direction == sf::Vector2i(1, 0))
	{
		pos.x = (1 - STARTSCENE_Y_RATIO)*SCENE_SIZE_X;
		pos.y = STARTSCENE_X_RATIO*SCENE_SIZE_Y;

	}
	else if (direction == sf::Vector2i(-1, 0))
	{
		pos.x = STARTSCENE_Y_RATIO*SCENE_SIZE_X;
		pos.y = STARTSCENE_X_RATIO*SCENE_SIZE_Y;
	}

	return pos;
}