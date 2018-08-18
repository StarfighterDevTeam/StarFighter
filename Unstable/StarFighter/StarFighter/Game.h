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

class Ship;

enum SFX_Bank
{
	SFX_Parry,
	SFX_Dash,
	SFX_Melee,
	SFX_GruntPlayer,
	SFX_GruntEnemy,
	SFX_Summon,
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

using namespace sf;

struct Game
{
public:
	void init(RenderWindow* window);
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
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_timescale;
	float m_vspeed;
	sf::Vector2f m_scale_factor;

	Ship* m_playerShip;
	bool m_pause;
	bool m_window_has_focus;

	sf::View m_view;
	sf::Vector2f m_map_size;

	int GetEnemyRatings();

	sf::Shader m_shader;

	//Utiliary methods
	GameObject* GetClosestObjectTyped(const GameObject* ref_obj, GameObjectType type_of_closest_object);
	GameObject* GetClosestObjectTyped(const sf::Vector2f ref_position, GameObjectType type_of_closest_object);
	GameObject* GetClosestObjectTypedIncoming(const GameObject* ref_obj, GameObjectType type_of_closest_object, float delta_angle_max);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

	//Fonts
	sf::Font* m_font[NBVAL_FontsStyle];

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	void SetSFXVolume(bool activate_sfx);

	sf::SoundBuffer m_soundBuffers[6];
	sf::Sound m_sounds[6];
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

	GameObject* getDashTarget(float dash_radius);
	GameObject* getDashTargetWithBlacklist(float dash_radius, vector<GameObject*> enemies_tagged);

	//CSV data
	map<string, vector<string> > m_gameObjectsConfig;

	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddGameObjectVectorToVector(vector<GameObject*> vector_slave, vector<GameObject*>* vector_master);
	void TransferGameObjectLayeredTempToSceneObjectsLayered(LayerType layer);
	void TransferGameObjectTypedTempToSceneObjectsTyped(GameObjectType collider_type);
	void GameObjectVectorToVector(vector<GameObject*> vector_slave, vector<GameObject*>* vector_master, LayerType layer_to_copy = NBVAL_Layer, GameObjectType collider_type_to_copy = NBVAL_GameObject);
	void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);

private:

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