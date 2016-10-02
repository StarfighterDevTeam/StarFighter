#ifndef SEGAME_H_INCLUDED
#define SEGAME_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include <SFML/Audio.hpp>
#include "SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "SEInputs.h"

#include "Glow.h"
#include "Stroboscopic.h"
#include "SFPanel.h"
#include "SFTextPop.h"
#include "Enums.h"

class SEShip;

using namespace sf;

struct SEGame;
extern SEGame* CurrentGame;

struct SEGame
{
public:
	virtual void init(RenderWindow* window);
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType layer, GameObjectType type);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(Text* text);
	void addToFeedbacks(SFText* text);
	void addToFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(Text* text);
	void removeFromFeedbacks(SFPanel* panel);
	void CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY);

	void updateScene(Time deltaTime);
	void drawScene();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;
	sf::Vector2f m_scale_factor;

	SEShip* m_playerShip;
	bool m_pause;

	sf::View m_view;
	sf::Vector2f m_map_size;

	//Utiliary methods
	GameObject* GetClosestObject(const GameObject* ref_obj, GameObjectType type_of_closest_object);
	GameObject* GetClosestObject(const sf::Vector2f position, GameObjectType type_of_closest_object);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

	//Fonts
	sf::Font* m_font[NBVAL_FontsStyle];

	//SFX
	virtual int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	void SetSFXVolume(bool activate_sfx);

	sf::SoundBuffer m_soundBuffers[1];
	sf::Sound m_soundsLaser[1];
	bool m_SFX_Activated;

	//Music
	sf::Music m_curMusic;
	bool m_Music_Activated;
	string m_next_music_name;
	Music_Bank m_curMusic_type;
	float m_music_fader;
	bool m_asking_music_fade_out;

	void SetMusicVolume(bool activate_music);
	void PlayMusic(Music_Bank music, string specific_filename = "");
	void ManageMusicTransitions(sf::Time deltaTime);

	//CSV data
	map<string, vector<string> > m_gameObjectsConfig;

private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);

	RenderWindow* m_window;
	std::list<RectangleShape*> m_sceneFeedbackBars;
	std::list<Text*> m_sceneFeedbackTexts;
	std::list<SFPanel*> m_sceneFeedbackSFPanels;
	std::vector<SFText*> m_sceneFeedbackSFTexts;
	std::vector<GameObject*> m_sceneGameObjects;
	std::vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
	std::vector<SFText*> m_garbageTexts;
};

#endif // SEGAME_H_INCLUDED