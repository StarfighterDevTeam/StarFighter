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

Game::Game(RenderWindow& window)
{
	m_window_has_focus = true;
	m_mouse_click_timer = 0.2f;

	m_window = &window;
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
	printf("Loading Musics\n");
	m_Music_Activated = false;
	m_music_fader = 0;
	PlayMusic(Music_Main);

	//PIRATES
	m_selected_ui = NULL;
	m_hovered_ui = NULL;
	m_target_ui = NULL;
	m_play_ui = NULL;
	m_pause = false;
	for (int i = 0; i < NB_INPUT_ACTIONS; i++)
	{
		m_input_actions[i] = Input_Release;
	}

	//dico
	m_dico_ship_class[Ship_Warship] = "Pirate Warship";
	m_dico_ship_class[Ship_FirstClass] = "First Class";
	m_dico_ship_class[Ship_SecondClass] = "Second Class";

	m_dico_room_types[Room_Crewquarter] = "Crew Quarter";
	m_dico_room_types[Room_Navigation] = "Navigation";
	m_dico_room_types[Room_Weapon] = "Weapon";
	m_dico_room_types[Room_Gold] = "Gold";
	m_dico_room_types[Room_Fishing] = "Fishing";
	m_dico_room_types[Room_Kitchen] = "Kitchen";
	m_dico_room_types[Room_Relic] = "Relic";
	m_dico_room_types[Room_Sword] = "Sword";
	m_dico_room_types[Room_Lifeboat] = "Lifeboat";
	m_dico_room_types[Room_Ammo] = "Ammo";
	m_dico_room_types[Room_Engine] = "Engine";

	m_dico_crew_types[Crew_Pirate] = "Pirate";
	m_dico_crew_types[Crew_Civilian] = "Civilian";
	m_dico_crew_types[Crew_Slave] = "Slave";
	m_dico_crew_types[Crew_Undead] = "Undead";

	m_dico_crew_skills[Skill_Gunner] = "Gunner";
	m_dico_crew_skills[Skill_Fishing] = "Fishing";
	m_dico_crew_skills[Skill_Melee] = "Melee";
	m_dico_crew_skills[Skill_Navigation] = "Navigation";
	m_dico_crew_skills[Skill_Cooking] = "Cooking";

	m_dico_crew_races[Race_Human] = "Human";
	m_dico_crew_races[Race_Fishman] = "Fishman";
	m_dico_crew_races[Race_Mecha] = "Mechanoid";

	m_dico_crew_names[0].push_back("Joe");
	m_dico_crew_names[0].push_back("Bill");
	m_dico_crew_names[0].push_back("Daniel");
	m_dico_crew_names[0].push_back("Thomas");

	m_dico_crew_names[1].push_back("Leandra");
	m_dico_crew_names[1].push_back("Serena");
	m_dico_crew_names[1].push_back("Lucy");
	m_dico_crew_names[1].push_back("Angela");
}

string Game::GetRandomCrewMemberName(int gender)
{
	//get a name at random in the list
	int r = RandomizeIntBetweenValues(0, m_dico_crew_names[gender].size() - 1);
	string name = m_dico_crew_names[gender][r];

	//remove from the list and keep it stored in the "used" list
	m_dico_crew_names_used[gender].push_back(m_dico_crew_names[gender][r]);
	m_dico_crew_names[gender].erase(m_dico_crew_names[gender].begin() + r);

	//all names are alreay used? recycle the list
	if (m_dico_crew_names[gender].empty() == true)
	{
		for (vector<string>::iterator it = m_dico_crew_names_used[gender].begin(); it != m_dico_crew_names_used[gender].end(); it++)
		{
			m_dico_crew_names[gender].push_back(*it);
		}
		m_dico_crew_names_used[gender].clear();
	}

	return name;
}

Game::~Game()
{
	for (vector<vector<WaterZone*> >::iterator it = m_waterzones.begin(); it != m_waterzones.end(); it++)
	{
		for (vector<WaterZone*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			delete *it2;
		}
	}

	for (vector<Ammo*>::iterator it = m_bullets.begin(); it != m_bullets.end(); it++)
	{
		delete *it;
	}

	for (vector<FX*>::iterator it = m_FX.begin(); it != m_FX.end(); it++)
	{
		delete *it;
	}

	for (vector<RoomTile*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
	{
		delete *it;
	}

	for (vector<RoomTile*>::iterator it = m_enemy_tiles.begin(); it != m_enemy_tiles.end(); it++)
	{
		delete *it;
	}
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

//PIRATES

void Game::GetMouseInputs(sf::Time deltaTime)
{
	sf::Vector2i mousepos2i = sf::Mouse::getPosition(*getMainWindow());
	m_mouse_pos = getMainWindow()->mapPixelToCoords(mousepos2i, m_view);

	if (m_mouse_click_timer > 0)
	{
		m_mouse_click_timer -= deltaTime.asSeconds();
	}

	m_mouse_click = Mouse_None;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_window_has_focus && m_mouse_click_timer <= 0)
	{
		m_mouse_click = Mouse_LeftClick;
		m_mouse_click_timer = 0.2f;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && m_window_has_focus && m_mouse_click_timer <= 0)
	{
		m_mouse_click = Mouse_RightClick;
		m_mouse_click_timer = 0.2f;
	}
}

void Game::GetControllerInputs()
{
	if (InputGuy::isPausing() == true)
	{
		m_input_actions[Action_Pause] = m_input_actions[Action_Pause] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_input_actions[Action_Pause] = Input_Release;
	}
}