#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "Independant.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include "Includes/PixelPerfectCollision.hpp"
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Hud.h"

#define LOOT_SPEED_Y		100.f

class Ship;

using namespace sf;

enum TargetScan
{
	NO_TARGET_FOUND,//0
	TARGET_OUT_OF_RANGE,//1
	TARGET_IN_RANGE,//2
};

struct Game 
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(Independant *object, LayerType m_layer, IndependantType type);
	void updateScene(Time deltaTime);
	void updateHud(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, sf::Time deltaTime, bool hub);
	void drawScene();
	void drawHud();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();
	void garbageLayer (LayerType m_layer, bool only_offscene=false);
	void SetLayerRotation(LayerType m_layer, float angle);
	sf::RenderTexture mainScreen;
	sf::RenderTexture hubScreen;
	PlayerHud hud;
	sf::Vector2i screen_size;
	float hyperspeedMultiplier;

	bool InsertObjectInShipGrid(Independant& object, int index = 0);
	bool InsertObjectInEquipmentGrid(Independant& object);
	bool SwapEquipObjectInShipGrid(Independant& object, int index_ship, int index_equipment);
	Independant* getHudFocusedItem();
	sf::Vector2i getHudFocusedGridAndIndex();
	sf::Vector2i getHudFocusedIndexWithinGrid(HudGridsIndex grid_);

	bool isLastEnemyDead();
	int getHazard();
	void resetHazard(int hazard_overkill = 0);
	sf::Vector2f scale_factor;
	Directions direction;

	int hazard;
	int hazardSpawned;

	Ship* playerShip;
	void SetPlayerShip(Ship* m_playerShip);
	void GetBeastScoreBonus (float m_playerShipBeastScore, float m_sceneBeastScore);
	TargetScan FoundNearestIndependant(IndependantType type, sf::Vector2f ref_position, float range = 0);
	float GetAngleToNearestIndependant(IndependantType type, sf::Vector2f ref_position, float range=0);
	void WakeUpEnemiesWithName(string m_display_name);

private:
	void SetMovementFromPattern(Vector2f* move, float delta_t, int movepattern_type);
	Clock sceneChronometer;
	float movementClock;
	float BeastScoreBonus;
	RenderWindow *window;
	std::list<Independant*> sceneIndependants;
	std::list<Independant*> sceneIndependantsLayered[NBVAL_Layer];
	std::list<Independant*> sceneIndependantsTyped[NBVAL_Independant];
	std::list<Independant*> garbage;
};

#endif // GAME_H_INCLUDED