#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include <SFML/Audio.hpp>
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "InputGuy.h"

using namespace sf;

class GameEntity;//forward declaration
class RoomTile;
class WaterZone;
class WaterTile;
class Ship;
class Ammo;
class FX;

enum SFX_Bank
{
	SFX_Laser,
	NBVAL_SFX_BANK,
};

enum Music_Bank
{
	Music_Main,
	NBVAL_MUSIC_BANK,
};

enum FontsStyle
{
	Font_Arial,//0
	Font_Terminator,//1
	NBVAL_FontsStyle,//2
};

enum MouseAction
{
	Mouse_None,
	Mouse_LeftClick,
	Mouse_RightClick,
};

enum ShipType
{
	Ship_Warship,
	Ship_FirstClass,
	Ship_SecondClass,
	NB_SHIP_TYPES,
};

enum CrewSkills
{
	Skill_Gunner,
	Skill_Fishing,
	Skill_Melee,
	Skill_Navigation,
	Skill_Cooking,
	NB_CREW_SKILLS,
};

enum CrewMemberRace
{
	Race_Human,
	Race_Fishman,
	Race_Mecha,
	NB_CREW_RACES,
};

enum RoomType
{
	Room_Empty,
	Room_Crewquarter,
	Room_Navigation,
	Room_Weapon,
	Room_Gold,
	Room_Fishing,
	Room_Prison,
	Room_Diving,
	Room_Sword,
	Room_Lifeboat,
	Room_Ammo,
	Room_Engine,
	NB_ROOM_TYPES,
};

enum CrewMemberType
{
	Crew_All,
	Crew_Pirate,
	Crew_Civilian,
	Crew_Slave,
	Crew_Undead,
	NB_CREW_TYPES,
};

enum Input_Status
{
	Input_Tap,
	Input_Hold,
	Input_Release
};

enum Input_Actions
{
	Action_Pause,
	NB_INPUT_ACTIONS,
};

enum Color_Chart
{
	Color_Yellow_Target,
	Color_Grey_Target,
	Color_Orange_System,
	Color_Green_System,
	Color_Cyan_System,
	Color_Blue_Flood,
	Color_Blue_Pierced,
	NB_COLOR_CHART,
};

struct Game
{
public:
	Game(RenderWindow& window);
	~Game();
	RenderWindow* getMainWindow();
	void CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY);

	void updateScene(Time deltaTime);
	void drawScene();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_vspeed;
	sf::Vector2f m_scale_factor;

	bool m_window_has_focus;

	sf::Vector2f m_mouse_pos;
	MouseAction m_mouse_click;
	float m_mouse_click_timer;
	void GetMouseInputs(sf::Time deltaTime);
	Input_Status m_input_actions[NB_INPUT_ACTIONS];
	void GetControllerInputs();

	sf::View m_view;
	sf::Vector2f m_map_size;

	//Fonts
	sf::Font* m_font[NBVAL_FontsStyle];

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	void SetSFXVolume(bool activate_sfx);

	sf::SoundBuffer m_soundBuffers[1];
	sf::Sound m_sounds[1];
	bool m_SFX_Activated;

	//Music
	sf::Music m_curMusic;
	bool m_Music_Activated;
	string m_next_music_name;
	Music_Bank m_curMusic_type;
	float m_music_fader;
	bool m_asking_music_fade_out;

	void SetMusicVolume(bool activate_music);
	void PlayMusic(Music_Bank music, string specific_filename = "");
	void ManageMusicTransitions(sf::Time deltaTime);

	//PIRATE
	GameEntity* m_selected_ui;
	GameEntity* m_hovered_ui;
	GameEntity* m_target_ui;
	GameEntity* m_play_ui;

	vector<RoomTile*> m_tiles;
	vector<RoomTile*> m_enemy_tiles;
	vector<vector<WaterZone*> > m_waterzones;
	vector<Ammo*> m_bullets;
	vector<FX*> m_FX;
	bool m_pause;

	string m_dico_ship_class[NB_SHIP_TYPES];
	string m_dico_room_types[NB_ROOM_TYPES];
	vector<string> m_dico_crew_names[2];
	vector<string> m_dico_crew_names_used[2];
	string m_dico_crew_types[NB_CREW_TYPES];
	string m_dico_crew_skills[NB_CREW_SKILLS];
	string m_dico_crew_races[NB_CREW_RACES];
	string GetRandomCrewMemberName(int gender);
	sf::Color m_dico_colors[NB_COLOR_CHART];

private:

	RenderWindow* m_window;
};

#endif // GAME_H_INCLUDED