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

	m_zoom = 1.3;

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
	addToScene(m_background, BackgroundLayer, BackgroundObject, false);

	SetSectorsNbSectorsManaged();

	//DEBUG
	m_sector_debug_current = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Blue, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);
	m_sector_debug_onscreen = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), sf::Color::Green, sf::Vector2f(STAR_SECTOR_SIZE, STAR_SECTOR_SIZE), 3);

	//Network
	m_is_server = false;
	m_network_status = Network_Disconnected;
	//m_ip = "93.2.208.150";
	//m_port = 53000;

	m_socket.setBlocking(false);
}

void Game::SetSectorsNbSectorsManaged()
{
	m_nb_sectors_managed_x = (REF_WINDOW_RESOLUTION_X / STAR_SECTOR_SIZE) + 4;
	m_nb_sectors_managed_x *= m_zoom;
	if (m_nb_sectors_managed_x % 2 == 0)
		m_nb_sectors_managed_x++;

	m_nb_sectors_managed_y = (REF_WINDOW_RESOLUTION_Y / STAR_SECTOR_SIZE) + 4;
	m_nb_sectors_managed_y *= m_zoom;
	if (m_nb_sectors_managed_y % 2 == 0)
		m_nb_sectors_managed_y++;
}

Game::~Game()
{
	delete m_background;

	delete m_sector_debug_current;
	delete m_sector_debug_onscreen;

	for (AsteroidField* field : m_asteroidFields)
		delete field;

	delete m_playerShip;

	for (GameObject* ship : m_onlineShips)
		delete ship;
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

void Game::addToScene(GameObject *object, LayerType layer, ColliderType type, bool created_by_updated_object)
{
	object->m_layer = layer;
	object->m_collider = type;
	object->m_removeMe = false;

	if (created_by_updated_object == true)
	{
		m_temp_sceneGameObjects.push_back(object);
	}
	else if (((int)layer >= 0 && (int)layer < NBVAL_Layer) && (type >= 0 && type < NBVAL_ColliderType))
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

/*NetworkStatus Game::UpdateNetworkServerStatus()
{
	if (m_network_status != Network_Connected)
	{
		//lie l'�couteur � un port
		if (m_listener.listen(m_port) != sf::Socket::Done)
		{
			//erreur...
		}

		// accepte une nouvelle connexion
		if (m_listener.accept(m_receiver) != sf::Socket::Done)
			m_network_status = Network_Disconnected;
		else
			m_network_status = Network_Connected;
	}

	return m_network_status;
}
*/


NetworkStatus Game::UpdateNetworkClientStatus()
{
	// bind the socket to a port
	if (m_socket.bind(m_port_receive) != sf::Socket::Done)
	{
		m_network_status = Network_Disconnected;
	}
	else
	{
		m_network_status = Network_Connected;
		m_port_receive = m_socket.getLocalPort();
	}

	return m_network_status;
}

void Game::UpdateScene(Time deltaTime)
{
	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//zoom / zoom back
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
	{
		m_zoom += 0.1;
		printf("zoom: %f\n", m_zoom);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
	{
		m_zoom -= 0.1;
		printf("zoom: %f\n", m_zoom);
	}

	//Network
	//Connexion
	//if (m_is_server == true)
	//	UpdateNetworkServer();
	//else
	UpdateNetworkClient();

	//we need an odd number of sectors on X and Y axis
	SetSectorsNbSectorsManaged();

	Bound(m_zoom, 1, 10);
	
	sf::Vector2i mousepos2i = sf::Mouse::getPosition(*getMainWindow());
	m_mouse_pos = getMainWindow()->mapPixelToCoords(mousepos2i, m_view);

	//Update quad-tree classification of objects
	UpdateSectorList(false);

	//Empty garbage objects
	ClearGarbage();

	//Update GameObjects, interface and feedbacks
	UpdateObjects(deltaTime);

	//Checking colisions
	CollisionChecks();

	//Update music transitions
	ManageMusicTransitions(deltaTime);

	//Update view
	m_mainScreen.setView(m_view);
}

void Game::UpdateObjects(Time deltaTime)
{
	GameObject* player = (GameObject*)m_playerShip;

	//Update objects and delete "garbage" objects
	m_playerShip->Update(deltaTime);
	for (GameObject* object : m_sceneGameObjects)
	{
		if (object->m_garbageMe == true)
			m_garbageObjects.push_back(object);
		else if (object->m_removeMe == false)//if true, we trust it has already been stored in m_sceneGameObjectsStored, therefore there is no memory leak if we don't push it back
		{
			if (object != m_playerShip)//was updated before-hand
				object->Update(deltaTime);
			m_temp_sceneGameObjects.push_back(object);
		}
	}

	m_sceneGameObjects.clear();//the objects that are not copied in temp are not lost in memory leak, they have been stored during UpdateSectorList and flagged with m_removeMe = true
	for (int i = 0; i < NBVAL_Layer; i++)
		m_sceneGameObjectsLayered[i].clear();
	for (int i = 0; i < NBVAL_ColliderType; i++)
		m_sceneGameObjectsTyped[i].clear();

	for (GameObject* object : m_temp_sceneGameObjects)
	{
		addToScene(object, object->m_layer, object->m_collider, false);

		if (object != player && object != m_background)//set position of objects on screen relative to the player
			object->SetPosition(sf::Vector2f(object->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
	}
	m_temp_sceneGameObjects.clear();

	player->UpdateMarkers(deltaTime);

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
			//DebugDrawSectors();
		}
		else if (i == GravitationLayer)
		{
			for (CircleDisplay* circle : m_gravity_circles)
			{
				GameObject* object = circle->m_target_object;

				object->setScale(1.f / m_zoom, 1.f / m_zoom);
				object->SetPosition(sf::Vector2f((object->m_position.x - m_playerShip->m_position.x) / m_zoom + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - m_playerShip->m_position.y) / m_zoom + REF_WINDOW_RESOLUTION_Y * 0.5));

				circle->setScale(1.f / m_zoom, 1.f / m_zoom);
				circle->setPosition(object->getPosition());

				m_mainScreen.draw(*circle);

				object->setScale(1, 1);
				object->SetPosition(sf::Vector2f(object->m_position.x - m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));

				circle->setScale(1, 1);
			}
		}
		else if (i == ShieldLayer)
		{
			for (CircleDisplay* circle : m_shield_circles)
			{
				GameObject* object = circle->m_target_object;

				object->setScale(1.f / m_zoom, 1.f / m_zoom);
				object->SetPosition(sf::Vector2f((object->m_position.x - m_playerShip->m_position.x) / m_zoom + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - m_playerShip->m_position.y) / m_zoom + REF_WINDOW_RESOLUTION_Y * 0.5));

				circle->setScale(1.f / m_zoom, 1.f / m_zoom);
				circle->setPosition(object->getPosition());

				m_mainScreen.draw(*circle);

				object->setScale(1, 1);
				object->SetPosition(sf::Vector2f(object->m_position.x - m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));

				circle->setScale(1, 1);
			}
		}
		else
		{
			for (GameObject* object : m_sceneGameObjectsLayered[i])
			{
				object->setScale(1.f / m_zoom, 1.f / m_zoom);
				if (object != m_playerShip && object != m_background)//set position of objects on screen relative to the player
					object->SetPosition(sf::Vector2f((object->m_position.x - m_playerShip->m_position.x) / m_zoom + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - m_playerShip->m_position.y) / m_zoom + REF_WINDOW_RESOLUTION_Y * 0.5));

				object->Draw(m_mainScreen);

				object->setScale(1, 1);
				if (object != m_playerShip && object != m_background)//set position of objects on screen relative to the player
					object->SetPosition(sf::Vector2f(object->m_position.x - m_playerShip->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(object->m_position.y - m_playerShip->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));

			}
		}
	}

	//Reset circles to be drawn
	m_gravity_circles.clear();
	m_shield_circles.clear();

	//DEBUG
	DebugDrawGameObjectsStats();

	m_mainScreen.display();
	sf::Sprite temp(m_mainScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	m_window->clear();
	m_window->draw(temp);
}

void Game::CollisionChecks()
{
	sf::Clock dt;
	dt.restart();

	for (GameObject* ally_ship : m_sceneGameObjectsTyped[AllyShipObject])
	{
		//Enemy shots
		for (GameObject* enemy_ammo : m_sceneGameObjectsTyped[EnemyFire])
			if (AreColliding(ally_ship, enemy_ammo, true) == true)
				ally_ship->GetHitByAmmo(enemy_ammo);

		//Beacons
		for (GameObject* beacon : m_sceneGameObjectsTyped[BeaconObject])
			if (GetDistanceSquaredBetweenPositions(ally_ship->m_position, beacon->m_position) <= 200 * 200)
				beacon->TryTrigger(ally_ship);
					
		//Collision with enemies
		for (GameObject* enemy_ship : m_sceneGameObjectsTyped[EnemyShipObject])
		{
			if (AreColliding(ally_ship, enemy_ship, true) == true)
			{
				ally_ship->GetHitByObject(enemy_ship);
				enemy_ship->GetHitByObject(ally_ship);
			}

			//Enemies hit by our gravitational range
			if (ally_ship->GetGravitationRange() > 0)
				if (GetDistanceSquaredBetweenPositions(ally_ship->m_position, enemy_ship->m_position) <= ally_ship->GetGravitationRange() * ally_ship->GetGravitationRange())
					ally_ship->HitWithGravitation(enemy_ship);
		}
		
		if (ally_ship != m_playerShip)
		{
			//Ally AI grouping
			ally_ship->UpdateAlliedShips();

			//Collateral damage by player shots on allies
			for (GameObject* ally_ammo : m_sceneGameObjectsTyped[AllyFire])
				if (ally_ammo->IsAmmoOwnedByPlayer() == true)
					if (AreColliding(ally_ship, ally_ammo, true) == true)
						ally_ship->GetHitByAmmo(ally_ammo);
		}
		else//Player
		{
			//Getting loots
			for (GameObject* loot : m_sceneGameObjectsTyped[LootObject])
			{
				if (AreColliding(ally_ship, loot, false) == true)
					ally_ship->GetHitByLoot(loot);
				//Loots hit by our gravitational range
				else if (ally_ship->GetGravitationRange() > 0)
					if (GetDistanceSquaredBetweenPositions(ally_ship->m_position, loot->m_position) <= ally_ship->GetGravitationRange() * ally_ship->GetGravitationRange())
						ally_ship->HitWithGravitation(loot);
			}

			//Collision with objects
			for (GameObject* object : m_sceneGameObjectsTyped[DestructibleObject])
			{
				if (AreColliding(ally_ship, object, true) == true)
				{
					object->GetHitByObject(ally_ship);
					ally_ship->GetHitByObject(object);
				}

				//Objects hit by our gravitational range
				if (ally_ship->GetGravitationRange() > 0)
					if (GetDistanceSquaredBetweenPositions(ally_ship->m_position, object->m_position) <= ally_ship->GetGravitationRange() * ally_ship->GetGravitationRange())
						ally_ship->HitWithGravitation(object);
			}
		}
	}

	for (GameObject* enemy_ship : m_sceneGameObjectsTyped[EnemyShipObject])
	{
		//Player & allies shots
		for (GameObject* ally_ammo : m_sceneGameObjectsTyped[AllyFire])
			if (AreColliding(ally_ammo, enemy_ship, true) == true)
				enemy_ship->GetHitByAmmo(ally_ammo);

		//Hit by enemy gravitational range
		if (enemy_ship->GetGravitationRange() > 0)
			for (GameObject* ally_ship : m_sceneGameObjectsTyped[AllyShipObject])
				if (GetDistanceSquaredBetweenPositions(enemy_ship->m_position, ally_ship->m_position) <= enemy_ship->GetGravitationRange() * enemy_ship->GetGravitationRange())
					enemy_ship->HitWithGravitation(ally_ship);

		//Enemies hit by objects
		for (GameObject* object : m_sceneGameObjectsTyped[DestructibleObject])
			if (AreColliding(enemy_ship, object, true) == true)
			{
				object->GetHitByObject(enemy_ship);
				enemy_ship->GetHitByObject(object);
			}

		//Enemy AI grouping
		enemy_ship->UpdateAlliedShips();
	}

	//Shots versus objects
	for (GameObject* object : m_sceneGameObjectsTyped[DestructibleObject])
	{
		for (GameObject* ally_ammo : m_sceneGameObjectsTyped[AllyFire])
			if (AreColliding(object, ally_ammo, true) == true)
				object->GetHitByAmmo(ally_ammo);

		for (GameObject* enemy_ammo : m_sceneGameObjectsTyped[EnemyFire])
			if (AreColliding(object, enemy_ammo, true) == true)
				object->GetHitByAmmo(enemy_ammo);
	}
}

bool Game::AreColliding(GameObject* objectA, GameObject* objectB, bool include_shield)
{
	if (objectA->m_visible == false || objectB->m_visible == false)
		return false;

	//discus check: on regarde si la distance entre les centres des 2 sprites est plus grande que leurs rayons additionn�s
	const float a = objectA->getPosition().x - objectB->getPosition().x;
	const float b = objectA->getPosition().y - objectB->getPosition().y;
	const float c = objectA->GetRadius(include_shield) + objectB->GetRadius(include_shield);

	if (a*a + b*b > c*c)
		return false;

	if (include_shield == true)
		return true;

	if (PIXEL_PERFECT_COLLISION)
	{
		return Collision::PixelPerfectTest(objectA, objectB, 127);
	}
	else
	{
		// No pixel perfect : are the corners included in the other sprite, or vice versa ?
		sf::IntRect boundsA(FToIRect(objectA->getGlobalBounds()));
		sf::IntRect boundsB(FToIRect(objectB->getGlobalBounds()));
		return boundsA.intersects(boundsB);
	}
}

sf::IntRect Game::FToIRect(const sf::FloatRect& f)
{
	return sf::IntRect((int)f.left, (int)f.top, (int)f.width, (int)f.height);
}

void Game::AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector)
{
	const size_t vectorSize = vector->size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if ((*vector)[i] == NULL)
		{
			(*vector)[i] = pSFText;
			return; // ay�, on a trouv� un free slot, ins�r�, maintenant on a fini
		}
	}

	// On n'arrive ici que dans le cas o� on n'a pas trouv� de free slot => on rajoute � la fin
	vector->push_back(pSFText);
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
	for (StarSector sector : m_sectorsKnown)
	{
		if (sector.m_index == star_sector_index)
		{
			return false;
		}
	}

	m_sectorsKnown.push_back(StarSector(star_sector_index, m_sectorsKnown.size()));

	return true;
}

void Game::UpdateSectorList(bool force_update)
{
	GameObject* player = (GameObject*)m_playerShip;

	//update needed?
	if (force_update == true || m_previous_star_sector_index != player->m_sector_index)
	{
		vector<sf::Vector2i> tmp_starSectorsManaged;
		sf::Vector2i index;

		//search for nearby sectors that were far from previous player sector => add them to the list of managed sectors
		for (int j = 0; j < m_nb_sectors_managed_y; j++)
		{
			index.y = j + m_playerShip->m_sector_index.y - (m_nb_sectors_managed_y / 2);
			if (force_update == true || abs(index.y - m_previous_star_sector_index.y) > m_nb_sectors_managed_y / 2)
				//add the whole line
				for (int i = 0; i < m_nb_sectors_managed_x; i++)
				{
					index.x = i + m_playerShip->m_sector_index.x - (m_nb_sectors_managed_x / 2);
					tmp_starSectorsManaged.push_back(index);
				}
			else
				for (int i = 0; i < m_nb_sectors_managed_x; i += force_update == true ? 1 : m_nb_sectors_managed_x - 1)
				{
					index.x = i + m_playerShip->m_sector_index.x - (m_nb_sectors_managed_x / 2);
					//add the left or right row (or the whole line if we are forcing a complete update)
					if (force_update == true || abs(index.x - m_previous_star_sector_index.x) > m_nb_sectors_managed_x / 2)
						tmp_starSectorsManaged.push_back(index);
				}
		}

		//update player sector flag
		m_previous_star_sector_index = m_playerShip->m_sector_index;
		
		//manage freshly added sectors
		for (sf::Vector2i index : tmp_starSectorsManaged)
		{
			//add a star in the background during next Gameloop update
			m_sectorsToAddStar.push_back(index);

			//sector is unknown? => add it to the list of known sectors and populate it
			int id = GetSectorId(index);
			if (id == -1)
			{
				m_sectorsToCreate.push_back(index);
				m_sectorsKnown.push_back(StarSector(index, m_sectorsKnown.size()));
			}

			//bring back objects stored in this sector back into game scene
			for (GameObject* object : m_sceneGameObjectsStored[id])
			{
				addToScene(object, object->m_layer, object->m_collider, false);
				//printf("game object restored.\n");
			}

			m_sceneGameObjectsStored.erase(id);
		}

		//search for sectors that were managed until now but have become now too far from current player sector => remove them from the list of managed sectors
		for (sf::Vector2i index : m_sectorsManaged)
		{
			//close enough to keep being managed
			if (abs(index.x - player->m_sector_index.x) <= m_nb_sectors_managed_x / 2 && abs(index.y - player->m_sector_index.y) <= m_nb_sectors_managed_y / 2)
				tmp_starSectorsManaged.push_back(index);
			//too far => sector objects have to be stored
			else
			{
				int id = GetSectorId(index);
				for (GameObject* object : m_sceneGameObjects)
				{
					if (object == m_background || object == m_playerShip)
						continue;

					if (object->m_sector_index == index && object->m_garbageMe == false)
					{
						//temporary objects such as flying ammunition don't need to be stored, they can be garbaged in the process
						if (object->m_layer != StarLayer && object->m_collider != EnemyFire && object->m_collider != AllyFire)
						{
							//marked objects are never stored, they keep getting updated at any distance
							if (object->IsMarked() == false)
							{
								//storage with the dedicated sector id
								object->m_removeMe = true;
								m_sceneGameObjectsStored[id].push_back(object);
							}
						}
						else
							object->m_garbageMe = true;
					}
				}
			}
		}

		//refresh the list of managed sectors
		m_sectorsManaged.clear();
		for (sf::Vector2i index : tmp_starSectorsManaged)
			m_sectorsManaged.push_back(index);

		//debug assert
		if (m_sectorsManaged.size() != m_nb_sectors_managed_x * m_nb_sectors_managed_y)
			printf("\n<!> BUG : managed sectors' count is wrong (UpdateSectorList). Expected: %d. Actual: %d)\n\n", m_nb_sectors_managed_x * m_nb_sectors_managed_y, m_sectorsManaged.size());
	}
}

int Game::GetSectorId(sf::Vector2i index)
{
	for (StarSector sector : m_sectorsKnown)
		if (sector.m_index == index)
			return sector.m_id;

	return -1;
}

void Game::SetStarSectorIndex(GameObject* object, sf::Vector2i sector_index)
{
	object->m_sector_index = sector_index;

	object->m_position.x = 1.f * sector_index.x * STAR_SECTOR_SIZE;
	object->m_position.y = 1.f * sector_index.y * STAR_SECTOR_SIZE;
}

void Game::DebugDrawSectors()
{
	GameObject* player = (GameObject*)m_playerShip;

	for (StarSector sector : m_sectorsManaged)
	{
		GameObject* sector_debug;
		if (sector.m_index == m_playerShip->m_sector_index)
			sector_debug = m_sector_debug_current;
		else
			sector_debug = m_sector_debug_onscreen;

		SetStarSectorIndex(sector_debug, sector.m_index);
		sector_debug->setPosition(sf::Vector2f(sector_debug->m_position.x - player->m_position.x + REF_WINDOW_RESOLUTION_X * 0.5, -(sector_debug->m_position.y - player->m_position.y) + REF_WINDOW_RESOLUTION_Y * 0.5));
		sector_debug->Draw(m_mainScreen);
	}
}

void Game::DebugDrawGameObjectsStats()
{
	int a = 0;
	for (GameObject* object : m_sceneGameObjects)
		if (object->m_layer != StarLayer && object != m_playerShip && object->m_collider != BackgroundObject && object->m_collider != AllyFire && object->m_collider != EnemyFire)
			a++;

	int b = m_playerShip->GetMarkedObjectsCount();

	int c = 0;
	for (map<int, vector<GameObject*> >::iterator it = m_sceneGameObjectsStored.begin(); it != m_sceneGameObjectsStored.end(); it++)
		for (GameObject* object : it->second)
			if (object->m_layer != StarLayer && object != m_playerShip && object != m_background)
				c++;

	for (int i = 0; i < 5; i++)
	{
		sf::Text text;
		text.setFont(*m_font[Font_Arial]);
		text.setCharacterSize(20);
		text.setColor(sf::Color::White);

		text.setPosition(sf::Vector2f(50, 50 * (1 + i)));

		if (i == 0)
			text.setString("Spatial objects updated: " + to_string(a));

		if (i == 1)
			text.setString("Marked objects: " + to_string(b));

		if (i == 2)
			text.setString("Stored objects: " + to_string(c));

		if (i == 3)
			text.setString("Zoom level: " + to_string(m_zoom));

		if (i == 4)
			if (m_is_server == true)
				//text.setString("Server | Port: " + to_string(m_port));
				text.setString("Server | Port receive: " + to_string(m_port_receive) + " | IP send: " + m_ip.toString() + " | Port send : " + to_string(m_port_send));
			else
				//text.setString("Client | IP: " + m_ip.toString() + " | Port: " + to_string(m_port));
				text.setString("Client | Port receive: " + to_string(m_port_receive) + " | IP send: " + m_ip.toString() + " | Port send : " + to_string(m_port_send));

		m_mainScreen.draw(text);
	}
}

bool Game::StoreObjectIfNecessary(GameObject* object)
{
	//need a manual storage? (if the sector is beyond "managed sectors" i.e. ~ off-screen)
	for (sf::Vector2i index : m_sectorsManaged)
		if (object->m_sector_index == index)
			return false;

	//sector unknown? give it an id first
	int id = GetSectorId(object->m_sector_index);
	if (id == -1)
	{
		id = m_sectorsKnown.size();
		m_sectorsKnown.push_back(StarSector(object->m_sector_index, id));
	}

	//store it
	m_sceneGameObjectsStored[id].push_back(object);
	object->m_removeMe = true;
	//printf("manuel storage\n");

	return true;
}

void Game::ClearGarbage()
{
	for (GameObject* object : m_garbageObjects)
		delete object;

	m_garbageObjects.clear();
}

SpatialObject* Game::GetTargetableEnemyShip(GameObject* const shooter, float dist_max, float angle_delta_max)
{
	float shortest_distance = -1;
	SpatialObject* target = NULL;

	ColliderType target_collider = (shooter->m_collider == AllyShipObject || shooter->m_collider == AllyFire) ? EnemyShipObject : AllyShipObject;

	for (GameObject* object : m_sceneGameObjectsTyped[target_collider])
	{
		const float a = shooter->m_position.x - object->m_position.x;
		const float b = shooter->m_position.y - object->m_position.y;

		float distance_to_ref = (a * a) + (b * b);
		if (distance_to_ref < shortest_distance || shortest_distance < 0)
		{
			if (distance_to_ref <= dist_max * dist_max)
			{
				float angle_delta = GetAngleDegToTargetPosition(shooter->m_position, shooter->m_heading, object->m_position);

				if (abs(angle_delta) <= angle_delta_max)
				{
					shortest_distance = distance_to_ref;
					target = (SpatialObject*)object;
				}
			}
		}
	}

	return target;
}

void Game::SendNetworkPacket()
{
	//network, send packets
	NetworkPacketShip packet;
	UpdateNetworkPacketShip(packet);

	if (m_socket.send(packet.m_buffer, sizeof(packet.m_buffer), m_ip, m_port_send) != sf::Socket::Done)
	{
		// erreur...
	}
	else
	{
		
	}
}

void Game::ReceiveNetworkPacket()
{
	//network, receive packets
	std::size_t received;

	NetworkPacketShip packet;

	//Status receive(void* data, std::size_t size, std::size_t& received, IpAddress& remoteAddress, unsigned short& remotePort);
	sf::IpAddress sender;
	unsigned short port;

	if (m_socket.receive(packet.m_buffer, sizeof(packet.m_buffer), received, sender, port) != sf::Socket::Done)
	{
		// erreur...
	}
	else
	{
		m_onlineShips.front()->m_position = sf::Vector2f(packet.m_data.m_position_x, packet.m_data.m_position_y);
		m_onlineShips.front()->m_heading = packet.m_data.m_heading;
	}
}

/*
void Game::UpdateNetworkServer()
{
	if (UpdateNetworkServerStatus() == Network_Connected)
	{
		SendNetworkPacket();
		ReceiveNetworkPacket();
	}
}
*/

void Game::UpdateNetworkClient()
{
	if (m_network_status != Network_Connected)
		UpdateNetworkClientStatus();

	if (m_network_status == Network_Connected)
	{
		SendNetworkPacket();
		ReceiveNetworkPacket();
	}
}

void Game::UpdateNetworkPacketShip(NetworkPacketShip& packet)
{
	packet.m_data.m_position_x = m_playerShip->m_position.x;
	packet.m_data.m_position_y = m_playerShip->m_position.y;
	packet.m_data.m_heading = m_playerShip->m_heading;
}