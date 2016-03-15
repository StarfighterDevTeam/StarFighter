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
#include "SFPanel.h"

class Ship;
class Scene;

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
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(Text* text);

	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();
	void garbageLayer(LayerType m_layer, bool only_offscene = false);
	void killGameObjectLayer(GameObjectType m_layer);
	void SetLayerRotation(LayerType m_layer, float angle);

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;

	sf::Font* m_font[NBVAL_FontsStyle];

	bool isLastEnemyDead();
	int getHazard();
	void resetHazard(int hazard_overkill = 0);

	sf::Vector2f m_scale_factor;
	Directions m_direction;

	int m_hazard;
	int m_hazardSpawned;
	float m_BeastScoreBonus;
	Scene* m_currentScene;

	Ship* playerShip;
	void SetPlayerShip(Ship* m_playerShip);
	void GetBeastScoreBonus(float m_playerShipBeastScore, float m_sceneBeastScore);

	TargetScan FoundNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	float GetAngleToNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	void WakeUpEnemiesWithName(string m_display_name);

	int GetPlayerStatsMultiplierForLevel(int level_);
	int GetEnemiesStatsMultiplierForLevel(int level_);
	int GetBonusStatsMultiplierToBeOnParForLevel(int level_);

private:
	void SetMovementFromPattern(Vector2f* move, float delta_t, int movepattern_type);
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);
	Clock m_sceneChronometer;

	float m_movementClock;
	RenderWindow *m_window;

	std::list<RectangleShape*> m_sceneFeedbackBars;
	std::list<SFPanel*> m_sceneSFPanels;
	std::list<Text*> m_sceneFeedbackTexts;
	std::vector<GameObject*> m_sceneGameObjects;
	std::vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
};

#endif // GAME_H_INCLUDED