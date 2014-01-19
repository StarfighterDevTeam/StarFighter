#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "Independant.h"
#include <list>
#include <vector>

using namespace sf;

enum LayerType {
	BackgroundLayer,
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
	void addToScene(Independant *object, int layer = 4);
	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecks();
	void cleanGarbage();
	void collectGarbage();

	//TODO : remove from scene 
	//TODO : scene position checker <=> remove an item which is no more on the scene
private:
	RenderWindow *window;
	std::list<Independant*> sceneIndependants;
	std::list<Independant*>* sceneIndependantsLayered[NBVAL_Layer];
	std::vector<Independant*> garbage;
};

#endif // GAME_H_INCLUDED