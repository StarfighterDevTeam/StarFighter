#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include <SFML/Audio.hpp>
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "InputGuy.h"

#include "Glow.h"
#include "Stroboscopic.h"

class Ship;
class Fluxor;
class Module;
class Glow;

enum FluxorType
{
	FluxorType_Blue,
	FluxorType_Green,
	FluxorType_Red,
	FluxorType_Purple,
	FluxorType_Black,
	NBVAL_FluxorType,
};

enum ModuleType
{
	ModuleType_Generator,
	ModuleType_Accelerator,
	ModuleType_Relay,
	ModuleType_Condensator,
	ModuleType_Amplifier,
	ModuleType_Factory,
	ModuleType_Factory_Up,
	ModuleType_Turret,
	ModuleType_Barrier,
	ModuleType_Armory,
	ModuleType_Battery,
	ModuleType_Shield,
	NBVAL_ModuleType,
};

enum SFX_Bank
{
	SFX_Laser,
};

using namespace sf;

struct Game
{
public:
	void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType layer, GameObjectType type);
	void addToFeedbacks(SFRectangle* feedback);
	void addToFeedbacks(SFText* text);

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

	Ship* playerShips[NBVAL_PlayerTeams];

	sf::View view;
	sf::View viewP1;
	sf::View viewP2;
	sf::Vector2f map_size;

	//utilitary methods
	GameObject* GetClosestObject(const GameObject* ref_obj, GameObjectType type_of_closest_object);
	GameObject* GetClosestObject(const sf::Vector2f position, GameObjectType type_of_closest_object);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

	//Fonts
	sf::Font* font;
	sf::Font* font2;

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	sf::SoundBuffer soundBuffers[1];
	sf::Sound soundsLaser[1];
	bool m_SFX_Activated;
	void SetSFXVolume(bool activate_sfx);

	//Music
	sf::Music m_curMusic;
	bool m_Music_Activated;
	void SetMusicVolume(bool activate_music);

	//FLUX SPECIFIC
	GameObject* m_module_grid[GRID_WIDTH][GRID_HEIGHT];
	Fluxor* m_fluxors[NBVAL_FluxorType];
	sf::Color m_team_colors[NBVAL_PlayerTeams];
	bool isCellFree(sf::Vector2f position);
	static sf::Vector2u GetGridIndex(sf::Vector2f position);
	static sf::Vector2f GridToPosition(sf::Vector2u grid_index);
	void ResolveProductionBufferList();
	vector<vector<int>> m_teams_vs_alliance_map;
	int GetTeamAlliance(PlayerTeams team);

private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddGameObjectToVector(SFRectangle* pRectangleShape, vector<SFRectangle*>* vector);
	void AddGameObjectToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);
	RenderWindow *window;
	std::vector<SFRectangle*> sceneFeedbackBars;
	std::vector<SFText*> sceneFeedbackSFTexts;
	std::vector<GameObject*> sceneGameObjects;
	std::vector<GameObject*> sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> garbage;
	std::vector<SFText*> garbageTexts;
	std::vector<SFRectangle*> garbageRectangleShapes;
};

#endif // GAME_H_INCLUDED