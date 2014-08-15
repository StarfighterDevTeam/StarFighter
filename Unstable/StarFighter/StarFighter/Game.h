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

#define LOOT_SPEED_Y		100.f

class Ship;

using namespace sf;

enum LayerType {
	BackgroundLayer,
	ExplosionLayer,
	FriendlyFireLayer,
	AuraLayer,
	BotLayer,
	FakeShipLayer,
	PlayerShipLayer,
	EnemyFireLayer,
	EnemyObjectLayer,
	NBVAL_Layer
};

struct Game 
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(Independant *object, int layer, IndependantType type);
	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();
	void garbageLayer (LayerType m_layer);

	bool isLastEnemyDead();
	int getHazard();
	void resetHazard(int hazard_overkill = 0);
	sf::Vector2f scale_factor;
	Directions direction;

	Ship* playerShip;
	void SetPlayerShip(Ship* m_playerShip);
	void GetBeastScoreBonus (float m_playerShipBeastScore, float m_sceneBeastScore);

private:
	void SetMovementFromPattern(Vector2f* move, float delta_t, int movepattern_type);
	Clock sceneChronometer;
	float movementClock;
	int hazard;
	float BeastScoreBonus;
	RenderWindow *window;
	std::list<Independant*> sceneIndependants;
	std::list<Independant*>* sceneIndependantsLayered[NBVAL_Layer];
	std::list<Independant*>* sceneIndependantsTyped[NBVAL_Independant];
	std::vector<Independant*> garbage;
};

#endif // GAME_H_INCLUDED