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
	//view.zoom(0.3f);

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
	m_Music_Activated = false;
	m_music_fader = 0;
	PlayMusic(Music_Main);

	//Inputs
	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_actions_states[i] = false;
	}

	//Background
	m_background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(1920, 1080));
	m_map_size = m_background->m_size;

	//Liaison 16
	//rectangular selection
	m_rectangular_selection.setOutlineColor(sf::Color::White);
	m_rectangular_selection.setOutlineThickness(-1);
	m_rectangular_selection.setFillColor(sf::Color(0, 0, 0, 0));
}

Game::~Game()
{
	for (vector<WaveBounce*>::iterator it = m_wave_bounces.begin(); it != m_wave_bounces.end(); it++)
	{
		delete *it;
	}

	for (vector<WaveReception*>::iterator it = m_wave_receptions.begin(); it != m_wave_receptions.end(); it++)
	{
		delete *it;
	}

	for (vector<L16Entity*>::iterator it = m_L16_entities.begin(); it != m_L16_entities.end(); it++)
	{
		delete *it;
	}
	
	for (vector<Wave*>::iterator it = m_waves.begin(); it != m_waves.end(); it++)
	{
		delete *it;
	}

	for (vector<Link*>::iterator it = m_links.begin(); it != m_links.end(); it++)
	{
		delete *it;
	}

	delete m_background;
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
				m_next_music_name = makePath("Music/Main.ogg");
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

void Game::UpdateRectangularSelection()
{
	if (m_display_rectangular_selection == true && m_mouse_click == Mouse_None)
	{
		m_released_rectangular_selection = true;
	}
	else
	{
		m_released_rectangular_selection = false;
	}

	m_display_rectangular_selection = false;
	if (m_mouse_click == Mouse_LeftClick)
	{
		m_rectangular_selection.setSize(sf::Vector2f(0, 0));
		m_rectangular_selection.setPosition(m_mouse_pos);
	}
	else if (m_mouse_click == Mouse_LeftClickHold)
	{
		m_rectangular_selection.setSize(m_mouse_pos - m_rectangular_selection.getPosition());
		m_display_rectangular_selection = (m_rectangular_selection.getSize().x != 0 || m_rectangular_selection.getSize().y != 0);
	}
}

void Game::updateScene(Time deltaTime)
{
	//printf("OnScene: %d / Collected: %d\n", this->sceneGameObjects.size(), this->garbage.size());

	//TODO: Updating screen resolution
	m_scale_factor.x = 1.0f * m_screen_size.x / REF_WINDOW_RESOLUTION_X;
	m_scale_factor.y = 1.0f * m_screen_size.y / REF_WINDOW_RESOLUTION_Y;

	//Checking colisions
	CheckCollisions();

	//Update inputs
	UpdateInputStates();
	UpdateRectangularSelection();

	//Update game objects
	vector<CircleObject*> sceneCircleObjets_tmp;
	for (int i = 0; i < NB_ALLIANCE_TYPES; i++)
		for (int j = 0; j < NB_CIRCLE_TYPES; j++)
		{
			for (CircleObject* object : m_sceneCircleObjects[i][j])
			{
				if (object->m_garbageMe == true)
					delete object;
				else
					sceneCircleObjets_tmp.push_back(object);
			}

			m_sceneCircleObjects[i][j].clear();
		}
	
	m_L16_entities.clear();
	m_waves.clear();

	for (CircleObject* object : sceneCircleObjets_tmp)
	{
		object->update(deltaTime);
		m_sceneCircleObjects[object->m_alliance][object->m_circle_type].push_back(object);
	}

	for (CircleObject* object : m_new_sceneCircleObjects)
	{
		m_sceneCircleObjects[object->m_alliance][object->m_circle_type].push_back(object);

		if (object->m_circle_type == Circle_Wave || object->m_circle_type == Circle_WaveBounce)
			m_waves.push_back((Wave*)object);
		else
			m_L16_entities.push_back((L16Entity*)object);
	}
		
	m_new_sceneCircleObjects.clear();

	//Update music transitions
	ManageMusicTransitions(deltaTime);

	//Update view
	m_mainScreen.setView(m_view);
}

void Game::drawScene()
{
	m_mainScreen.clear();

	m_mainScreen.draw(*m_background);

	for (int i = 0; i < NBVAL_Layer; i++)
	{
		if (i == NodeLayer)
		{
			for (int j = 0; j < NB_ALLIANCE_TYPES; j++)
			{
				for (int k = 0; k < NB_CIRCLE_TYPES; k++)
				{
					for (vector<CircleObject*>::iterator it = m_sceneCircleObjects[j][k].begin(); it != m_sceneCircleObjects[j][k].end(); it++)
					{
						//if ((*it)->m_visible == true || (*it)->m_alliance == PlayerAlliance)
						{
							(*it)->Draw(m_mainScreen);
						}
					}
				}
				
				for (vector<LineObject*>::iterator it = m_sceneLineObjects[j].begin(); it != m_sceneLineObjects[j].end(); it++)
				{
					//m_mainScreen.draw((*(*it)).m_points, 2, sf::Lines);
					m_mainScreen.draw((*(*it)).m_quad, 4, sf::Quads);
				}
			}
		}
		else if (i == RectangularSelectionLayer)
		{
			if (m_display_rectangular_selection == true)
			{
				m_mainScreen.draw(m_rectangular_selection);
			}
		}
	}

	m_mainScreen.display();
	sf::Sprite temp(m_mainScreen.getTexture());
	temp.scale(m_scale_factor.x, m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	m_window->draw(temp);
}


void Game::CheckCollisions()
{
	//Wave collisions with everything else
	for (int i = 0; i < NB_ALLIANCE_TYPES; i++)
	{
		for (int j = 0; j < NB_ALLIANCE_TYPES; j++)
		{
			vector<CircleType> types_to_check;
			types_to_check.push_back(Circle_L16Entity_Air);
			types_to_check.push_back(Circle_L16Entity_Ground);
			types_to_check.push_back(Circle_L16Ballistic_Air);
			types_to_check.push_back(Circle_L16Ballistic_MultiDomain);

			if (i != j)
			{
				for (vector<CircleObject*>::iterator it = m_sceneCircleObjects[i][Circle_Wave].begin(); it != m_sceneCircleObjects[i][Circle_Wave].end(); it++)
				{
					//if ((*it)->m_visible == false)
					//	continue;

					for (vector<CircleType>::iterator it2 = types_to_check.begin(); it2 != types_to_check.end(); it2++)
					{
						for (vector<CircleObject*>::iterator it3 = m_sceneCircleObjects[j][*it2].begin(); it3 != m_sceneCircleObjects[j][*it2].end(); it3++)
						{
							WaveCollisionCheck(*it, *it3);
						}
					}
				}
			}
			else
			{
				for (vector<CircleObject*>::iterator it = m_sceneCircleObjects[i][Circle_WaveBounce].begin(); it != m_sceneCircleObjects[i][Circle_WaveBounce].end(); it++)
				{
					//if ((*it)->m_visible == false)
					//	continue;

					for (vector<CircleType>::iterator it2 = types_to_check.begin(); it2 != types_to_check.end(); it2++)
					{
						for (vector<CircleObject*>::iterator it3 = m_sceneCircleObjects[j][*it2].begin(); it3 != m_sceneCircleObjects[j][*it2].end(); it3++)
						{
							WaveCollisionCheck(*it, *it3);
						}
					}
				}
			}
		}
	}

	for (vector<WaveBounce*>::iterator it = m_wave_bounces.begin(); it != m_wave_bounces.end(); it++)
	{
		CircleObject* object = (CircleObject*)(*it)->m_bounced_entity;
		object->CreateWaveBounce((*it)->m_position, (*it)->m_radius, (*it)->m_direction, (*it)->m_wave);

		delete *it;
	}
	m_wave_bounces.clear();

	for (vector<WaveReception*>::iterator it = m_wave_receptions.begin(); it != m_wave_receptions.end(); it++)
	{
		CircleObject* object = (CircleObject*)(*it)->m_entity;
		object->WaveReception((*it)->m_wave);
	}
	m_wave_receptions.clear();

	//Ballistic collision
	for (int i = 0; i < NB_ALLIANCE_TYPES; i++)
	{
		for (int j = 0; j < NB_ALLIANCE_TYPES; j++)
		{
			if (i == j)
				continue;

			for (vector<CircleObject*>::iterator it = m_sceneCircleObjects[i][Circle_L16Ballistic_MultiDomain].begin(); it != m_sceneCircleObjects[i][Circle_L16Ballistic_MultiDomain].end(); it++)
			{
				if ((*it)->m_visible == false)
					continue;

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Entity_Air].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Entity_Air].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Entity_Ground].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Entity_Ground].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Ballistic_Air].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Ballistic_Air].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Ballistic_MultiDomain].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Ballistic_MultiDomain].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}
			}

			for (vector<CircleObject*>::iterator it = m_sceneCircleObjects[i][Circle_L16Ballistic_Air].begin(); it != m_sceneCircleObjects[i][Circle_L16Ballistic_Air].end(); it++)
			{
				if ((*it)->m_visible == false)
					continue;

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Entity_Air].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Entity_Air].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Ballistic_Air].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Ballistic_Air].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}

				for (vector<CircleObject*>::iterator it2 = m_sceneCircleObjects[j][Circle_L16Ballistic_MultiDomain].begin(); it2 != m_sceneCircleObjects[j][Circle_L16Ballistic_MultiDomain].end(); it2++)
				{
					BallisticCollisionCheck(*it, *it2);
				}
			}
		}
	}
}

L16Entity* Game::GetClosestL16Target(sf::Vector2f position, float heading, CircleType collision_domain, AllianceType alliance, float dist_max, float angle_delta_max)
{
	float shortest_distance = -1;
	L16Entity* returned_entity = NULL;
	for (std::vector<L16Entity*>::iterator it = m_L16_entities.begin(); it != m_L16_entities.end(); it++)
	{
		CircleObject* obj = (CircleObject*)(*it);

		if (obj->m_alliance != alliance)
			continue;

		if (collision_domain == Circle_L16Ballistic_Air)
		{
			if (obj->m_circle_type == Circle_L16Entity_Ground)
				continue;
		}

		if (collision_domain == Circle_L16Ballistic_MultiDomain)
		{
			//ok.
		}

		if (obj->m_visible == false || obj->m_garbageMe == true)
			continue;

		const float a = position.x - obj->getPosition().x;
		const float b = position.y - obj->getPosition().y;

		float distance_to_ref = (a * a) + (b * b);
		//if the item is the closest, or the first one to be found, we are selecting it as the target, unless a closer one shows up in a following iteration
		if ((distance_to_ref <= dist_max * dist_max) && (distance_to_ref < shortest_distance || shortest_distance < 0))
		{
			float angle_delta = GetAngleDegToTargetPosition(position, heading, obj->getPosition());
			if (abs(angle_delta) <= angle_delta_max)
			{
				shortest_distance = distance_to_ref;
				returned_entity = (*it);
			}
		}
	}

	return returned_entity;
}

void Game::GetMouseInputs(sf::Time deltaTime)
{
	sf::Vector2i mousepos2i = sf::Mouse::getPosition(*getMainWindow());
	m_mouse_pos = getMainWindow()->mapPixelToCoords(mousepos2i, m_view);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_window_has_focus && m_mouse_click == Mouse_None)
	{
		m_mouse_click = Mouse_LeftClick;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_window_has_focus && (m_mouse_click == Mouse_LeftClick || m_mouse_click == Mouse_LeftClickHold))
	{
		m_mouse_click = Mouse_LeftClickHold;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && m_window_has_focus && m_mouse_click == Mouse_None)
	{
		m_mouse_click = Mouse_RightClick;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && m_window_has_focus && (m_mouse_click == Mouse_RightClick || m_mouse_click == Mouse_RightClickHold))
	{
		m_mouse_click = Mouse_RightClickHold;
	}
	else// if (sf::Mouse::isButtonPressed(sf::Mouse::Left) == false && sf::Mouse::isButtonPressed(sf::Mouse::Right) == false)//release
	{
		m_mouse_click = Mouse_None;
	}
}

//Liaison 16
void Game::AddCircleObject(CircleObject* object)
{
	m_new_sceneCircleObjects.push_back(object);
}

void Game::AddLineObject(LineObject* object)
{
	m_sceneLineObjects[object->m_alliance].push_back(object);
}

void Game::WaveCollisionCheck(CircleObject* object_wave, CircleObject* object_entity)
{
	if (object_wave == object_entity)
		return;

	//if (object_entity->m_visible == false)
	//	return;

	float dx = object_wave->getPosition().x - object_entity->getPosition().x;
	float dy = object_wave->getPosition().y - object_entity->getPosition().y;
	float radius = object_wave->getRadius() + object_entity->getRadius();
	if (dx*dx + dy*dy < radius*radius)
	{
		//this node already evaded this wave? no need to check again as it's 100% unlikely to happen
		L16Entity* entity = (L16Entity*)object_entity;
		if (object_wave->IsEvadedEntity(entity) == true)
		{
			return;
		}

		//already bounced on this node?
		if (object_wave->HasBouncedOnEntity(entity) == false)
		{
			//testing sector or circle
			Wave* wave = (Wave*)object_wave;
			float direction = GetAngleForVector(sf::Vector2f(object_entity->getPosition().x - object_wave->getPosition().x, -(object_entity->getPosition().y - object_wave->getPosition().y)));

			if (object_entity->IsColliding(wave, direction) == true)
			{
				//wave bounce on enemy
				if (object_wave->m_circle_type == Circle_Wave && object_wave->m_alliance != object_entity->m_alliance)
				{
					sf::Vector2f vector = object_entity->getPosition() - object_wave->getPosition();
					ScaleVector(&vector, object_wave->getRadius());
					sf::Vector2f position = object_wave->getPosition() + vector;

					m_wave_bounces.push_back(new WaveBounce(position, direction, object_entity->getRadius(), wave, entity));
				}
				//wave reception
				else if (object_wave->m_circle_type == Circle_WaveBounce &&  object_wave->m_alliance == object_entity->m_alliance)
				{
					m_wave_receptions.push_back(new WaveReception(wave, entity));
				}
			}
			else 
			{
				object_wave->AddToEvadedEntities(entity);
			}
		}
	}
}

void Game::BallisticCollisionCheck(CircleObject* object_ballistic, CircleObject* object_entity)
{
	if (object_ballistic == object_entity)
		return;

	if (object_entity->m_visible == false)
		return;

	float dx = object_ballistic->getPosition().x - object_entity->getPosition().x;
	float dy = object_ballistic->getPosition().y - object_entity->getPosition().y;
	float radius = object_ballistic->getRadius() + object_entity->getRadius();
	if (dx*dx + dy*dy < radius*radius)
	{
		//hit
		object_ballistic->m_garbageMe = true;
		object_entity->m_garbageMe = true;
	}
}

void Game::UpdateInputState(bool input_guy_boolean, PlayerActions action)
{
	if (input_guy_boolean)
	{
		m_inputs_states[action] = m_inputs_states[action] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_inputs_states[action] = Input_Release;
	}
}

void Game::UpdateInputStates()
{
	if (m_window_has_focus)
	{
		UpdateInputState(InputGuy::isFiring(), Action_Firing);
	}
	else
	{
		UpdateInputState(false, Action_Firing);
	}
}