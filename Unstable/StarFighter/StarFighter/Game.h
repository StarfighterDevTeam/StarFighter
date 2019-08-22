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
#include "SFPanel.h"
#include "SFTextPop.h"

class Ship;

enum SFX_Bank
{
	SFX_Laser,
	NBVAL_SFX_BANK,
};

enum Music_Bank
{
	Music_Main,
	NBVAL_MUSIC_BANK,
};

enum FontsStyle
{
	Font_Arial,//0
	Font_Terminator,//1
	NBVAL_FontsStyle,//2
};

enum StarSectorStatus
{
	Sector_Current,//player is inside this sector
	Sector_OnScreen,//player is not inside the sector, but it can be seen on screen
	Sector_Incoming,//the sector cannot be seen on screen, but is adjacent to a partial or full sector
	Sector_Far,//other sectors
};

struct StarSector
{
public:
	StarSector(){};
	StarSector(sf::Vector2i index, StarSectorStatus status){ m_index = index; m_status = status; }
	StarSectorStatus m_status;
	sf::Vector2i m_index;
};

using namespace sf;

struct Game
{
public:
	Game(RenderWindow* window);
	~Game();
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType layer, ColliderType type);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(Text* text);
	void addToFeedbacks(SFText* text);
	void addToFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(Text* text);
	void removeFromFeedbacks(SFPanel* panel);
	void CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY);

	void UpdateObjects(Time deltaTime);
	void UpdateScene(Time deltaTime);
	void drawScene();
	void CheckCollisions();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;
	sf::Vector2f m_scale_factor;

	Ship* m_playerShip;
	bool m_pause;
	bool m_window_has_focus;

	sf::View m_view;
	sf::Vector2f m_map_size;

	//Utiliary methods
	GameObject* GetClosestObjectTyped(const GameObject* ref_obj, ColliderType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	GameObject* GetClosestObjectTyped(const sf::Vector2f position, ColliderType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	std::vector<GameObject*> GetSceneGameObjectsTyped(ColliderType type);

	//Fonts
	sf::Font* m_font[NBVAL_FontsStyle];

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sfx_name);
	void SetSFXVolume(bool activate_sfx);

	sf::SoundBuffer m_soundBuffers[1];
	sf::Sound m_sounds[1];
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

	//Star Hunter
	bool AddToStarSectorsKnown(sf::Vector2i star_sector_index, StarSectorStatus status = Sector_Far);
	bool AddToStarSectorsKnown(StarSector sector);
	void UpdateSectorList(bool force_update = false);

	GameObject* m_background;
	StarSector m_current_star_sector;
	vector<sf::Vector2i> m_star_sectors_to_create;//all sectors that are close enough to need an updated

	//DEBUG
	GameObject* m_sector_debug_current;
	GameObject* m_sector_debug_onscreen;
	GameObject* m_sector_debug_incoming;
	GameObject* m_sector_debug_far;

private:
	void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);

	RenderWindow* m_window;

	vector<GameObject*> m_sceneGameObjects;
	map<sf::Vector2i, vector<GameObject*>, Vector2iComp > m_sceneGameObjectsStored;
	
	list<RectangleShape*> m_sceneFeedbackBars;
	list<Text*> m_sceneFeedbackTexts;
	list<SFPanel*> m_sceneFeedbackSFPanels;
	vector<SFText*> m_sceneFeedbackSFTexts;
	vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_ColliderType];
	vector<GameObject*> m_sceneGameObjectsTypedTemp[NBVAL_ColliderType];
	vector<SFText*> m_garbageTexts;

	//Star Hunter
	vector<StarSector> m_star_sectors_known;//all sectors encountered by the player
	vector<StarSector> m_star_sectors_managed;//all sectors that are close enough to need an updated
	
};

#endif // GAME_H_INCLUDED