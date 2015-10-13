#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "Independant.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "InputGuy.h"

class Ship;

using namespace sf;

struct Game
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(Independant *object, LayerType m_layer, IndependantType type);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(Text* text);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(Text* text);

	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();
	sf::RenderTexture mainScreen;
	sf::Vector2i screen_size;
	float hyperspeedMultiplier;
	float vspeed;
	sf::Vector2f scale_factor;

	Ship* playerShip;
	void SetPlayerShip(Ship* m_playerShip);

private:
	void AddIndependantToVector(Independant* pIndependant, vector<Independant*>* vector);
	bool isVectorEmpty(vector <Independant*>* vector);
	RenderWindow *window;
	std::list<RectangleShape*> sceneFeedbackBars;
	std::list<Text*> sceneFeedbackTexts;
	std::vector<Independant*> sceneIndependants;
	std::vector<Independant*> sceneIndependantsLayered[NBVAL_Layer];
	std::vector<Independant*> sceneIndependantsTyped[NBVAL_Independant];
	std::vector<Independant*> garbage;
};

#endif // GAME_H_INCLUDED