#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Globals.h"
#include <list>
#include <vector>
#include "TextUtils.h"
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include "Includes/SimpleCollision.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

#include "InputGuy.h"

#include "Glow.h"
#include "Stroboscopic.h"
#include "SFPanel.h"
#include "SFTextPop.h"
#include "CircleDisplay.h"

class Ship;
class Planet;
class AsteroidField;

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

enum NetworkStatus
{
	Network_Disconnected,
	Network_Connected,
	Network_Communicating,
};

enum NetworkPacketType
{
	Packet_PlayerShipUpdate,
	Packet_AmmoCreation,
	Packet_AmmoImpact,
	Packet_AsteroidCreation,
	Packet_AsteroidFieldCreation,
	Packet_PlanetCreation,
};

struct StarSector
{
public:
	//StarSector(){ m_hazard_level = 1; };
	StarSector(sf::Vector2i index){ m_index = index;}
	StarSector(sf::Vector2i index, int id) : StarSector(index) {m_id = id;}

	sf::Vector2i m_index;
	int m_hazard_level = 1;
	int m_id = -1;
};

struct Game
{
public:
	Game(RenderWindow* window);
	~Game();
	RenderWindow* getMainWindow();
	void addToScene(GameObject *object, LayerType layer, ColliderType type, bool created_by_updated_object);
	void addToFeedbacks(RectangleShape* feedback);
	void addToFeedbacks(Text* text);
	void addToFeedbacks(SFText* text);
	void addToFeedbacks(SFPanel* panel);
	void removeFromFeedbacks(RectangleShape* feedback);
	void removeFromFeedbacks(Text* text);
	void removeFromFeedbacks(SFPanel* panel);
	void CreateSFTextPop(string text, FontsStyle font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* follow_target, float offset_positionY);

	void ClearGarbage();
	void UpdateObjects(Time deltaTime);
	void UpdateScene(Time deltaTime);
	void drawScene();
	void CollisionChecks();

	sf::RenderTexture m_mainScreen;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;
	sf::Vector2f m_scale_factor;
	float m_zoom;

	GameObject* m_playerShip;
	vector<GameObject*> m_onlineShips;
	bool m_pause;
	bool m_window_has_focus;

	sf::Vector2f m_mouse_pos;

	sf::View m_view;
	sf::Vector2f m_map_size;

	//Fonts
	sf::Font* m_font[NBVAL_FontsStyle];

	//SFX
	int LoadSFX();
	void PlaySFX(SFX_Bank sFX_name);
	void SetSFXVolume(bool activate_sFX);

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
	bool AddToStarSectorsKnown(sf::Vector2i star_sector_index);
	void UpdateSectorList(bool force_update);
	int GetSectorId(sf::Vector2i index);
	void SetStarSectorIndex(GameObject* object, sf::Vector2i sector_index);
	void DebugDrawGameObjectsStats();
	void DebugDrawSectors();
	bool AreColliding(GameObject* objectA, GameObject* objectB, bool include_shield);
	static sf::IntRect FToIRect(const sf::FloatRect& f);
	bool StoreObjectIfNecessary(GameObject* object);
	SpatialObject* GetTargetableEnemyShip(GameObject* const shooter, float dist_max, float angle_delta_max);
	void SetSectorsNbSectorsManaged();

	vector<GameObject*> m_sceneGameObjects;
	vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_ColliderType];
	vector<GameObject*> m_temp_sceneGameObjects;
	map<int, vector<GameObject*> > m_sceneGameObjectsStored;
	vector<GameObject*> m_garbageObjects;

	int m_nb_sectors_managed_x;
	int m_nb_sectors_managed_y;

	GameObject* m_background;
	vector<CircleDisplay *> m_gravity_circles;
	vector<CircleDisplay *> m_shield_circles;

	vector<StarSector> m_sectorsKnown;//all sectors encountered by the player
	vector<sf::Vector2i> m_sectorsManaged;//all sectors that are close enough to need an updated
	vector<sf::Vector2i> m_sectorsToCreate;//all sectors that have just been created and need content creation
	vector<sf::Vector2i> m_sectorsToAddStar;//all planets that are requesting a mission creation from Gameloop
	vector<Planet*> m_planetsToCreateMission;//all planets that are requesting a mission creation from Gameloop
	vector<AsteroidField*> m_asteroidFields;//all asteroid fields created
	vector<Planet*> m_planets;//all planets created

	sf::Vector2i m_previous_star_sector_index;
	map<int, Planet*> m_planet_ids;

	//DEBUG
	GameObject* m_sector_debug_current;
	GameObject* m_sector_debug_onscreen;

	//Network
	bool m_is_server;
	NetworkStatus m_network_status;
	sf::IpAddress m_ip;
	int m_port_receive;
	int m_port_send;

	NetworkStatus UpdateNetworkConnexion();

	//Network - client
	//sf::TcpSocket m_receiver;
	//
	////Network - server
	//sf::TcpSocket m_sender;
	//sf::TcpListener m_listener;

	sf::UdpSocket m_socket;

private:
	void AddSFTextToVector(SFText* pSFText, vector<SFText*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);

	RenderWindow* m_window;
	
	list<RectangleShape*> m_sceneFeedbackBars;
	list<Text*> m_sceneFeedbackTexts;
	list<SFPanel*> m_sceneFeedbackSFPanels;
	vector<SFText*> m_sceneFeedbackSFTexts;
	vector<GameObject*> m_sceneGameObjectsTypedTemp[NBVAL_ColliderType];
	vector<SFText*> m_garbageTexts;
};

#endif // GAME_H_INCLUDED