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
	m_mouse_click = Mouse_None;

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
	m_dico_room_types[Room_Diving] = "Diving";
	m_dico_room_types[Room_Prison] = "Prison";
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
	m_dico_crew_skills[Skill_Combat] = "Combat";
	m_dico_crew_skills[Skill_Navigation] = "Navigation";
	m_dico_crew_skills[Skill_Engine] = "Engine";
	m_dico_crew_skills[Skill_Diving] = "Diving";

	m_dico_crew_races[Race_Human] = "Human";
	m_dico_crew_races[Race_Fishman] = "Fishman";
	m_dico_crew_races[Race_Mecha] = "Mechanoid";

	

	//load pirate names
	vector<vector<string> > names_male = *(FileLoaderUtils::FileLoader(PIRATE_NAMES_MALE_CSV));
	for (vector<vector< string> >::iterator it = names_male.begin(); it != names_male.end(); it++)
	{
		m_dico_crew_names[0].push_back((*it).front());
	}

	vector<vector<string> > names_female = *(FileLoaderUtils::FileLoader(PIRATE_NAMES_FEMALE_CSV));
	for (vector<vector< string> >::iterator it = names_female.begin(); it != names_female.end(); it++)
	{
		m_dico_crew_names[1].push_back((*it).front());
	}

	vector<vector<string> > surnames = *(FileLoaderUtils::FileLoader(PIRATE_SURNAMES_CSV));
	for (vector<vector< string> >::iterator it = surnames.begin(); it != surnames.end(); it++)
	{
		m_dico_crew_surnames.push_back((*it).front());
	}

	vector<vector<string> > island_names = *(FileLoaderUtils::FileLoader(ISLAND_NAMES_CSV));
	for (vector<vector< string> >::iterator it = island_names.begin(); it != island_names.end(); it++)
	{
		m_dico_islands_names.push_back((*it).front() + " Island");
	}

	vector<vector<string> > ship_names_first_class = *(FileLoaderUtils::FileLoader(SHIP_NAMES_FIRSTCLASS_CSV));
	for (vector<vector< string> >::iterator it = ship_names_first_class.begin(); it != ship_names_first_class.end(); it++)
	{
		m_dico_ships_names[Ship_FirstClass].push_back(StringReplace((*it).front(), "_", " "));
	}

	vector<vector<string> > ship_names_second_class = *(FileLoaderUtils::FileLoader(SHIP_NAMES_SECONDCLASS_CSV));
	for (vector<vector< string> >::iterator it = ship_names_second_class.begin(); it != ship_names_second_class.end(); it++)
	{
		m_dico_ships_names[Ship_SecondClass].push_back(StringReplace((*it).front(), "_", " "));
	}

	m_dico_resources_textures[Resource_Gold] = "2D/icon_gold.png";
	m_dico_resources_textures[Resource_Fish] = "2D/icon_fish.png";
	m_dico_resources_textures[Resource_Mech] = "2D/icon_mech.png";
	m_dico_resources_textures[Resource_Fidelity] = "2D/icon_fidelity.png";
	m_dico_resources_textures[Resource_Days] = "2D/icon_days.png";
	m_dico_resources_textures[Resource_Crew] = "2D/icon_crew.png";

	m_dico_colors[Color_Red_Destroyed] = sf::Color::Red;
	m_dico_colors[Color_Yellow_Target_Hovered] = sf::Color(255, 242, 39, 255);
	m_dico_colors[Color_DarkGrey_Background] = sf::Color(100, 100, 100, 255);
	m_dico_colors[Color_VeryDarkGrey_Background] = sf::Color(30, 30, 30, 255);
	m_dico_colors[Color_Orange_System] = sf::Color(255, 127, 39, 255);
	m_dico_colors[Color_Green_System] = sf::Color::Green;
	m_dico_colors[Color_Cyan_System] = sf::Color::Cyan;
	m_dico_colors[Color_Blue_Flood] = sf::Color(0, 100, 170, 255);
	m_dico_colors[Color_Blue_Pierced] = sf::Color(0, 50, 255, 255);
	m_dico_colors[Color_Red_Prison] = sf::Color(237, 22, 33, 255);
	m_dico_colors[Color_Yellow_Prisoner] = sf::Color(255, 242, 39, 255);
	m_dico_colors[Color_Magenta_Crew] = sf::Color::Magenta;

	m_dico_colors[Color_Red_Target_Locked] = m_dico_colors[Color_Red_Destroyed];
	m_dico_colors[Color_Red_CrewFidelity_Negative] = m_dico_colors[Color_Red_Destroyed];
	m_dico_colors[Color_Blue_CrewFidelity_Positive] = m_dico_colors[Color_Blue_Flood];

	m_dico_colors[Color_Blue_Water] = m_dico_colors[Color_Blue_Flood];
	m_dico_colors[Color_Green_Lifebar] = m_dico_colors[Color_Green_System];
	m_dico_colors[Color_Red_Impossible] = m_dico_colors[Color_Red_Target_Locked];
	m_dico_colors[Color_Magenta_EngineCharged] = m_dico_colors[Color_Magenta_Crew];

	//load database files
	m_choices_config = *(FileLoaderUtils::FileLoader(CHOICES_CSV_FILE));
	m_rewards_config = *(FileLoaderUtils::FileLoader(REWARDS_CSV_FILE));
	m_commodities_config = *(FileLoaderUtils::FileLoader(COMMODITY_CSV_FILE));
}

string Game::GetRandomNameFromDico(vector<string>& dico, vector<string>& dico_used)
{
	//get a name at random in the list
	int r = RandomizeIntBetweenValues(0, dico.size() - 1);
	string name = dico[r];

	//remove from the list and keep it stored in the "used" list
	dico_used.push_back(dico[r]);
	dico.erase(dico.begin() + r);

	//all names are alreay used? recycle the list
	if (dico.empty() == true)
	{
		for (vector<string>::iterator it = dico_used.begin(); it != dico_used.end(); it++)
		{
			dico.push_back(*it);
		}
		dico_used.clear();
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

	for (vector<SFTextPop*>::iterator it = m_texts_pop.begin(); it != m_texts_pop.end(); it++)
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
			case Music_Combat:
			{
				m_next_music_name = makePath("Music/Pirate_GoingToWar.ogg");
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

	if (InputGuy::isSaving() == true)
	{
		m_input_actions[Action_Saving] = m_input_actions[Action_Saving] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_input_actions[Action_Saving] = Input_Release;
	}

	if (InputGuy::isEntering() == true)
	{
		m_input_actions[Action_Entering] = m_input_actions[Action_Entering] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_input_actions[Action_Entering] = Input_Release;
	}
}