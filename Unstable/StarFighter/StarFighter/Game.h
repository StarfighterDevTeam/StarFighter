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
	void addToScene(Independant *object);
	void updateScene(Time deltaTime);
	void drawScene();

	
	//TODO : remove from scene 
	//TODO : scene position checker <=> remove an item which is no more on the scene
private:
	RenderWindow *window;
	std::list<Independant*> sceneIndependants;
};

#endif // GAME_H_INCLUDED