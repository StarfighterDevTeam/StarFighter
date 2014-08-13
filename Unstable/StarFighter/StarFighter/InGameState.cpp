#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	this->currentScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//first_scene = true
	this->nextScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//first_scene = true
	(*CurrentGame).SetScrollingDirection(this->currentScene->direction);
	if (this->currentScene->direction != sf::Vector2i(0, 0))
	{
		this->IG_State = InGameStateMachine::SCROLLING;
	}
	else
	{
		this->IG_State = InGameStateMachine::HUB_ROAMING;
	}

	this->playerShip = new Ship(Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO), *FileLoader::LoadShipConfig("default"));
	(*CurrentGame).SetPlayerShip(this->playerShip);

	LOGGER_WRITE(Logger::Priority::DEBUG, "Playership loaded\n");

	//bg
	(*CurrentGame).addToScene(this->currentScene->bg, LayerType::BackgroundLayer, IndependantType::Background);

	//ship
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, IndependantType::PlayerShip);
	playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
	playerShip->ship_config.GenerateFakeShip((*CurrentGame).playerShip);

	hud = new PlayerHud();
	hud->Init(this->playerShip->ship_config.getShipConfigArmor(), this->playerShip->ship_config.getShipConfigShield());
}

void InGameState::Update(Time deltaTime)
{
	this->currentScene->Update(deltaTime);
	InGameStateMachineCheck();

	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), this->currentScene->getSceneBeastScore());
	(*CurrentGame).updateScene(deltaTime);
	this->mainWindow->clear();

	hud->Update(this->mainWindow, (*CurrentGame).playerShip->ship_config.getShipConfigArmor(), (*CurrentGame).playerShip->ship_config.getShipConfigShield(), (*CurrentGame).playerShip->getMoney(),
		(*CurrentGame).playerShip->getGrazeCount(), (*CurrentGame).getHazard(), this->currentScene->getSceneHazardBreakValue(), deltaTime);
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
	hud->Draw(this->mainWindow);
}

void InGameState::Release()
{
	//TODO
}

void InGameState::LoadNextScene(string m_nextScene_name)
{
	//free(this->nextScene);
	this->nextScene = new Scene(m_nextScene_name);
}

void InGameState::InGameStateMachineCheck()
{
	float w = this->currentScene->bg->m_size.x;
	float h = this->currentScene->bg->m_size.y;
	float w_ = this->playerShip->m_size.x;
	float h_ = this->playerShip->m_size.y;

	switch (this->IG_State)
	{
	case InGameStateMachine::SCROLLING:
	{
										  if ((*CurrentGame).direction == sf::Vector2i(0, 1))
										  {
											  if (this->currentScene->bg->getPosition().y > h / 2)
											  {
												  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  this->currentScene->bg->setPosition(sf::Vector2f(this->currentScene->bg->getPosition().x, h / 2));
												  this->currentScene->bg->speed = sf::Vector2f(0, 0);
												  this->currentScene->generating_enemies = false;
												  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_UP]);

												  printf("PHASE LAST SCREEN\n");
											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(0, -1))
										  {
											  if (this->currentScene->bg->getPosition().y < (-h / 2) + SCENE_SIZE_Y)
											  {
												  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  this->currentScene->bg->setPosition(sf::Vector2f(this->currentScene->bg->getPosition().x, (-h / 2) + SCENE_SIZE_Y));
												  this->currentScene->bg->speed = sf::Vector2f(0, 0);
												  this->currentScene->generating_enemies = false;
												  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_DOWN], true);
											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(1, 0))
										  {
											  if (this->currentScene->bg->getPosition().x < (-w / 2) + SCENE_SIZE_X)
											  {
												  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  this->currentScene->bg->setPosition(sf::Vector2f((-w / 2) + SCENE_SIZE_X, this->currentScene->bg->getPosition().y));
												  this->currentScene->bg->speed = sf::Vector2f(0, 0);
												  this->currentScene->generating_enemies = false;
												  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_RIGHT]);
											  }
										  }
										  else if ((*CurrentGame).direction == sf::Vector2i(-1, 0))
										  {
											  if (this->currentScene->bg->getPosition().x > w / 2)
											  {
												  this->IG_State = InGameStateMachine::LAST_SCREEN;
												  this->currentScene->bg->setPosition(sf::Vector2f(w / 2, this->currentScene->bg->getPosition().y));
												  this->currentScene->bg->speed = sf::Vector2f(0, 0);
												  this->currentScene->generating_enemies = false;
												  this->nextScene->LoadSceneFromFile(this->currentScene->links[Directions::DIRECTION_LEFT], true);
											  }
										  }
										  break;

	case InGameStateMachine::LAST_SCREEN:
	{
											if ((*CurrentGame).isLastEnemyDead())
											{
												this->IG_State = InGameStateMachine::TRANSITION_PHASE1_2;
												this->playerShip->disable_inputs = true;
												this->playerShip->disable_fire = true;

												this->playerShip->speed.x = (*CurrentGame).direction.x * ENDSCENE_TRANSITION_SPEED_UP;
												this->playerShip->speed.y = -(*CurrentGame).direction.y * ENDSCENE_TRANSITION_SPEED_UP;

												printf("PHASE TRANSITION 1\n");
											}
											break;

	}
	case InGameStateMachine::TRANSITION_PHASE1_2:
	{
													if ((*CurrentGame).direction == sf::Vector2i(0, 1))
													{
														if (this->playerShip->getPosition().y <= h_ / 2)
														{
															this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
															this->playerShip->setPosition(sf::Vector2f(this->playerShip->getPosition().x, h_ / 2));
															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_X / 2) - playerShip->getPosition().x) / SCENE_SIZE_Y;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_Y / 2) - playerShip->getPosition().y) / SCENE_SIZE_Y;
															this->currentScene->bg->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN;
															this->nextScene->bg->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN;
															(*CurrentGame).addToScene(this->nextScene->bg, LayerType::BackgroundLayer, IndependantType::Background);
															(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);

															printf("PHASE TRANSITION 2\n");
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(0, -1))
													{
														if (this->playerShip->getPosition().y >= SCENE_SIZE_Y - (h_ / 2))
														{
															this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
															this->playerShip->setPosition(sf::Vector2f(this->playerShip->getPosition().x, SCENE_SIZE_Y - (h_ / 2)));
															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_X / 2) - playerShip->getPosition().x) / SCENE_SIZE_Y;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_Y / 2) - playerShip->getPosition().y) / SCENE_SIZE_Y;
															this->currentScene->bg->speed.y = -ENDSCENE_TRANSITION_SPEED_DOWN;
															this->nextScene->bg->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN;
															(*CurrentGame).addToScene(this->nextScene->bg, LayerType::BackgroundLayer, IndependantType::Background);
															(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(1, 0))
													{
														if (this->playerShip->getPosition().x >= SCENE_SIZE_X - (w_ / 2))
														{
															this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
															this->playerShip->setPosition(sf::Vector2f(this->playerShip->getPosition().x, SCENE_SIZE_X - (w_ / 2)));
															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_X / 2) - playerShip->getPosition().x) / SCENE_SIZE_Y;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_Y / 2) - playerShip->getPosition().y) / SCENE_SIZE_Y;
															this->currentScene->bg->speed.x = -ENDSCENE_TRANSITION_SPEED_DOWN;
															this->nextScene->bg->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN;
															(*CurrentGame).addToScene(this->nextScene->bg, LayerType::BackgroundLayer, IndependantType::Background);
															(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(-1, 0))
													{
														if (this->playerShip->getPosition().x <= w_ / 2)
														{
															this->IG_State = InGameStateMachine::TRANSITION_PHASE2_2;
															this->playerShip->setPosition(sf::Vector2f(this->playerShip->getPosition().x, w_ / 2));
															this->playerShip->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_X / 2) - playerShip->getPosition().x) / SCENE_SIZE_Y;
															this->playerShip->speed.y = ENDSCENE_TRANSITION_SPEED_DOWN * ((SCENE_SIZE_Y / 2) - playerShip->getPosition().y) / SCENE_SIZE_Y;
															this->currentScene->bg->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN;
															this->nextScene->bg->speed.x = ENDSCENE_TRANSITION_SPEED_DOWN;
															(*CurrentGame).addToScene(this->nextScene->bg, LayerType::BackgroundLayer, IndependantType::Background);
															(*CurrentGame).garbageLayer(LayerType::FriendlyFireLayer);
														}
													}
													break;
	}

	case InGameStateMachine::TRANSITION_PHASE2_2:
	{
													if ((*CurrentGame).direction == sf::Vector2i(0, 1))
													{
														if (this->currentScene->bg->getPosition().y > SCENE_SIZE_Y + (h / 2))
														{
															this->nextScene->bg->speed.y = this->nextScene->vspeed;
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

															printf("PHASE SCROLLING\n");
														}
													}
													else if ((*CurrentGame).direction == sf::Vector2i(0, -1))
													{
														if (this->currentScene->bg->getPosition().y < -h/2)
														{
															this->currentScene->bg->GarbageMe = true;

															this->nextScene->bg->speed.y = - this->nextScene->vspeed;
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
													}
													else if ((*CurrentGame).direction == sf::Vector2i(1, 0))
													{
														if (this->currentScene->bg->getPosition().x < -w/2)
														{
															this->currentScene->bg->GarbageMe = true;

															this->nextScene->bg->speed.x = - this->nextScene->vspeed;
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
													}
													else if ((*CurrentGame).direction == sf::Vector2i(-1, 0))
													{
														if (this->currentScene->bg->getPosition().x > SCENE_SIZE_X + (w/2))
														{
															this->currentScene->bg->GarbageMe = true;

															this->nextScene->bg->speed.x = this->nextScene->vspeed;
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
													}
													
													break;
	}

	case InGameStateMachine::HUB_ROAMING:
	{

											break;
	}

	default:
		break;
	}



	}


}