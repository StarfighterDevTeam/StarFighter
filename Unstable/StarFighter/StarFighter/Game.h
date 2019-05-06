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
#include "FileLoadUtils.h"

#include "InputGuy.h"

using namespace sf;

class GameEntity;//forward declaration
class RoomTile;
class WaterZone;
class WaterTile;
class Ship;
class Ammo;
class FX;
class SFTextPop;

#define SFTEXTPOP_SIZE_1			18
#define SFTEXTPOP_SIZE_2			30

#define SFTEXTPOP_OFFSET_1			20.f
#define SFTEXTPOP_OFFSET_2			30.f
#define SFTEXTPOP_OFFSET_2_BIS		50.f

enum SFX_Bank
{
	SFX_Laser,
	NBVAL_SFX_BANK,
};

enum Music_Bank
{
	Music_Main,
	Music_Combat,
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
	Mouse_LeftClickHold,
	Mouse_RightClick,
	Mouse_RightClickHold,
};

enum ShipType
{
	Ship_FirstClass,
	Ship_SecondClass,
	NB_SHIP_TYPES,
	Ship_Warship,
};

enum CrewSkills
{
	Skill_Gunner,
	Skill_Fishing,
	Skill_Combat,
	Skill_Navigation,
	Skill_Engine,
	Skill_Diving,
	NB_CREW_SKILLS,
};

enum CrewMemberRace
{
	Race_Human,
	Race_Fishman,
	Race_Mecha,
	NB_CREW_RACES,
};

enum ResourceType
{
	Resource_Gold,
	Resource_Fish,
	Resource_Mech,
	Resource_Fidelity,
	Resource_Days,
	NB_RESOURCES_TYPES,
	Resource_Crew,
	NB_RESOURCES_TYPES_TOTAL,
	Resource_SecretWreck,
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
	Room_PrisonCell,
	Room_Diving,
	Room_Sword,
	Room_Lifeboat,
	Room_Ammo,
	Room_Engine,
	NB_ROOM_TYPES,
};

enum CrewMemberType
{
	Crew_Pirate,
	Crew_Civilian,
	Crew_Slave,
	Crew_Undead,
	NB_CREW_TYPES,
	Crew_All,
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
	Action_Saving,
	Action_Entering,
	NB_INPUT_ACTIONS,
};

enum Color_Chart
{	
	Color_Red_Destroyed,
	Color_Red_CrewFidelity_Negative,
	Color_Blue_CrewFidelity_Positive,
	Color_Yellow_Target_Hovered,
	Color_Red_Target_Locked,
	Color_Red_Impossible,
	Color_DarkGrey_Background,
	Color_VeryDarkGrey_Background,
	Color_Orange_System,
	Color_Green_System,
	Color_Green_Lifebar,
	Color_Cyan_System,
	Color_Magenta_EngineCharged,
	Color_Blue_Water,
	Color_Blue_Flood,
	Color_Blue_Pierced,
	Color_Red_Prison,
	Color_Yellow_Prisoner,
	Color_Magenta_Crew,
	NB_COLOR_CHART,
};

enum ShipSystem
{
	System_None,
	System_Weapon,
	System_Navigation,
	System_Engine,
	NB_SHIP_SYSTEMS,
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
	vector<SFTextPop*> m_texts_pop;

	vector<vector<string> > m_choices_config;
	vector<vector<string> > m_rewards_config;

	//void CreateSFTextPop(const Font& font, unsigned int size, Uint32 style, const Color& color, string text_string, float distance_not_faded, float distance_faded, float total_pop_time, GameEntity* target, float offset_positionY);

	bool m_pause;

	string m_dico_ship_class[NB_SHIP_TYPES];
	string m_dico_room_types[NB_ROOM_TYPES];
	vector<string> m_dico_crew_names[2];
	vector<string> m_dico_crew_names_used[2];
	vector<string> m_dico_islands_names;
	vector<string> m_dico_islands_names_used;
	vector<string> m_dico_ships_names;
	vector<string> m_dico_ships_names_used;
	string m_dico_crew_types[NB_CREW_TYPES];
	string m_dico_crew_skills[NB_CREW_SKILLS];
	string m_dico_crew_races[NB_CREW_RACES];
	string m_dico_resources_textures[NB_RESOURCES_TYPES_TOTAL];
	string GetRandomCrewMemberName(int gender);
	string GetRandomIslandName();
	string GetRandomShipName();
	sf::Color m_dico_colors[NB_COLOR_CHART];

private:

	RenderWindow* m_window;
};

#endif // GAME_H_INCLUDED