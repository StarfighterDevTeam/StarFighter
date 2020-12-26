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
	m_window = window;
	m_mainScreen.create(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y, false);
	m_mainScreen.setSmooth(true);

	m_sceneChronometer.restart();
	m_scale_factor.x = 1.0f * WINDOW_RESOLUTION_X / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * WINDOW_RESOLUTION_Y / REF_WINDOW_RESOLUTION_Y;
	m_screen_size = sf::Vector2i(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y);

	resetHazard();;//initalisation of the scoring system
	m_BeastScoreBonus = 0;
	m_hyperspeedMultiplier = 1.0f;
	m_vspeed = 0;

	m_playership = NULL;
	m_background = NULL;
	m_waiting_for_dialog_validation = false;
	m_waiting_for_scene_transition = false;
	m_Pause = false;

	//Sounds
	m_SFX_Activated = true;
	LoadSFX();

	//Music
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading Musics");
	m_Music_Activated = false;
	m_music_fader = 0;
	m_asking_music_fade_out = false;
	//if (!SpaceCowboys.openFromFile("Music/SpaceCowboys.ogg"))
	//if (!SpaceCowboys.openFromFile("Music/CrimeaDigital.ogg"))
	//if (!SpaceCowboys.openFromFile("Music/Rebecca.ogg"))
	//if (!SpaceCowboys.openFromFile("Music/Daft Punk - Derezzed.ogg"))

	//Fonts
	try
	{
		m_font[Font_Arial] = new sf::Font();
		if (!m_font[Font_Arial]->loadFromFile(makePath("Fonts/arial.ttf")))
		{
			
		}
		m_font[Font_Terminator] = new sf::Font();
		if (!m_font[Font_Terminator]->loadFromFile(makePath("Fonts/terminator_custom_nfi.ttf")))
		{
			if (!m_font[Font_Terminator]->loadFromFile(makePath("Fonts/terminator_real_nfi.ttf")))
			{

			}
		}
	}
	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::LERROR, ex.what());
	}
}

void Game::SetSFXVolume(bool activate_sfx)
{
	typedef std::map<SFX_Bank, sf::Sound>::iterator it_type;
	for (it_type iterator = m_sounds.begin(); iterator != m_sounds.end(); iterator++)
	{
		iterator->second.setVolume(DEFAULT_SFX_VOLUME * activate_sfx);
	}
}

void Game::SetMusicVolume(bool activate_music)
{
	m_curMusic.setVolume(DEFAULT_MUSIC_VOLUME * activate_music);
	m_Music_Activated = activate_music;
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
		m_next_music_name = makePath(specific_filename);
	}
	//default musics
	else
	{
		//choose the right music file
		switch (music)
		{
			case Music_Hub:
			{
				//m_next_music_name = makePath("Music/Hub.ogg");
				m_next_music_name = makePath("Music/ShadowWarrior/MasterNinjaTheme.ogg");
				break;
			}
			case Music_Scene:
			{
				//m_next_music_name = makePath("Music/Scene.ogg");
				m_next_music_name = makePath("Music/ShadowWarrior/Ruins.ogg");
				break;
			}
			case Music_Boss:
			{
				//m_next_music_name = makePath("Music/Boss.ogg");
				m_next_music_name = makePath("Music/ShadowWarrior/Pursuit.ogg");
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

void Game::SetPlayerShip(Ship* playership)
{
	m_playership = playership;
}

int Game::LoadSFX()
{
	if (!m_soundBuffers[SFX_Fire].loadFromFile(makePath("Sounds/laser_repeat.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_Kill].loadFromFile(makePath("Sounds/kill.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_BigKill].loadFromFile(makePath("Sounds/big_kill.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_Teleport].loadFromFile(makePath("Sounds/teleport.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_BuyOrSell].loadFromFile(makePath("Sounds/trade.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_MoneyLoot].loadFromFile(makePath("Sounds/money_loot.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_Equip].loadFromFile(makePath("Sounds/equip.ogg")))
		return -1;
	//if (!m_soundBuffers[SFX_Heal].loadFromFile(makePath("Sounds/heal.ogg")))
	//	return -1;
	if (!m_soundBuffers[SFX_PortalOpening].loadFromFile(makePath("Sounds/gate_opening.ogg")))
		return -1;
	if (!m_soundBuffers[SFX_EnteringPortal].loadFromFile(makePath("Sounds/entering_portal.ogg")))
		return -1;

	for (size_t i = 0; i < NBVAL_SFX_BANK; i++)
	{
		sf::Sound* new_sound = new sf::Sound;
		new_sound->setBuffer(m_soundBuffers[i]);
		new_sound->setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);
		m_sounds.insert(map<SFX_Bank, sf::Sound>::value_type((SFX_Bank)i, *new_sound));
	}

	return 0;
}

void Game::PlaySFX(SFX_Bank sfx_name)
{
	m_sounds[sfx_name].play();
}

void Game::addToScene(GameObject* object, bool during_update)
{
	if (during_update == false)
		m_sceneGameObjects.push_back(object);
	else
		m_sceneGameObjectsCreated.push_back(object);
}

void Game::addToRectangles(SFRectangle& rectangle)
{
	m_sceneSFRectangles.push_back(&rectangle);
}

void Game::addToTexts(SFText& text)
{
	m_sceneSFTexts.push_back(&text);
}

void Game::addToPanels(SFPanel* panel)
{
	m_sceneSFPanels.push_back(panel);
}

void Game::addToTextPops(SFTextPop* textpop)
{
	m_sceneSFTextPops.push_back(textpop);
}

void Game::addToGuidedMissiles(GameObject* missile)
{
	m_sceneGuidedMissiles.push_back(missile);
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneGameObjects.size(), this->garbage.size());

	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Clearing garbage and updating the rest + adding newly created objects
	for (int i = 0; i < NBVAL_Layer; i++)
		m_sceneGameObjectsLayered[i].clear();
	for (int i = 0; i < NBVAL_GameObject; i++)
		m_sceneGameObjectsTyped[i].clear();

	//handling guided missiles specifically, because we need to clean their target if target is to be "garbaged"
	vector<GameObject*> sceneGameGuidedMissiles_tmp;
	for (GameObject* missile : m_sceneGuidedMissiles)
	{
		if (missile->m_garbageMe == false)
		{
			sceneGameGuidedMissiles_tmp.push_back(missile);
			missile->ClearTargetIfGarbage();
		}
	}

	m_sceneGuidedMissiles.clear();
	for (GameObject* missile : sceneGameGuidedMissiles_tmp)
		m_sceneGuidedMissiles.push_back(missile);

	//delete "garbage" objects, keep the rest in a temporary vector
	vector<GameObject*> sceneGameObjects_tmp;
	for (GameObject* object : m_sceneGameObjects)
	{
		object->GarbageWhenOutOfScreen();

		if (object->m_garbageMe == true)
			delete object;
		else
		{
			sceneGameObjects_tmp.push_back(object);

			m_sceneGameObjectsTyped[object->m_collider_type].push_back(object);
			m_sceneGameObjectsLayered[object->m_layer].push_back(object);
		}
	}

	//add in newly created objects
	for (GameObject* object : m_sceneGameObjectsCreated)
		if (object->m_garbageMe == true)
			delete object;
		else
		{
			sceneGameObjects_tmp.push_back(object);

			m_sceneGameObjectsTyped[object->m_collider_type].push_back(object);
			m_sceneGameObjectsLayered[object->m_layer].push_back(object);
		}
			
	m_sceneGameObjects.clear();
	m_sceneGameObjectsCreated.clear();

	//update all remaining objects and order them by collider type and layer type
	for (GameObject* object : sceneGameObjects_tmp)
	{
		object->update(deltaTime, m_hyperspeedMultiplier);
		m_sceneGameObjects.push_back(object);//assuming we will never browse this vector directly during the update of objects
	}

	//Checking colisions
	colisionChecksV2(deltaTime);

	//HUD ELEMENTS
	//SFRectangles
	vector<SFRectangle*> sceneSFRectangles_tmp;
	for (SFRectangle* rect : m_sceneSFRectangles)
		if (rect->m_garbageMe == false)//we don't delete the "garbage" ones because the pointer is a reference to a member object of class Enemy, who will destroy them in its destructor
			sceneSFRectangles_tmp.push_back(rect);

	m_sceneSFRectangles.clear();
	for (SFRectangle* rect : sceneSFRectangles_tmp)
		m_sceneSFRectangles.push_back(rect);

	//SFTexts
	vector<SFText*> sceneSFTexts_tmp;
	for (SFText* text : m_sceneSFTexts)
		if (text->m_garbageMe == false)//we don't delete the "garbage" ones because the pointer is a reference to a member object of class Enemy, who will destroy them in its destructor
			sceneSFTexts_tmp.push_back(text);

	m_sceneSFTexts.clear();
	for (SFText* text : sceneSFTexts_tmp)
		m_sceneSFTexts.push_back(text);

	//SFPanels
	vector<SFPanel*> sceneSFPanels_tmp;
	for (SFPanel* panel : m_sceneSFPanels)
		if (panel->m_garbageMe == true)
			delete panel;
		else
			sceneSFPanels_tmp.push_back(panel);

	m_sceneSFPanels.clear();
	for (SFPanel* panel : sceneSFPanels_tmp)
	{
		panel->Update(deltaTime, InputGuy::getDirections());
		m_sceneSFPanels.push_back(panel);
	}
		
	//SFTextPops
	vector<SFTextPop*> sceneSFTextPops_tmp;
	for (SFTextPop* textpop : m_sceneSFTextPops)
		if (textpop->m_garbageMe == true)
			delete textpop;
		else
			sceneSFTextPops_tmp.push_back(textpop);

	m_sceneSFTextPops.clear();
	for (SFTextPop* textpop : sceneSFTextPops_tmp)
	{
		textpop->update(deltaTime, m_hyperspeedMultiplier);
		m_sceneSFTextPops.push_back(textpop);
	}

	//Update music transitions
	ManageMusicTransitions(deltaTime);
}

void Game::killGameObjectType(GameObjectType type)
{
	for (GameObject* object : m_sceneGameObjectsTyped[type])
	{
		if (object->m_isOnScene == true)
		{
			object->Death();

			//Combo
			//if (type == EnemyFire)
			//{
			//	GameObject* obj = (GameObject*)m_playership;
			//	obj->AddComboCount(10);
			//}
		}
		
	}
}

void Game::damageGameObjectType(GameObjectType type, int damage)
{
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type].begin(); it != m_sceneGameObjectsTyped[type].end(); it++)
		if ((*it)->m_isOnScene == true)
			(*it)->GetDamage(damage);
}

void Game::drawScene()
{
	m_mainScreen.clear();

	float black_stripe = (REF_WINDOW_RESOLUTION_X - (SCENE_SIZE_X * 4.0f / 3)) / 2;

	//drawing stuff
	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == FeedbacksLayer)
		{
			for (SFRectangle* rect : m_sceneSFRectangles)
				if (rect->m_visible == true)
					m_mainScreen.draw(*rect);

			for (SFText* text : m_sceneSFTexts)
				if (text->m_visible == true)
					m_mainScreen.draw(*text);

			for (SFTextPop* textpop : m_sceneSFTextPops)
				if (textpop->m_visible == true)
					m_mainScreen.draw(*textpop);
		}
		else if (i == BlackStripesLayer)
		{
			//adding black stripes on the left and right
			
			sf::RectangleShape blackStripeRight;// , blackStripeLeft;
			//blackStripeLeft.setSize(sf::Vector2f(black_stripe, REF_WINDOW_RESOLUTION_Y));
			blackStripeRight.setSize(sf::Vector2f(black_stripe, REF_WINDOW_RESOLUTION_Y));
			//blackStripeLeft.setFillColor(sf::Color(0, 0, 0, 255));
			blackStripeRight.setFillColor(sf::Color(0, 0, 0, 255));
			//blackStripeLeft.setOrigin(0, 0);
			blackStripeRight.setOrigin(0, 0);
			//blackStripeLeft.setPosition(200, 0);
			blackStripeRight.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - 2*black_stripe, 0));
			//m_mainScreen.draw(blackStripeLeft);
			m_mainScreen.draw(blackStripeRight);
		}
		else if (i == PanelLayer)
		{
			for (SFPanel* panel : m_sceneSFPanels)
				panel->Draw(m_mainScreen);
		}
		else
		{
			for (GameObject* object : m_sceneGameObjectsLayered[i])
				object->Draw(m_mainScreen);	
		}
	}

	m_mainScreen.display();
	sf::Sprite temp(m_mainScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	
	temp.setPosition(sf::Vector2f(m_scale_factor.x * black_stripe, 0));
	m_window->draw(temp);
}

void Game::colisionChecksV2(Time deltaTime)
{
	sf::Clock dt;
	dt.restart();

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[PlayerShip].begin(); it1 != m_sceneGameObjectsTyped[PlayerShip].end(); it1++)
	{
		//Enemy bullets hitting the player
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[EnemyFire].begin(); it2 != m_sceneGameObjectsTyped[EnemyFire].end(); it2++)
		{
			if (SimpleCollision::IsGrazing((*it1), (*it2)))
			{
				(*it1)->GetGrazing(deltaTime, m_hyperspeedMultiplier);

				if (SimpleCollision::AreColliding((*it1), (*it2)))
					(*it1)->GetDamageFrom(*(*it2));
			}
		}

		//Enemy objects
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[EnemyObject].begin(); it2 != m_sceneGameObjectsTyped[EnemyObject].end(); it2++)
		{
			if (SimpleCollision::AreColliding((*it1), (*it2)))
			{
				(*it1)->GetDamageFrom(*(*it2));
				(*it2)->GetDamageFrom(*(*it1));
			}
		}

		//Loot
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[LootObject].begin(); it2 != m_sceneGameObjectsTyped[LootObject].end(); it2++)
		{
			GameObject* fake_ship = (*it1)->GetFakeShip();
			if (SimpleCollision::AreColliding(fake_ship, *it2, false))
			{
				//Do something (like, take the loot)
				if ((*it1)->GetLoot((*(*it2))))
				{
					(*it2)->m_visible = false;
					(*it2)->m_garbageMe = true;
				}
			}
		}

		//Portal
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[PortalObject].begin(); it2 != m_sceneGameObjectsTyped[PortalObject].end(); it2++)
		{
			if (SimpleCollision::AreColliding((*it1), (*it2)))
				(*it1)->GetPortal((*it2));
		}

		//Shop
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[ShopObject].begin(); it2 != m_sceneGameObjectsTyped[ShopObject].end(); it2++)
		{
			if (SimpleCollision::AreColliding((*it1), (*it2)))
				(*it1)->GetShop((*it2));
		}
	}

	//Then, check if any allied bullet collide with any enemy
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[EnemyObject].begin(); it1 != m_sceneGameObjectsTyped[EnemyObject].end(); it1++)
	{
		//Player bullets on enemy
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[FriendlyFire].begin(); it2 != m_sceneGameObjectsTyped[FriendlyFire].end(); it2++)
		{
			//Bullets impact on enemy
			if (SimpleCollision::AreColliding((*it1), (*it2)))
				(*it1)->GetDamageFrom(*(*it2));
		}
	}

	//First, Checks if the ship has been touched by an enemy/enemy bullet
	for (std::vector<GameObject*>::iterator it1 = m_sceneGameObjectsTyped[FakePlayerShip].begin(); it1 != m_sceneGameObjectsTyped[FakePlayerShip].end(); it1++)
	{
		for (std::vector<GameObject*>::iterator it2 = m_sceneGameObjectsTyped[LootObject].begin(); it2 != m_sceneGameObjectsTyped[LootObject].end(); it2++)
		{
			if (SimpleCollision::AreColliding((*it1), (*it2), false))
			{
				//Do something (like, take the loot)
				if ((*it1)->GetLoot((*(*it2))))
				{
					(*it2)->m_visible = false;
					(*it2)->m_isOnScene = false;
					(*it2)->m_garbageMe = true;
				}
			}
		}
	}
	//printf("| Collision: %d \n",dt.getElapsedTime().asMilliseconds());
}

void Game::garbageLayer(LayerType layer, bool only_offscene, bool delete_DontGarbageMe)
{
	if (layer == FeedbacksLayer)
	{
		for (SFTextPop* textpop : m_sceneSFTextPops)
			if (textpop->m_DontGarbageMe == false)
				textpop->GarbageMe();

		return;
	}

	for (GameObject* object : m_sceneGameObjectsLayered[layer])
	{
		if (only_offscene == true && object->m_isOnScene == true)
			continue;

		if (object->m_DontGarbageMe == true && delete_DontGarbageMe == false)
			continue;

		object->m_visible = false;
		object->m_garbageMe = true;

		//don't count them as "spawned" enemies if we cut them off this way
		if (layer == EnemyObjectLayer)
			m_hazardSpawned -= object->m_money;
	}

	for (GameObject* object : m_sceneGameObjectsCreated)
	{
		if (only_offscene == true && object->m_isOnScene == true)
			continue;

		if (object->m_layer != layer)
			continue;

		if (object->m_DontGarbageMe == true)
			continue;

		object->m_visible = false;
		object->m_garbageMe = true;

		//don't count them as "spawned" enemies if we cut them off this way
		if (layer == EnemyObjectLayer)
			m_hazardSpawned -= object->m_money;
	}
}

void Game::SetLayerRotation(LayerType layer, float angle)
{
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[layer].begin(); it != m_sceneGameObjectsLayered[layer].end(); it++)
	{
		if (*it == NULL)
			continue;

		(*it)->setRotation(angle);
	}
}

void Game::SetLayerSpeed(LayerType layer, sf::Vector2f speed)
{
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsLayered[layer].begin(); it != m_sceneGameObjectsLayered[layer].end(); it++)
	{
		if (*it == NULL)
			continue;

		(*it)->m_speed = speed;
	}
}

bool Game::isLastEnemyDead()
{
	return m_sceneGameObjectsTyped[EnemyFire].empty() == true && m_sceneGameObjectsTyped[EnemyObject].empty() == true && m_sceneGameObjectsCreated.empty() == true;
}

int Game::getHazard()
{
	return m_hazard;
}

void Game::resetHazard(int hazard_overkill)
{
	m_hazard = hazard_overkill;
	m_hazardSpawned = 0;
}

float Game::GetBeastScoreBonus(float playershipBeastScore, float sceneBeastScore)
{
	m_BeastScoreBonus = playershipBeastScore + sceneBeastScore;
	return m_BeastScoreBonus;

}

GameObject* Game::GetNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range)
{
	float shortest_distance = -1;

	if (type == PlayerShip && m_playership != NULL)
		return (GameObject*)m_playership;
	
	GameObject* object_found = NULL;
	for (GameObject* object : m_sceneGameObjectsTyped[type])
	{
		if (object->m_isOnScene == true && object->m_ghost == false)
		{
			const float a = ref_position.x - object->getPosition().x;
			const float b = ref_position.y - object->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				object_found = object;
			}
		}
	}

	return object_found;
}

float Game::GetAngleToNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range)
{
	float angle = 0.f;
	sf::Vector2f pos;
	float shortest_distance = -1;
	for (std::vector<GameObject*>::iterator it = m_sceneGameObjectsTyped[type].begin(); it != m_sceneGameObjectsTyped[type].end(); it++)
	{
		if (*it == NULL)
			continue;

		if ((*it)->m_isOnScene && !(*it)->m_ghost)
		{
			const float a = ref_position.x - (*it)->getPosition().x;
			const float b = ref_position.y - (*it)->getPosition().y;

			float distance_to_ref = (a * a) + (b * b);
			//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
			if (distance_to_ref < shortest_distance || shortest_distance < 0)
			{
				shortest_distance = distance_to_ref;
				pos = (*it)->getPosition();
			}
		}
	}
	if (shortest_distance > 0)
	{
		if (range > 0 && (range * range) < shortest_distance)//shortest_distance is already squared
		{
			//do nothing: range is too short to reach the target, therefore we don't aim for it.
		}
		else
		{
			shortest_distance = sqrtf(shortest_distance);
			//angle = acos((ref_position.y - pos.y) / shortest_distance);
			sf::Vector2f diff_position = sf::Vector2f((ref_position.y - pos.y), (ref_position.x - pos.x));
			angle = acos(diff_position.x / shortest_distance);
			angle = angle * 180 / M_PI;

			//if (ref_position.x < pos.x)
			if (diff_position.y < 0)
				angle = - angle;
		}
	}
	//if no target found, the default value for enemies is 180 degrees (firing straight ahead)
	else if (type == PlayerShip)
		return 180;

	return angle;
}

void Game::WakeUpEnemiesWithName(string display_name)
{
	for (GameObject* object : m_sceneGameObjectsTyped[EnemyObject])
	{
		if (object->m_display_name.compare(display_name) == 0)
		{
			object->m_wake_up = true;
			object->CheckCondition();//apply phase change directly
		}
	}
}

int Game::GetPlayerStatsMultiplierForLevel(int level)
{
	int result_ = round(100 * (pow((1 + PLAYER_LEVELING_STATS_MULTIPLIER), (level - 1))));
	return result_;
}

int Game::GetEnemiesStatsMultiplierForLevel(int level)
{
	int result_ = round(100 * (pow((1 + ENEMIES_LEVELING_STATS_MULTIPLIER), (level - 1))));
	return result_;
}

EquipmentQuality Game::GetItemQualityClass(float quality)
{
	if (quality < 25)
	{
		return ItemQuality_Poor;
	}
	else if (quality < 50)
	{
		return ItemQuality_Medium;
	}
	else if (quality < 75)
	{
		return ItemQuality_Good;
	}
	else
	{
		return ItemQuality_Epic;
	}
}

void Game::DamageObjectsInRange(LayerType layer, sf::Vector2f position, float range, int damage)
{
	for (GameObject* object : m_sceneGameObjectsLayered[layer])
	{
		if (object->m_immune == true || object->m_isOnScene == false)
			continue;

		sf::Vector2f diff_position = sf::Vector2f((object->getPosition().y - position.y), (object->getPosition().x - position.x));
		float distance_square = diff_position.x * diff_position.x + diff_position.y * diff_position.y;
		
		if (distance_square < (range + object->m_diag) * (range + object->m_diag))
			object->GetDamage(damage);
	}
}