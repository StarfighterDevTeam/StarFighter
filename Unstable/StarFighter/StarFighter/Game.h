#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#define _USE_MATH_DEFINES
#include <math.h>

#include "SFPanel.h"

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
	void addToScene(GameObject* object, bool during_update);
	void addToRectangles(SFRectangle& rectangle);
	void addToTexts(SFText& text);
	void addToPanels(SFPanel* panel);
	void addToTextPops(SFTextPop* textpop);
	void addToGuidedMissiles(GameObject* missile);

	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2(Time deltaTime);
	//void cleanGarbage();
	//void collectGarbage();
	void garbageLayer(LayerType layer, bool only_offscene, bool delete_DontGarbageMe);
	void killGameObjectType(GameObjectType type);
	void damageGameObjectType(GameObjectType type, int damage);
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
	static EquipmentQuality GetItemQualityClass(float quality);

	GameObject* GetNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	float GetAngleToNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	void WakeUpEnemiesWithName(string display_name);

	int GetPlayerStatsMultiplierForLevel(int level_);
	int GetEnemiesStatsMultiplierForLevel(int level_);
	int GetBonusStatsMultiplierToBeOnParForLevel(int level_);

	map<string, vector<string> > m_generalScenesConfig;
	map<string, vector<vector<string> > > m_sceneConfigs;
	map<string, vector<string> > m_enemiesConfig;
	map<string, vector<string> > m_FXConfig;
	map<string, vector<string> > m_upgradesConfig;

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

private:
	void SetMovementFromPattern(Vector2f* move, float delta_t, int movepattern_type);
	
	Clock m_sceneChronometer;

	float m_movementClock;
	RenderWindow *m_window;

	vector<SFRectangle*> m_sceneSFRectangles;
	vector<SFText*> m_sceneSFTexts;
	vector<SFPanel*> m_sceneSFPanels;
	vector<SFTextPop*> m_sceneSFTextPops;

	vector<GameObject*> m_sceneGameObjects;
	vector<GameObject*> m_sceneGameObjectsCreated;
	vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	vector<GameObject*> m_sceneGuidedMissiles;
};

#endif // GAME_H_INCLUDED