#include "Game.h"
#define stringify(x)  #x

const char* ColliderTypeValues[] =
{
	stringify(BackgroundObject),
	stringify(PlayerShip),
	stringify(FriendlyFire),
	stringify(EnemyFire),
	stringify(EnemyObject)
};

Game::Game(RenderWindow* window)
{
	m_playerShip = NULL;
	m_pause = false;
	m_window_has_focus = true;

	m_window = window;
	m_mainScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	m_mainScreen.setSmooth(true);

	m_scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	m_screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	m_view.setCenter(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	m_view.setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

	//default value
	m_map_size = (sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));

	//fonts
	m_font[Font_Terminator] = new sf::Font();
	if (!m_font[Font_Terminator]->loadFromFile(makePath("Fonts/terminator_real_nfi.ttf")))
	{
		// error
		//TODO: font loader
	}

	m_font[Font_Arial] = new sf::Font();
	if (!m_font[Font_Arial]->loadFromFile(makePath("Fonts/arial.ttf")))
	{
		// error
		//TODO: font loader
	}

	//Sounds
	m_SFX_Activated = true;
	LoadSFX();

	//Music
	printf("Loading Musics");
	m_Music_Activated = true;
	m_music_fader = 0;
	PlayMusic(Music_Main);

	//background
	m_background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y));
	m_background->setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	addToScene(m_background, BackgroundLayer, BackgroundObject);

	//Star Hunter
	//AddToStarSectorsKnown(m_current_star_sector);
	//UpdateSectorList(true);

	//DEBUG
	m_sector_debug_current = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Blue, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);
	m_sector_debug_onscreen = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Green, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);
}

Game::~Game()
{
	delete m_background;

	delete m_sector_debug_current;
	delete m_sector_debug_onscreen;
}

void Game::SetSFXVolume(bool activate_sfx)
{
	m_sounds[0].setVolume(DEFAULT_SFX_VOLUME * activate_sfx);
}

int Game::LoadSFX()
{
	if (!m_soundBuffers[0].loadFromFile(makePath("Sounds/laser.ogg")))
		return -1;

	m_sounds[0].setBuffer(m_soundBuffers[0]);

	m_sounds[0].setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);
	//soundsSwitch.setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);

	return 0;
}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	if (m_SFX_Activated)
		m_sounds[sfx_name].play();
}

void Game::SetMusicVolume(bool activate_music)
{
	m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * activate_music);
}

void Game::PlayMusic(Music_Bank music, string specific_filename)
{
	if (music == m_curMusic_type)
	{
		return;
	}

	m_curMusic_type = music;

	if (!m_Music_Activated)
		return;

	//filename has been specified?
	if (!specific_filename.empty())
	{
		m_next_music_name = specific_filename;
	}
	//default musics
	else
	{
		//choose the right music file
		switch (music)
		{
			case Music_Main:
			{
				m_next_music_name = makePath("Music/03-TheSonofFlynn.ogg");
				break;
			}
		}
	}

	m_asking_music_fade_out = true;
}

void Game::ManageMusicTransitions(sf::Time deltaTime)
{
	if (!m_Music_Activated)
		return;

	//fade out previous music
	if (m_asking_music_fade_out)
	{
		float volume_change = 100.f / MUSIC_FADE_OUT_TIME * deltaTime.asSeconds();
		if (m_music_fader - volume_change > 0)
		{
			m_music_fader -= volume_change;
		}
		else
		{
			m_music_fader = 0;
			//load new music
			m_curMusic.openFromFile(m_next_music_name);
			m_curMusic.play();
			m_curMusic.setLoop(true);
			m_asking_music_fade_out = false;
		}
		m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * m_Music_Activated * m_music_fader / 100);
	}

	//fade in new music
	if (!m_asking_music_fade_out && m_music_fader < 100)
	{
		float volume_change = 100.f / MUSIC_FADE_IN_TIME * deltaTime.asSeconds();
		if (m_music_fader + volume_change < 100)
		{
			m_music_fader += volume_change;
		}
		else
		{
			m_music_fader = 100;
		}
		m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * m_Music_Activated * m_music_fader / 100);
	}
}

sf::RenderWindow* Game::getMainWindow()
{
	return m_window;
}

void Game::addToScene(GameObject *object, LayerType layer, ColliderType type)
{
	object->m_layer = layer;
	object->m_collider = type;
	object->m_removeMe = false;
		
	if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_ColliderType))
	{
		m_sceneGameObjectsTyped[(int)type].push_back(object);
		m_sceneGameObjectsLayered[(int)layer].push_back(object);
		m_sceneGameObjects.push_back(object);
	}
	else
	{
		throw invalid_argument(TextUtils::format("Game eror: Unable to add GameObject '%s' to layer '%d'", object->GetTextureName().c_str(), (int)layer));
	}
}

void Game::addToFeedbacks(RectangleShape* feedback)
{
	m_sceneFeedbackBars.push_back(feedback);
}

void Game::addToFeedbacks(Text* text)
{
	m_sceneFeedbackTexts.push_back(text);
}

void Game::addToFeedbacks(SFPanel* panel)
{
	m_sceneFeedbackSFPanels.push_back(panel);
}

void Game::addToFeedbacks(SFText* text)
{
	if (text)
	{
		AddSFTextToVector(text, &m_sceneFeedbackSFTexts);
	}
}

void Game::removeFromFeedbacks(RectangleShape* feedback)
{
	m_sceneFeedbackBars.remove(feedback);
}

void Game::removeFromFeedbacks(Text* text)
{
	m_sceneFeedbackTexts.remove(text);
}

void Game::removeFromFeedbacks(SFPanel* panel)
{
	m_sceneFeedbackSFPanels.remove(panel);
}

void Game::UpdateScene(Time deltaTime)
{
	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Update quad-tree classification of objects
	UpdateSectorList(false);

	//Checking colisions
	CollisionChecks();

	//Update GameObjects, interface and feedbacks
	UpdateObjects(deltaTime);

	//Update music transitions
	ManageMusicTransitions(deltaTime);

	//Update view
	m_mainScreen.setView(m_view);
}



void Game::UpdateObjects(Time deltaTime)
{
	GameObject* player = (GameObject*)m_playerShip;

	//Update objects and delete "garbage" objects
	vector<GameObject*> temp_sceneGameObjects;
	for (GameObject* object : m_sceneGameObjects)
		temp_sceneGameObjects.push_back(object);
	m_sceneGameObjects.clear();
	for (int i = 0; i < NBVAL_Layer; i++)
		m_sceneGameObjectsLayered[i].clear();
	for (int i = 0; i < NBVAL_ColliderType; i++)
		m_sceneGameObjectsTyped[i].clear();

	for (GameObject* object : temp_sceneGameObjects)
	{
		if (object == m_playerShip)
			addToScene(object, object->m_layer, object->m_collider);//update player ship at the end of the list
		else if (object->m_garbageMe == true)
			delete object;
		else if (object->m_removeMe == false)//if true, we trust it has already been stored in m_sceneGameObjectsStored, therefore there is no memory leak if we don't push it back
		{
			object->Update(deltaTime);

			if (object != player && object != m_background)
				object->setPosition(sf::Vector2f(object->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, - (object->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
				
			addToScene(object, object->m_layer, object->m_collider);
		}
	}

	m_playerShip->Update(deltaTime);

	//SFTextPop (text feedbacks)
	size_t sceneTextPopFeedbacksSize = m_sceneFeedbackSFTexts.size();
	for (size_t i = 0; i < sceneTextPopFeedbacksSize; i++)
	{
		if (m_sceneFeedbackSFTexts[i] == NULL)
			continue;

		m_sceneFeedbackSFTexts[i]->Update(deltaTime);
	}
}

void Game::drawScene()
{
	m_mainScreen.clear();

	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == SectorLayer)
		{
			//DEBUG
			GameObject* player = (GameObject*)m_playerShip;
			
			for (StarSector sector : m_star_sectors_managed)
			{
				GameObject* sector_debug;
				if (sector.m_index == m_playerShip->m_star_sector_index)
					sector_debug = m_sector_debug_current;
				else
					sector_debug = m_sector_debug_onscreen;

				SetStarSectorIndex(sector_debug, sector.m_index);
				sector_debug->setPosition(sf::Vector2f(sector_debug->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(sector_debug->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
			}
		}
		else
		{
			for (GameObject* object : m_sceneGameObjectsLayered[i])
			{
				object->Draw(m_mainScreen);
			}
		}
	}

	m_mainScreen.display();
	sf::Sprite temp(m_mainScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	m_window->draw(temp);
}

void Game::CollisionChecks()
{
	sf::Clock dt;
	dt.restart();

	for (GameObject* player_ship : m_sceneGameObjectsTyped[PlayerShipObject])
		for (GameObject* enemy_ammo : m_sceneGameObjectsTyped[EnemyFire])
			if (SimpleCollision::AreColliding(player_ship, enemy_ammo) == true)
				player_ship->GetHitByAmmo(enemy_ammo);

	for (GameObject* player_ammo : m_sceneGameObjectsTyped[PlayerFire])
		for (GameObject* enemy_ship : m_sceneGameObjectsTyped[EnemyShipObject])
			if (SimpleCollision::AreColliding(player_ammo, enemy_ship) == true)
				enemy_ship->GetHitByAmmo(player_ammo);

}

void Game::AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector)
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

GameObject* Game::GetClosestObjectTyped(const GameObject* ref_obj, ColliderType type_of_closest_object, float dist_max, float angle_delta_max)
{
	const sf::Vector2f ref_position = ref_obj->getPosition();

	return GetClosestObjectTyped(ref_position, type_of_closest_object, dist_max, angle_delta_max);
}

GameObject* Game::GetClosestObjectTyped(const sf::Vector2f position, ColliderType type_of_closest_object, float dist_max, float angle_delta_max)
{
	float shortest_distance = -1;
	GameObject* returned_obj = NULL;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type_of_closest_object].begin(); it != m_sceneGameObjectsTyped[type_of_closest_object].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_visible == true)
		{
			const float a = position.x - (*it)->getPosition().x;
			const float b = position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				if (dist_max < 0 || distance_to_ref < dist_max)
				{
					float angle_delta = GetAngleDegToTargetPosition((*it)->getPosition(), (*it)->getRotation(), position);
					if (angle_delta < 0)
						angle_delta = -angle_delta;

					if (angle_delta_max < 0 || angle_delta < angle_delta_max)
					{
						shortest_distance = distance_to_ref;
						returned_obj = (*it);
					}
				}

				shortest_distance = distance_to_ref;
				returned_obj = (*it);
			}
		}
	}

	return returned_obj;
}

std::vector<GameObject*> Game::GetSceneGameObjectsTyped(ColliderType type)
{
	return m_sceneGameObjectsTyped[type];
}

void Game::CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY)
{
	SFText* text_feedback = new SFText(m_font[font], 16, color, sf::Vector2f(position.x, position.y), team);
	SFTextPop* pop_feedback = new SFTextPop(text_feedback, distance_not_faded, distance_faded, total_pop_time, follow_target, offset_positionY);
	pop_feedback->setString(text);
	pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
	delete text_feedback;
	addToFeedbacks(pop_feedback);
}

bool Game::AddToStarSectorsKnown(sf::Vector2i star_sector_index)
{
	for (StarSector sector : m_star_sectors_known)
	{
		if (sector.m_index == star_sector_index)
		{
			return false;
		}
	}

	m_star_sectors_known.push_back(StarSector(star_sector_index, m_star_sectors_known.size()));

	return true;
}

void Game::UpdateSectorList(bool force_update)
{
	GameObject* player = (GameObject*)m_playerShip;

	//update needed?
	if (force_update == true || m_previous_star_sector_index != player->m_star_sector_index)
	{
		m_previous_star_sector_index = player->m_star_sector_index;

		vector<sf::Vector2i> old_star_sectors_managed;
		for (sf::Vector2i index : m_star_sectors_managed)
			old_star_sectors_managed.push_back(index);
		m_star_sectors_managed.clear();
		
		int nb_sectors_x = (REF_WINDOW_RESOLUTION_X / STAR_SECTOR_SIZE) + 3;
		int nb_sectors_y = (REF_WINDOW_RESOLUTION_Y / STAR_SECTOR_SIZE) + 3;
		for (int i = 0; i < nb_sectors_x; i++)
		{
			for (int j = 0; j < nb_sectors_y; j++)
			{
				sf::Vector2i index;
				index.x = i + m_playerShip->m_star_sector_index.x - (nb_sectors_x / 2);
				index.y = j + m_playerShip->m_star_sector_index.y - (nb_sectors_y / 2);
				//StarSector sector(sf::Vector2i(i + m_playerShip->m_star_sector_index.x - (nb_sectors_x / 2), j + m_playerShip->m_star_sector_index.y - (nb_sectors_y / 2)), (i == nb_sectors_x / 2 && j == nb_sectors_y / 2) ? Sector_Current : Sector_OnScreen);
				m_star_sectors_managed.push_back(index);
				
				if (GetSectorId(index) == -1)
				{
					m_star_sectors_to_create.push_back(index);//new sector discovered needs to be created
					m_star_sectors_known.push_back(StarSector(index, m_star_sectors_known.size()));
					old_star_sectors_managed.push_back(index);//this will prevent from reaching out for this sector in stored content, since it's a new content that will not be found in storage
				}
			}
		}

		//delta between old and new manageable sectors
		vector<sf::Vector2i> sector_index_to_add;
		vector<sf::Vector2i> sector_index_to_remove;
		for (sf::Vector2i old_index : old_star_sectors_managed)
		{
			bool found = false;
			for (sf::Vector2i new_index : m_star_sectors_managed)
			{
				if (old_index == new_index)
				{
					found = true;
					break;
				}
			}

			if (found == false)
			{
				sector_index_to_remove.push_back(old_index);
			}
		}

		for (sf::Vector2i new_index : m_star_sectors_managed)
		{
			bool found = false;
			for (sf::Vector2i old_index : old_star_sectors_managed)
			{
				if (old_index == new_index)
				{
					found = true;
					break;
				}
			}

			if (found == false)
			{
				sector_index_to_add.push_back(new_index);
			}
		}

		//Sectors to remove from scene
		for (sf::Vector2i index : sector_index_to_remove)
		{
			vector<GameObject*> vector_objects;
			for (GameObject* object : m_sceneGameObjects)
			{
				if (object == m_background)
					continue;

				if (object->m_star_sector_index == index && object->m_garbageMe == false)
				{
					if (object->m_collider != EnemyFire && object->m_collider != PlayerFire)//temporary objects such as flying ammunition don't need to be stored, they can be deleted in the process
					{
						object->m_removeMe = true;
						vector_objects.push_back(object);
						//printf("game object removed and stored.\n");
					}
					else
						object->m_garbageMe = true;
				}
			}

			if (vector_objects.empty() == false)
			{
				int id = GetSectorId(index);
				m_sceneGameObjectsStored.insert(pair<int, vector<GameObject*> > (id, vector_objects));
			}
		}
		
		//Sectors to insert insert into scene
		for (sf::Vector2i index : sector_index_to_add)
		{
			int id = GetSectorId(index);

			for (GameObject* object : m_sceneGameObjectsStored[id])
			{
				addToScene(object, object->m_layer, object->m_collider);
				//printf("game object restored.\n");
			}
				
			m_sceneGameObjectsStored.erase(id);
		}	
	}
}

int Game::GetSectorId(sf::Vector2i index)
{
	for (StarSector sector : m_star_sectors_known)
		if (sector.m_index == index)
			return sector.m_id;

	return -1;
}

void Game::SetStarSectorIndex(GameObject* object, sf::Vector2i sector_index)
{
	object->m_star_sector_index = sector_index;

	object->m_position.x = 1.f * sector_index.x * STAR_SECTOR_SIZE;
	object->m_position.y = 1.f * sector_index.y * STAR_SECTOR_SIZE;

	if (object != m_playerShip)
	{
		//need a manual storage? (if the sector is beyond "managed sectors" i.e. ~ off-screen)
		for (sf::Vector2i index : m_star_sectors_managed)
			if (sector_index == index)
				return;

		//sector unknown? give it an id first
		int id = GetSectorId(sector_index);
		if (id == -1)
		{
			id = m_star_sectors_known.size();
			m_star_sectors_known.push_back(StarSector(sector_index, id));
		}
		
		//store it
		m_sceneGameObjectsStored[id].push_back(object);
		object->m_removeMe = true;	
		//printf("manuel storage\n");
	}
}