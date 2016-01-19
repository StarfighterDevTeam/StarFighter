#include "Game.h"
#define stringify(x)  #x

const char* GameObjectTypeValues[] =
{
	stringify(BackgroundObject),
	stringify(PlayerShip),
	stringify(FriendlyFire),
	stringify(EnemyFire),
	stringify(EnemyObject)
};

void Game::init(RenderWindow* window)
{
	this->window = window;
	this->mainScreen.create(REF_WINDOW_RESOLUTION_X - HUD_PANEL_SIZE_X - (USE_SPLIT_SCREEN * HUD_PANEL_SIZE_X), REF_WINDOW_RESOLUTION_Y, false);
	this->mainScreen.setSmooth(true);

	scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	//HUD
	m_module_HUD = new GameObject(sf::Vector2f(screen_size.x + 128 / 2, screen_size.y / 2), sf::Vector2f(0, 0), "Assets/2D/Modules_HUD.png", sf::Vector2f(HUD_PANEL_SIZE_X, HUD_PANEL_SIZE_Y));
	m_module_HUD->setColor(sf::Color(255, 255, 255, 200));
	m_module_HUD->setOrigin(sf::Vector2f(0, 0));
	m_module_HUD->setPosition(sf::Vector2f(0, 0));

	this->hudScreen.create(HUD_PANEL_SIZE_X, REF_WINDOW_RESOLUTION_Y, false);
	this->hudScreen.setSmooth(true);

	if (USE_SPLIT_SCREEN == true || SHARED_VIEW == true)
	{
		this->hudScreen_SplitScreen.create(HUD_PANEL_SIZE_X, REF_WINDOW_RESOLUTION_Y, false);
		this->hudScreen_SplitScreen.setSmooth(true);
	}

	for (int v = 0; v < 1 + USE_SPLIT_SCREEN; v++)
	{
		view[v].setCenter(sf::Vector2f((1 + 2 * USE_SPLIT_SCREEN) * REF_WINDOW_RESOLUTION_X / 2 / (1 + USE_SPLIT_SCREEN), REF_WINDOW_RESOLUTION_Y / 2));
		view[v].setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X / (1 + USE_SPLIT_SCREEN), REF_WINDOW_RESOLUTION_Y));
		if (USE_SPLIT_SCREEN == true)
		{
			view[v].setViewport(sf::FloatRect(v * 0.5f, 0, 0.5f, 1));
		}
		view[v].zoom(1.777777);
	}

	//default value
	map_size = (sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

	//initialization of players
	for (int i = 0; i < NBVAL_PlayerTeams; i++)
	{
		playerShips[i] = NULL;
	}
	
	//fonts
	font = new sf::Font();

	if (!font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
	{
		// error
		//TODO: font loader
	}
	else
	{
		m_fonts[Font_Terminator] = font;
	}

	font2 = new sf::Font();
	if (!font2->loadFromFile("Assets/Fonts/arial.ttf"))
	{
		// error
		//TODO: font loader
	}
	else
	{
		m_fonts[Font_Arial] = font2;
	}

	//TODO: save in local preferences
	m_Music_Activated = true;
	m_SFX_Activated = false;

	//Sounds
	LoadSFX();

	//Music
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading Musics");
	//if (!SpaceCowboys.openFromFile("Assets/Music/SpaceCowboys.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/CrimeaDigital.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/Rebecca.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/Daft Punk - Derezzed.ogg"))
	//if (m_curMusic.openFromFile("Assets/Music/Tron_End_Titles.ogg"))
	if (m_curMusic.openFromFile("Assets/Music/Tyrian_Remix/technoguyx_Sarah's_Song_(remix).ogg"))
	{
		m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * m_Music_Activated);
		m_curMusic.play();
		m_curMusic.setLoop(true);
	}

	//FLUX SPECIFIC
	for (int i = 0; i < GRID_WIDTH; i++)
	{
		for (int j = 0; j < GRID_HEIGHT; j++)
		{
			m_module_grid[i][j] = NULL;
		}
	}
}

void Game::SetSFXVolume(bool activate_sfx)
{
	soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * activate_sfx);
}

int Game::LoadSFX()
{
	if (!soundBuffers[0].loadFromFile("Assets/Sounds/laser.ogg"))
		return -1;

	soundsLaser[0].setBuffer(soundBuffers[0]);

	soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);

	return 0;
}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	if (sfx_name == SFX_Laser)
	{
		soundsLaser[0].play();
	}
}

sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::addToScene(GameObject *object, LayerType layer, GameObjectType type)
{
	object->m_layer = layer;
	object->m_collider_type = type;

	if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_GameObject))
	{
		AddGameObjectToVector(object, &this->sceneGameObjectsTyped[(int)type]);
		AddGameObjectToVector(object, &this->sceneGameObjectsLayered[(int)layer]);
		AddGameObjectToVector(object, &this->sceneGameObjects);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->getName().c_str(), (int)layer));
	}
}

void Game::addToFeedbacks(SFRectangle* feedback, LayerType layer)
{
	if (feedback)
	{
		AddGameObjectToVector(feedback, &this->sceneFeedbackBars[layer]);
		feedback->m_layer = layer;
	}
		
}

void Game::addToFeedbacks(SFText* text)
{
	if (text)
		AddGameObjectToVector(text, &this->sceneFeedbackSFTexts);
}

void Game::addToFeedbacks(SFGauge* gauge)
{
	if (gauge)
		AddGameObjectToVector(gauge, &this->sceneFeedbackSFGauge);
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneGameObjects.size(), this->garbage.size());

	//TODO: Updating screen resolution
	scale_factor.x = 1.0f * screen_size.x / REF_WINDOW_RESOLUTION_X;
	scale_factor.y = 1.0f * screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Clean garbage
	cleanGarbage();

	//Checking colisions
	colisionChecksV2();
	//ResolveProductionBufferList();//must be done after collision checks

	size_t sceneGameObjectsSize = this->sceneGameObjects.size();
	for (size_t i = 0; i < sceneGameObjectsSize; i++)
	{
		if (this->sceneGameObjects[i] == NULL)
			continue;

		this->sceneGameObjects[i]->update(deltaTime);
	}

	size_t sceneTextPopFeedbacksSize = this->sceneFeedbackSFTexts.size();
	for (size_t i = 0; i < sceneTextPopFeedbacksSize; i++)
	{
		if (this->sceneFeedbackSFTexts[i] == NULL)
			continue;

		this->sceneFeedbackSFTexts[i]->update(deltaTime);
	}

	//Collect the dust
	collectGarbage();
}

void Game::ResolveProductionBufferList()
{
	size_t scenePlayerShipsVectorSize = this->sceneGameObjectsTyped[PlayerShip].size();
	for (size_t i = 0; i < scenePlayerShipsVectorSize; i++)
	{
		if (this->sceneGameObjectsTyped[PlayerShip][i] == NULL)
			continue;
		
		this->sceneGameObjectsTyped[PlayerShip][i]->ResolveProductionBufferList();
	}

	size_t sceneModulesVectorSize = this->sceneGameObjectsTyped[ModuleObject].size();
	for (size_t i = 0; i < sceneModulesVectorSize; i++)
	{
		if (this->sceneGameObjectsTyped[ModuleObject][i] == NULL)
			continue;

		this->sceneGameObjectsTyped[ModuleObject][i]->ResolveProductionBufferList();
	}
}

void Game::drawScene()
{
	this->mainScreen.clear();

	for (int v = 0; v < 1 + USE_SPLIT_SCREEN; v++)//Split screen
	{
		mainScreen.setView(view[v]);

		for (int i = 0; i < NBVAL_Layer; i++)
		{
			if (i == FeedbacksLayer)
			{
				for (std::vector<SFText*>::iterator it = this->sceneFeedbackSFTexts.begin(); it != this->sceneFeedbackSFTexts.end(); it++)
				{
					if (*it == NULL)
						continue;

					if ((*(*it)).m_visible)
					{
						if ((v == 0 && ((*(*it)).m_alliance != Alliance1 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == true))
							continue;

						if ((v == 1 && ((*(*it)).m_alliance != Alliance2 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == true))
							continue;

						mainScreen.draw(*(*it));
					}
				}
				for (std::vector<SFGauge*>::iterator it = this->sceneFeedbackSFGauge.begin(); it != this->sceneFeedbackSFGauge.end(); it++)
				{
					if (*it == NULL)
						continue;

					if ((*(*it)).m_visible)
					{
						if ((v == 0 && ((*(*it)).m_alliance != Alliance1 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == true))
							continue;

						if ((v == 1 && ((*(*it)).m_alliance != Alliance2 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == true))
							continue;

						(*(*it)).draw(mainScreen);
					}
				}
			}
			else if (i == FakeGridLayer)
			{
				std::vector<GameObject*>::iterator it = this->sceneGameObjectsLayered[i].begin();

				if (*it)
				{
					for (size_t j = 0; j < GRID_WIDTH; j++)
					{
						for (size_t k = 0; k < GRID_HEIGHT; k++)
						{
							(*it)->setPosition(sf::Vector2f(j*TILE_SIZE + TILE_SIZE / 2, k*TILE_SIZE + TILE_SIZE / 2));
							this->mainScreen.draw((*(*it)));
						}
					}
				}
			}

			else
			{
				//feedbacks
				for (std::vector<SFRectangle*>::iterator it = this->sceneFeedbackBars[i].begin(); it != this->sceneFeedbackBars[i].end(); it++)
				{
					if (*it == NULL)
						continue;

					if ((*(*it)).m_visible)
					{
						if ((v == 0 && ((*(*it)).m_alliance != Alliance1 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == true) && SHARED_VIEW == false)
							continue;

						if ((v == 1 && ((*(*it)).m_alliance != Alliance2 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == true) && SHARED_VIEW == false)
							continue;

						if ((((*(*it)).m_alliance != Alliance1 && (*(*it)).m_alliance != AllianceNeutral) && USE_SPLIT_SCREEN == false) && SHARED_VIEW == false)
							continue;

						mainScreen.draw(*(*it));
					}
				}

				//game objects
				for (std::vector<GameObject*>::iterator it = this->sceneGameObjectsLayered[i].begin(); it != this->sceneGameObjectsLayered[i].end(); it++)
				{
					if (*it == NULL)
						continue;
					
					if ((*(*it)).m_visible)
					{
						this->mainScreen.draw((*(*it)));
					}
				}
			}
		}
	}

	this->mainScreen.display();
	sf::Sprite temp(this->mainScreen.getTexture());
	temp.scale(scale_factor.x, scale_factor.y);
	temp.setPosition(sf::Vector2f(HUD_PANEL_SIZE_X * scale_factor.x, 0));
	this->window->draw(temp);
}

void Game::drawHud()
{
	this->hudScreen.clear();

	if (m_module_HUD)
	{
		//draw background hud image
		hudScreen.draw(*m_module_HUD);

		if (USE_SPLIT_SCREEN == true || SHARED_VIEW == true)
		{
			hudScreen_SplitScreen.draw(*m_module_HUD);
		}

		//draw production feedbacks
		for (int v = 0; v < 1 + (USE_SPLIT_SCREEN || SHARED_VIEW); v++)
		{
			for (std::vector<SFRectangle*>::iterator it = this->m_HUD_productions_mask[v].begin(); it != this->m_HUD_productions_mask[v].end(); it++)
			{
				if (*it == NULL)
					continue;
		
				if ((*(*it)).m_visible)
				{
					if (v == 0)
						this->hudScreen.draw((*(*it)));
					else
						this->hudScreen_SplitScreen.draw((*(*it)));
				}
			}
		}
		
		hudScreen.display();
		sf::Sprite temp(hudScreen.getTexture());
		temp.scale(scale_factor.x, scale_factor.y);
		temp.setPosition(0, 0);
		this->window->draw(temp);

		if (USE_SPLIT_SCREEN == true || SHARED_VIEW == true)
		{
			hudScreen_SplitScreen.display();
			sf::Sprite tempSplit(hudScreen_SplitScreen.getTexture());
			tempSplit.scale(scale_factor.x, scale_factor.y);
			tempSplit.setPosition(screen_size.x - (HUD_PANEL_SIZE_X * scale_factor.x), 0);
			this->window->draw(tempSplit);
		}
	}
}

void Game::colisionChecksV2()
{
	sf::Clock dt;
	dt.restart();

	//Player interactions with modules and fluxors
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[PlayerShip].begin(); it1 != sceneGameObjectsTyped[PlayerShip].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Player eating Fluxors (unguided)
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[FluxorUnguidedObject].begin(); it2 != sceneGameObjectsTyped[FluxorUnguidedObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;
			
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				(*it1)->GetFluxor(*it2);
			}
		}

		//Player interacting with Module
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[ModuleObject].begin(); it2 != sceneGameObjectsTyped[ModuleObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if ((*(*it2)).m_under_construction && (*(*it1)).m_team != (*(*it2)).m_team)
				continue;

			if ((*(*it1)).m_alliance!= (*(*it2)).m_alliance)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				(*it1)->GetModule(*it2);
			}
		}
	}

	//Fluxors interactions (guided)
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[FluxorGuidedObject].begin(); it1 != sceneGameObjectsTyped[FluxorGuidedObject].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		bool has_collided = false;
		//Fluxors interactions with Shields (before interacting with Modules).
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[ShieldObject].begin(); it2 != sceneGameObjectsTyped[ShieldObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Attack them maybe? (before attacking Modules inside it)
				(*it2)->GetFluxor(*it1);
				if (!(*it2)->m_under_construction)
				{
					has_collided = true;
				}
			}
		}

		//Fluxors interactions with Modules
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[ModuleObject].begin(); it2 != sceneGameObjectsTyped[ModuleObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Attack them maybe?
				(*it2)->GetFluxor(*it1);
				if (!(*it2)->m_under_construction)
				{
					has_collided = true;
				}
			}
		}

		//Check if we are allowed to continue
		if (!has_collided)
		{
			(*it1)->CheckCondensation();
		}

		//Fluxors eating each others
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[FluxorGuidedObject].begin(); it2 != sceneGameObjectsTyped[FluxorGuidedObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			if (*it1 == *it2)
				continue;

			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				(*it2)->GetFluxor(*it1);
			}
		}
	}

	//Fluxors interactions (unguided)
	for (std::vector<GameObject*>::iterator it1 = sceneGameObjectsTyped[FluxorUnguidedObject].begin(); it1 != sceneGameObjectsTyped[FluxorUnguidedObject].end(); it1++)
	{
		if (*it1 == NULL)
			continue;

		//Fluxors interactions with Modules
		for (std::vector<GameObject*>::iterator it2 = sceneGameObjectsTyped[ModuleObject].begin(); it2 != sceneGameObjectsTyped[ModuleObject].end(); it2++)
		{
			if (*it2 == NULL)
				continue;

			//if (GameObject::DistancePointToSement((*it2)->getPosition().x, (*it2)->getPosition().y, (*it1)->m_initial_position.x, (*it1)->m_initial_position.y, (*it1)->getPosition().x, (*it1)->getPosition().y) == 0)
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				//Do something 
				(*it2)->GetFluxor(*it1);
			}
		}
	}
	//printf("| Collision: %d \n",dt.getElapsedTime().asMilliseconds());
}

void Game::cleanGarbage()
{
	sf::Clock dt;
	dt.restart();

	// On "cache" les size, pour éviter d'appeler des fonctions à chaque itération
	const size_t garbageSize = this->garbage.size();
	const size_t sceneGameObjectsSize = this->sceneGameObjects.size();
	//Size layer
	size_t sceneGameObjectsLayeredSize[NBVAL_Layer];
	for (int layer = 0; layer < NBVAL_Layer; layer++)
	{
		sceneGameObjectsLayeredSize[layer] = this->sceneGameObjectsLayered[layer].size();
	}
	//Size ind type
	size_t sceneGameObjectsTypedSize[NBVAL_GameObject];
	for (int layer = 0; layer < NBVAL_GameObject; layer++)
	{
		sceneGameObjectsTypedSize[layer] = this->sceneGameObjectsTyped[layer].size();
	}

	//Scene GameObjects
	for (size_t i = 0; i < garbageSize; i++)
	{
		GameObject*    pCurGameObject = this->garbage[i];

		// On remet à NULL lorsqu'on a trouvé un élément à dégager
		for (size_t j = 0; j < sceneGameObjectsSize; j++)
		{
			if (this->sceneGameObjects[j] == pCurGameObject)
			{
				this->sceneGameObjects[j] = NULL;
				break;
			}
		}

		// "layered"...
		const int layer = pCurGameObject->m_layer;
		for (size_t j = 0; j < sceneGameObjectsLayeredSize[layer]; j++)
		{
			if (this->sceneGameObjectsLayered[layer][j] == pCurGameObject)
			{
				this->sceneGameObjectsLayered[layer][j] = NULL;
				break;
			}
		}

		// "typed"
		const int type = pCurGameObject->m_collider_type;
		for (size_t j = 0; j < sceneGameObjectsTypedSize[type]; j++)
		{
			if (this->sceneGameObjectsTyped[type][j] == pCurGameObject)
			{
				this->sceneGameObjectsTyped[type][j] = NULL;
				break;
			}
		}

		//destructor function ??

		// A la fin, on delete l'élément
		delete pCurGameObject;
	}

	//Now deleting texts and feedbacks
	const size_t garbageTextsSize = this->garbageTexts.size();
	for (size_t i = 0; i < garbageTextsSize; i++)
	{
		SFText*    pSFText = this->garbageTexts[i];

		const size_t VectorTextsSize = this->sceneFeedbackSFTexts.size();
		for (size_t j = 0; j < VectorTextsSize; j++)
		{
			if (this->sceneFeedbackSFTexts[j] == pSFText)
			{
				this->sceneFeedbackSFTexts[j] = NULL;
				break;
			}
		}

		delete pSFText;
	}

	
	const size_t garbageRectangleSize = this->garbageRectangleShapes.size();
	for (size_t i = 0; i < garbageRectangleSize; i++)
	{
		SFRectangle*    pRectangle = this->garbageRectangleShapes[i];
		const int layer = pRectangle->m_layer;

		const size_t VectorRectanglesSize = this->sceneFeedbackBars[layer].size();
		for (size_t j = 0; j < VectorRectanglesSize; j++)
		{
			if (this->sceneFeedbackBars[layer][j] == pRectangle)
			{
				this->sceneFeedbackBars[layer][j] = NULL;
				break;
			}
		}

		delete pRectangle;
	}

	const size_t garbagegaugeSize = this->garbageGauges.size();
	for (size_t i = 0; i < garbagegaugeSize; i++)
	{
		SFGauge*    pGauge = this->garbageGauges[i];

		const size_t VectorGaugeSize = this->sceneFeedbackSFGauge.size();
		for (size_t j = 0; j < VectorGaugeSize; j++)
		{
			if (this->sceneFeedbackSFGauge[j] == pGauge)
			{
				this->sceneFeedbackSFGauge[j] = NULL;
				break;
			}
		}

		delete pGauge;
	}

	//printf("| Clean: %d ",dt.getElapsedTime().asMilliseconds());
}

void Game::AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pGameObject;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pGameObject);
}

void Game::AddGameObjectToVector(SFRectangle* pRectangleShape, vector<SFRectangle*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pRectangleShape;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pRectangleShape);
}

void Game::AddGameObjectToVector(SFText* pSFText, vector<SFText*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pSFText;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pSFText);
}

void Game::AddGameObjectToVector(SFGauge* pSFGauge, vector<SFGauge*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pSFGauge;
			return; // ayé, on a trouvé un free slot, inséré, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas où on n'a pas trouvé de free slot => on rajoute à la fin
	vector->push_back(pSFGauge);
}

void Game::collectGarbage()
{
	sf::Clock dt;
	dt.restart();

	this->garbage.clear();
	this->garbageTexts.clear();
	this->garbageRectangleShapes.clear();

	for (std::vector<GameObject*>::iterator it = (this->sceneGameObjects).begin(); it != (this->sceneGameObjects).end(); it++)
	{
		if (*it == NULL)
			continue;

		//Content flagged for deletion
		if ((**it).m_GarbageMe)
		{
			this->garbage.push_back(*it);
			continue;
		}

		if (!(**it).m_isOnScene)
		{
			//objects that are spawning out of screen are not deleted
			if (((**it).getPosition().x + ((**it).m_size.x) / 2 >= 0 && (**it).getPosition().x - ((**it).m_size.x) / 2 <= map_size.x) && ((**it).getPosition().y + ((**it).m_size.y) / 2 >= 0 && (**it).getPosition().y - ((**it).m_size.y) / 2 <= map_size.y))
			{
				(**it).m_isOnScene = true;
			}
		}

		//Content that went on scene and then exited have to be deleted
		if (!(**it).m_DontGarbageMe && (**it).m_isOnScene)
		{
			if ((**it).getPosition().x + ((**it).m_size.x) / 2 < 0 || (**it).getPosition().x - ((**it).m_size.x) / 2 > map_size.x
				|| (**it).getPosition().y + ((**it).m_size.y) / 2 < 0 || (**it).getPosition().y - ((**it).m_size.y) / 2 > map_size.y)
			{
				this->garbage.push_back(*it);
				continue;
			}
		}
	}

	//Texts and feedbacks
	for (std::vector<SFText*>::iterator it = (this->sceneFeedbackSFTexts).begin(); it != (this->sceneFeedbackSFTexts).end(); it++)
	{
		if (*it == NULL)
			continue;

		//Content flagged for deletion
		if ((**it).m_GarbageMe)
		{
			this->garbageTexts.push_back(*it);
			continue;
		}
	}
	for (size_t layer = 0; layer < NBVAL_Layer; layer++)
	{
		for (std::vector<SFRectangle*>::iterator it = (this->sceneFeedbackBars[layer]).begin(); it != (this->sceneFeedbackBars[layer]).end(); it++)
		{
			if (*it == NULL)
				continue;

			//Content flagged for deletion
			if ((**it).m_GarbageMe)
			{
				this->garbageRectangleShapes.push_back(*it);
				continue;
			}
		}
	}
	for (std::vector<SFGauge*>::iterator it = (this->sceneFeedbackSFGauge).begin(); it != (this->sceneFeedbackSFGauge).end(); it++)
	{
		if (*it == NULL)
			continue;

		//Content flagged for deletion
		if ((**it).m_GarbageMe)
		{
			this->garbageGauges.push_back(*it);
			continue;
		}
	}

	//printf("| Collect: %d ",dt.getElapsedTime().asMilliseconds());
}

bool Game::isCellFree(sf::Vector2f position)
{
	sf::Vector2u grid_index = GetGridIndex(position);

	return !m_module_grid[grid_index.x][grid_index.y];
}

sf::Vector2u Game::GetGridIndex(sf::Vector2f position)
{
	unsigned int grid_line = (unsigned int)(ceil(1.f * position.x / TILE_SIZE)) - 1;
	unsigned int grid_row = (unsigned int)(ceil(1.f * position.y / TILE_SIZE)) - 1;

	return sf::Vector2u(grid_line, grid_row);
}

sf::Vector2f Game::GridToPosition(sf::Vector2u grid_index)
{
	return sf::Vector2f(grid_index.x*TILE_SIZE + TILE_SIZE / 2, grid_index.y*TILE_SIZE + TILE_SIZE / 2);
}

GameObject* Game::GetClosestObject(const sf::Vector2f position, GameObjectType type_of_closest_object)
{
	float shortest_distance = -1;
	GameObject* returned_obj = NULL;
	for (std::vector<GameObject*>::iterator it = sceneGameObjectsTyped[type_of_closest_object].begin(); it != sceneGameObjectsTyped[type_of_closest_object].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_isOnScene && !(*it)->m_ghost && (*it)->m_visible)
		{
			const float a = position.x - (*it)->getPosition().x;
			const float b = position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				returned_obj = (*it);
			}
		}
	}

	return returned_obj;
}

GameObject* Game::GetClosestObject(const GameObject* ref_obj, GameObjectType type_of_closest_object)
{
	const sf::Vector2f ref_position = ref_obj->getPosition();
	
	return GetClosestObject(ref_position, type_of_closest_object);
}

std::vector<GameObject*> Game::GetSceneGameObjectsTyped(GameObjectType type)
{
	return sceneGameObjectsTyped[type];
}

std::vector<GameObject*> Game::GetSceneGameObjectsLayered(LayerType layer)
{
	return sceneGameObjectsLayered[layer];
}

std::vector<SFRectangle*> Game::GetSceneSFRectanglesLayered(LayerType layer)
{
	return sceneFeedbackBars[layer];
}

//ol Game::isAlly(PlayerTeams ref_team, PlayerTeams other_team)
//
//eturn ref_team != other_team && find(m_player_alliances[ref_team].begin(), m_player_alliances[ref_team].end(), other_team) != m_player_alliances[ref_team].end();
//

TeamAlliances Game::GetTeamAlliance(PlayerTeams team)
{
	for (size_t i = 0; i < NBVAL_TeamAlliances; i++)
	{
		if (find(m_teams_vs_alliance_map[i].begin(), m_teams_vs_alliance_map[i].end(), team) != m_teams_vs_alliance_map[i].end())
		{
			return (TeamAlliances)i;
		}
	}

	printf("<!> Game::GetTeamAlliance(PlayerTeams team) is looking for a team assigned to no alliance in vector<vector<int>> Game::m_player_alliances.\n");
	return (TeamAlliances)0;
}