#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "FX.h"
#include "Loot.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include "Includes/PixelPerfectCollision.hpp"
#include "Includes/SimpleCollision.hpp"

using namespace sf;

enum LayerType {
	BackgroundLayer,
	ExplosionLayer,
	PlayerShipLayer,
	FriendlyFireLayer,
	EnemyFireLayer,
	EnemyObjectLayer,
	NBVAL_Layer
};

struct Game 
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	//PlayerHud* ship_hud;
	void addToScene(Independant *object, int layer, IndependantType type);
	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecks();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();

	//TODO : remove from scene 
	//TODO : scene position checker <=> remove an item which is no more on the scene
private:
	RenderWindow *window;
	std::list<Independant*> sceneIndependants;
	std::list<Independant*>* sceneIndependantsLayered[NBVAL_Layer];
	std::list<Independant*>* sceneIndependantsTyped[NBVAL_Independant];
	std::vector<Independant*> garbage;
};

#endif // GAME_H_INCLUDED