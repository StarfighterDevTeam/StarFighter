#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include <SFML/Audio.hpp>
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "InputGuy.h"

#include "Glow.h"
#include "Stroboscopic.h"
#include "SFPanel.h"
#include "SFTextPop.h"
#include "InputGuy.h"

#include "CircleObject.h"
#include "LineObject.h"

class Ship;
class L16Entity;
class Node;
class Wing;
class Link;
class Ballistic;
class Wave;

enum PlayerActions
{
	Action_Idle,
	Action_Firing,
	Action_Muting,
	Action_Pausing,
	NBVAL_PlayerActions,
};

enum PlayerInputStates
{
	Input_Release,//0
	Input_Tap,//1
	Input_Hold,//2
};

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
	Mouse_LeftClickHold,
	Mouse_RightClick,
	Mouse_RightClickHold,
};

using namespace sf;

struct WaveBounce
{
public:
	WaveBounce(sf::Vector2f position, float direction, float radius, Wave* wave, L16Entity* bounced_entity)
	{
		m_position = position; m_direction = direction; m_radius = radius; m_wave = wave; m_bounced_entity = bounced_entity;
	}

	sf::Vector2f m_position;
	float m_direction;
	float m_radius;
	Wave* m_wave;
	L16Entity* m_bounced_entity;
};

struct WaveReception
{
public:
	WaveReception(Wave* wave, L16Entity* entity)
	{
		m_wave = wave; m_entity = entity;
	}

	Wave* m_wave;
	L16Entity* m_entity;
};

struct Game
{
public:
	Game(RenderWindow* window);
	~Game();
	RenderWindow* getMainWindow();

	void updateScene(Time deltaTime);
	void drawScene();
	void CheckCollisions();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;
	sf::Vector2f m_scale_factor;

	Ship* m_playerShip;
	bool m_pause;
	bool m_window_has_focus;

	sf::Vector2f m_mouse_pos;
	MouseAction m_mouse_click;
	void GetMouseInputs(sf::Time deltaTime);

	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];
	void UpdateInputStates();
	void UpdateInputState(bool input_guy_boolean, PlayerActions action);

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

	//CSV data
	map<string, vector<string> > m_gameObjectsConfig;

	//Liaison 16
	GameObject* m_background;

	void AddCircleObject(CircleObject* object);
	void AddLineObject(LineObject* object);

	void WaveCollisionCheck(CircleObject* object_wave, CircleObject* object_entity);
	void BallisticCollisionCheck(CircleObject* object_ballistic, CircleObject* object_entity);
	L16Entity* GetClosestL16Target(sf::Vector2f position, float heading, CircleType collision_domain, AllianceType alliance, float dist_max, float angle_delta_max);

	vector<WaveBounce*> m_wave_bounces;
	vector<WaveReception*> m_wave_receptions;

	sf::RectangleShape m_rectangular_selection;
	bool m_display_rectangular_selection;
	bool m_released_rectangular_selection;

	void UpdateRectangularSelection();
	vector<L16Entity*> m_hovered_entities;
	vector<L16Entity*> m_selected_entities;

	vector<L16Entity*> m_L16_entities;
	vector<Wave*> m_waves;
	vector<Link*> m_links;
	
	vector<CircleObject*> m_sceneCircleObjects[NB_ALLIANCE_TYPES][NB_CIRCLE_TYPES];
	vector<CircleObject*> m_new_sceneCircleObjects;
	vector<LineObject*> m_sceneLineObjects[NB_ALLIANCE_TYPES];

	//AI commands
	float m_AI_direction_x;
	float m_AI_fire;

	//AI metrics
	float m_clock;
	int m_shots_fired;

private:
	RenderWindow* m_window;
};

#endif // GAME_H_INCLUDED