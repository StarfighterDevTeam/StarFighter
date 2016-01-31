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
#include "SFText.h"

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
	ModuleType_Relay,
	ModuleType_Accelerator,
	ModuleType_Amplifier,
	ModuleType_Shield,
	ModuleType_Condensator,
	ModuleType_Generator,
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
	void addToFeedbacks(SFRectangle* feedback, LayerType layer);
	void addToFeedbacks(SFText* text);
	void addToFeedbacks(SFGauge* gauge);

	void updateScene(Time deltaTime);
	void drawScene();
	void drawHud();
	void Draw(sf::RenderTexture& offscreen);
	void colisionChecksV2(Time deltaTime);
	void cleanGarbage();
	void collectGarbage();
	sf::RenderTexture mainScreen;
	sf::RenderTexture hudScreen;
	sf::RenderTexture hudScreen_SplitScreen;
	sf::Vector2i screen_size;
	float hyperspeedMultiplier;
	float vspeed;
	sf::Vector2f scale_factor;

	Ship* playerShips[NBVAL_PlayerTeams];

	sf::Vector2u m_startingGeneratorGridIndex[NBVAL_PlayerTeams];
	void ActivateWarningFeedback(PlayerTeams team);

	sf::View view[1+USE_SPLIT_SCREEN];
	sf::Vector2f map_size;

	//utilitary methods
	GameObject* GetClosestObject(const GameObject* ref_obj, GameObjectType type_of_closest_object);
	GameObject* GetClosestObject(const sf::Vector2f position, GameObjectType type_of_closest_object);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);
	std::vector<GameObject*> GetSceneGameObjectsLayered(LayerType layer);
	std::vector<SFRectangle*> GetSceneSFRectanglesLayered(LayerType layer);

	//Fonts
	sf::Font* font;
	sf::Font* font2;
	sf::Font* m_fonts[NBVAL_FontReferences];

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
	GameObject* m_flux_source_grid[GRID_WIDTH][GRID_HEIGHT];
	vector<SFRectangle*> sceneFreeTilesFeedbacks;
	vector<GameObject*> sceneModuleObjects;
	void WipeAllObjectsWithGridIndex(GameObjectType type, sf::Vector2u grid_index);

	//Prototypes
	Fluxor* m_fluxors[NBVAL_FluxorType];
	Module* m_modules[NBVAL_ModuleType];
	GameObject* m_module_arrows;
	GameObject* m_flux_source;
	GameObject* m_player_warnings;
	sf::Color m_team_colors[NBVAL_PlayerTeams];
	GameObject* m_team_markers[NBVAL_PlayerTeams];
	SFGauge* m_flux_gauges[NBVAL_GaugeStyles];
	GameObject* m_module_HUD;
	vector<SFRectangle*> m_HUD_productions_mask[1 + (USE_SPLIT_SCREEN || SHARED_VIEW)];

	bool isCellFree(sf::Vector2f position);
	static sf::Vector2u GetGridIndex(sf::Vector2f position);
	static sf::Vector2f GridToPosition(sf::Vector2u grid_index);
	vector<vector<int>> m_teams_vs_alliance_map;
	TeamAlliances GetTeamAlliance(PlayerTeams team);
	
private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddGameObjectToVector(SFRectangle* pRectangleShape, vector<SFRectangle*>* vector);
	void AddGameObjectToVector(SFText* pSFText, vector<SFText*>* vector);
	void AddGameObjectToVector(SFGauge* pSFGauge, vector<SFGauge*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);
	RenderWindow *window;
	std::vector<SFRectangle*> sceneFeedbackBars[NBVAL_Layer];
	std::vector<SFText*> sceneFeedbackSFTexts;
	std::vector<SFGauge*> sceneFeedbackSFGauge;
	std::vector<GameObject*> sceneGameObjects;
	std::vector<GameObject*> sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> garbage;
	std::vector<SFText*> garbageTexts;
	std::vector<SFRectangle*> garbageRectangleShapes;
	std::vector<SFGauge*> garbageGauges;
};

#endif // GAME_H_INCLUDED