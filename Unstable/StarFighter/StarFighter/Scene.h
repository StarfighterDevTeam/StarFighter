#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include "FileLoader.h"

using namespace std;

enum ScenesData
{
	SCENE_NAME,//0
	SCENE_FILENAME,//1
	SCENE_DISPLAYNAME,//2
	SCENE_LINK_UP,//3
	SCENE_LINK_DOWN,//4
	SCENE_LINK_RIGHT,//5
	SCENE_LINK_LEFT,//6
	SCENE_BACKGROUND_FILENAME,//7
	SCENE_BACKGROUND_WIDTH,//8
	SCENE_BACKGROUND_HEIGHT,//9
	SCENE_BACKGROUND_VSPEED,//10
	SCENE_IS_HUB,//11
	SCENE_SIZE_IN_STELLAR_MAP,//12
	SCENE_MUSIC_NAME,//13
};

enum ScenesDialogData
{
	SCENE_DIALOG,//0
	SCENE_DIALOG_NAME,//1
	SCENE_DIALOG_TIME,//2
};

enum SceneScriptData
{
	SceneScript_PortalOpenDuringBoss,
	NBVAL_SceneScripts,
};

class Scene
{
public:
	//Scene(string name, ShipConfig* shipConf);
	Scene(string name);//, int hazard_level, bool reverse_scene=false, bool first_scene=false);
	//Scene(string name);
	~Scene();
	void GenerateEnemies(Time deltaTime);
	void SpawnBoss();
	void PlayTitleFeedback();

	string m_name;
	float m_vspeed;
	bool m_hazardbreak_has_occurred;

	int m_score_destruction;
	int m_score_graze;
	int m_score_total;

	static float getSceneBeastScore(int for_hazard_level);

	Background* m_bg;
	bool m_generating_enemies;
	bool m_generating_boss;
	sf::Clock m_spawnClock;
	int m_level;
	bool m_scripts[NBVAL_SceneScripts];
	//map<float, vector<Dialog*> > m_dialogs;
	vector<pair<float, vector<Dialog*> > > m_dialogs;
	sf::Clock m_scene_clock;
	string m_scene_music;
	bool m_is_hub;

	int getSceneHazardLevelUnlockedValue();
	void setSceneHazardLevelUnlockedValue(int hazard_unlocked_value);
	int getSceneHazardLevelValue();

	std::string m_links[NO_DIRECTION];

	vector<EnemyGenerator*> m_sceneEnemyGenerators;

	void GenerateEnemiesv2(Time deltaTime);
	void SpawnEnemy(int enemy_class);
	void CollateralSpawnCost(float collateral_cost, float collateral_multiplier = 0, int below_enemy_class = (int)NBVAL_EnemyClass);
	void ApplyHazardLevelModifiers(int hazard_level, Enemy& enemy);

	void HazardBreak();

	bool m_canHazardBreak;
	vector <string> m_scenesLinkedToUpdate;

	bool CheckHazardBreakConditions();
	void DisplayDestructions(bool hazard_break = false);//OLD: to remove
	void DisplayScore(bool hazard_break = false);
	bool IsLastSceneBeforeHub();

private:
	vector<EnemyBase*> m_boss_list;
	vector<EnemyBase*> m_enemies_ranked_by_class[NBVAL_EnemyClass];
	int m_total_class_probability[NBVAL_EnemyClass];
	int m_hazard_level;
	int m_hazard_level_unlocked;
};

#endif // STARFIGHTER_H_INCLUDED