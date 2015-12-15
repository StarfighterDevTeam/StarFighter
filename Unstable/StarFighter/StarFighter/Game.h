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
#include "Glow.h"
#include "Stroboscopic.h"

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

enum ScriptedGameRules
{
	ClassicMatchGamesRules,
	SoloTraining,
	CarryToAbleInputs,
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
	void CleanAllGameObjects();
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

	void updateScene(Time deltaTime);
	void updateSceneAfterCollisions(Time deltaTime);
	void drawScene();
	void colisionChecksV2(Time deltaTime);
	void cleanGarbage();
	void collectGarbage();
	sf::RenderTexture mainScreen;
	sf::Vector2f screen_size;
	sf::Vector2f map_size;
	float hyperspeedMultiplier;
	float vspeed;
	sf::Vector2f scale_factor;

	sf::View view;

	Ship* playerShip;
	GameObject* scriptLauncher;
	ScriptedGameRules cur_GameRules;
	void Goal(Teams team);

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
	bool m_SFX_Activated;
	void SetSFXVolume(bool activate_sfx);

	//Music
	sf::Music m_curMusic;
	bool m_Music_Activated;
	void SetMusicVolume(bool activate_music);

	//TRON SPECIFIC
	float GetAngleOfCollision(const GameObject* ref_obj, const GameObject* aimed_obj);
	GameObject* GetClosestObject(const GameObject* ref_obj, GameObjectType type_of_closest_object);
	unsigned int score_blue_team;
	unsigned int score_red_team;
	unsigned int score_to_win;
	sf::Font* font;
	sf::Font* font2;
	sf::Clock score_again_blue_clock;
	sf::Clock score_again_red_clock;
	bool m_goal_happened;
	sf::Clock m_after_goal_clock;
	sf::Clock m_map_clock;

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