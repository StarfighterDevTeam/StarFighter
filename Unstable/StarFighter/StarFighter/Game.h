#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Independant.h"
#include <list>

using namespace sf;

struct Game 
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(Independant *object, int layer = 4);
	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecks(Time deltaTime);

	//TODO : remove from scene 
	//TODO : scene position checker <=> remove an item which is no more on the scene
private:
	RenderWindow *window;
	std::list<Independant*> sceneIndependants;
	std::list<Independant*>* sceneIndependantsLayered[8];
};

#endif // GAME_H_INCLUDED