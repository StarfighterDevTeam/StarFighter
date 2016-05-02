#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFText.h"
#include "SFPanel.h"
#include "Stroboscopic.h"

#include <SFML/Audio.hpp>

class Ship;
class Scene;

enum SFX_Bank
{
	SFX_Fire,
	SFX_Kill,
	SFX_BigKill,
	SFX_MoneyLoot,
	SFX_Equip,
	SFX_Teleport,
	SFX_BuyOrSell,
	SFX_PortalOpening,
	SFX_EnteringPortal,
	//SFX_Heal,
	NBVAL_SFX_BANK,
};

enum Music_Bank
{
	Music_Hub,
	Music_Scene,
	Music_Boss,
	NBVAL_MUSIC_BANK,
};

using namespace sf;

enum TargetScan
{
	NO_TARGET_FOUND,//0
	TARGET_OUT_OF_RANGE,//1
	TARGET_IN_RANGE,//2
};

enum FontsStyle
{
	Font_Arial,//0
	Font_Terminator,//1
	NBVAL_FontsStyle,//2
};

struct Game
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType m_layer, GameObjectType type);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(SFPanel* panel);
	void addToFeedbacks(Text* text);
	void addToFeedbacks(SFText* text);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(Text* text);

	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2(Time deltaTime);
	void cleanGarbage();
	void collectGarbage();
	void garbageLayer(LayerType layer, bool only_offscene = false);
	void killGameObjectLayer(GameObjectType layer);
	void SetLayerSpeed(LayerType layer, sf::Vector2f speed);
	void SetLayerRotation(LayerType layer, float angle);

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;

	bool m_waiting_for_dialog_validation;
	bool m_waiting_for_scene_transition;
	sf::Clock m_end_dialog_clock;

	sf::Font* m_font[NBVAL_FontsStyle];

	bool isLastEnemyDead();
	int getHazard();
	void resetHazard(int hazard_overkill = 0);

	sf::Vector2f m_scale_factor;
	Directions m_direction;

	int m_hazard;
	int m_hazardSpawned;
	float m_BeastScoreBonus;

	Ship* m_playerShip;
	bool m_Pause;

	void SetPlayerShip(Ship* playerShip);
	float GetBeastScoreBonus(float playerShipBeastScore, float sceneBeastScore);

	TargetScan FoundNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	float GetAngleToNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	void WakeUpEnemiesWithName(string display_name);

	int GetPlayerStatsMultiplierForLevel(int level_);
	int GetEnemiesStatsMultiplierForLevel(int level_);
	int GetBonusStatsMultiplierToBeOnParForLevel(int level_);

	map<string, vector<string> > m_generalScenesConfig;
	map<string, vector<vector<string> > > m_sceneConfigs;
	map<string, vector<string> > m_enemiesConfig;

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);

	sf::SoundBuffer m_soundBuffers[NBVAL_SFX_BANK];
	map<SFX_Bank, sf::Sound> m_sounds;
	bool m_SFX_Activated;
	void SetSFXVolume(bool activate_sfx);

	//Music
	sf::Music m_curMusic;
	float m_music_fader;
	bool m_asking_music_fade_out;
	bool m_Music_Activated;
	void SetMusicVolume(bool activate_music);
	void PlayMusic(Music_Bank music, string specific_filename = "");
	void ManageMusicTransitions(sf::Time deltaTime);
	string m_next_music_name;
	Music_Bank m_curMusic_type;

	//utilitary methods
	static void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	static void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	static bool isVectorEmpty(vector <GameObject*>* vector);

private:
	void SetMovementFromPattern(Vector2f* move, float delta_t, int movepattern_type);
	
	Clock m_sceneChronometer;

	float m_movementClock;
	RenderWindow *m_window;

	std::list<RectangleShape*> m_sceneFeedbackBars;
	std::list<SFPanel*> m_sceneSFPanels;
	std::list<Text*> m_sceneFeedbackTexts;
	std::vector<SFText*> m_sceneFeedbackSFTexts;
	std::vector<GameObject*> m_sceneGameObjects;
	std::vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
	std::vector<SFText*> m_garbageTexts;
};

#endif // GAME_H_INCLUDED