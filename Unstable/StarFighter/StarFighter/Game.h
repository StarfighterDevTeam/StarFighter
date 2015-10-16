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
#include <SFML/Audio.hpp>

class Ship;

enum SFX_Bank
{
	SFX_Bounce,
	SFX_Goal,
	SFX_Tackle,
	SFX_Throw,
	SFX_Catch,
	SFX_Switch,
};

using namespace sf;

struct Game
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType m_layer, GameObjectType type);
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

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	sf::SoundBuffer soundBuffers[9];
	sf::Sound soundsBounce[5];
	sf::Sound soundsTackle;
	sf::Sound soundsGoal;
	sf::Sound soundsThrow;
	sf::Sound soundsCatch;
	sf::Sound soundsSwitch;

	//TRON SPECIFIC
	float GetAngleOfCollision(const GameObject* ref_obj, const GameObject* aimed_obj);

private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);
	RenderWindow *window;
	std::list<RectangleShape*> sceneFeedbackBars;
	std::list<Text*> sceneFeedbackTexts;
	std::vector<GameObject*> sceneGameObjects;
	std::vector<GameObject*> sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> garbage;
};

#endif // GAME_H_INCLUDED