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
	void addToScene(GameObject *object, LayerType layer, GameObjectType type);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(Text* text);
	void addToFeedbacks(SFText* text);
	void addToFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(Text* text);
	void removeFromFeedbacks(SFPanel* panel);
	void CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY);
	void changeObjectTypeAndLayer(GameObject *object, LayerType new_layer, GameObjectType new_type);

	void updateScene(Time deltaTime);
	void drawScene();
	void collision_checks();
	void cleanGarbage();
	void collectGarbage();

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

	//Utiliary methods
	GameObject* GetClosestObjectTyped(const GameObject* ref_obj, GameObjectType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	GameObject* GetClosestObjectTyped(const sf::Vector2f position, GameObjectType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

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
	void AddCircleObject(CircleObject* object);
	void AddLineObject(LineObject* object);

	void WaveCollisionCheck(CircleObject* object_wave, CircleObject* object_entity);
	void BallisticCollisionCheck(CircleObject* object_ballistic, CircleObject* object_entity);

	vector<WaveBounce*> m_wave_bounces;
	vector<WaveReception*> m_wave_receptions;

	sf::RectangleShape m_rectangular_selection;
	bool m_display_rectangular_selection;
	bool m_released_rectangular_selection;

	void UpdateRectangularSelection();
	vector<L16Entity*> m_hovered_entities;
	vector<L16Entity*> m_selected_entities;

	vector<Node*> m_nodes;
	vector<Link*> m_links;
	vector<Wing*> m_wings;
	vector<Ballistic*> m_ballistics;
	vector<Wave*> m_waves;
	
	vector<CircleObject*> m_sceneCircleObjects[NB_ALLIANCE_TYPES][NB_CIRCLE_TYPES];
	vector<CircleObject*> m_new_sceneCircleObjects;
	vector<LineObject*> m_sceneLineObjects[NB_ALLIANCE_TYPES];

private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddGameObjectVectorToVector(vector<GameObject*> vector_slave, vector<GameObject*>* vector_master);
	void TransferGameObjectLayeredTempToSceneObjectsLayered(LayerType layer);
	void TransferGameObjectTypedTempToSceneObjectsTyped(GameObjectType collider_type);
	void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);

	RenderWindow* m_window;
	std::list<RectangleShape*> m_sceneFeedbackBars;
	std::list<Text*> m_sceneFeedbackTexts;
	std::list<SFPanel*> m_sceneFeedbackSFPanels;
	std::vector<SFText*> m_sceneFeedbackSFTexts;
	std::vector<GameObject*> m_sceneGameObjects;
	std::vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> m_sceneGameObjectsLayeredTemp[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTypedTemp[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
	std::vector<SFText*> m_garbageTexts;
};

#endif // GAME_H_INCLUDED