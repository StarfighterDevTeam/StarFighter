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

using namespace sf;


enum ManaType
{
	Mana_Fire,		//0
	Mana_Water,		//1
	Mana_Air,		//2
	Mana_Earth,		//3
	NB_MANATYPES,	//4
};

enum ManaValue
{
	Mana_0,		//0
	Mana_1,		//1
	Mana_2,		//2
	Mana_3,		//3
};

class Card
{
public:
	Card(){};
	Card(ManaType type, ManaValue value, int owner) { m_type = type; m_value = value; m_owner = owner; }

	ManaType m_type;
	ManaValue m_value;
	int m_owner;
};

enum CardSlotStatus
{
	CardSlot_Free,		//0
	CardSlot_Occupied,	//1
	CardSlot_Burnt,		//2
	CardSlot_Temp,		//3
};

enum MouseAction
{
	Mouse_None,
	Mouse_LeftClick,
	Mouse_RightClick,
};

enum CardStack
{
	Stack_None,
	Stack_Hand,
	Stack_Altar,
	Stack_AltarSlot,
	Stack_Library,
	Stack_Graveyard,
	Stack_MonsterCurses,
};

class CardSlot
{
public:
	CardSlot(){ m_status = CardSlot_Free; m_hovered = false; m_selected = false; }
	CardSlotStatus m_status;
	Card m_card;
	CardStack m_stack;
	int m_index;
	void GetCard(Card& card);
	static sf::Color GetManaColor(ManaType type);
	static sf::Color GetStatusColor(CardSlotStatus status);
	void Update(MouseAction mouse_click);

	RectangleShape m_shape_container;
	RectangleShape m_shape;
	SFText m_text;

	bool m_hovered;
	bool m_selected;
};

struct Game
{
public:
	void init(RenderWindow* window);
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
	void changeObjectTypeAndLayer(GameObject *object, LayerType new_layer, GameObjectType new_type);

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

	Ship* m_playerShip;
	bool m_pause;
	bool m_window_has_focus;

	sf::Vector2f m_mouse_pos;
	MouseAction m_mouse_click;
	float m_mouse_click_timer;

	sf::View m_view;
	sf::Vector2f m_map_size;

	//Utiliary methods
	GameObject* GetClosestObjectTyped(const GameObject* ref_obj, GameObjectType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	GameObject* GetClosestObjectTyped(const sf::Vector2f position, GameObjectType type_of_closest_object, float dist_max = 1.f, float angle_delta_max = -1.f);
	std::vector<GameObject*> GetSceneGameObjectsTyped(GameObjectType type);

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

	//BIG BOOK
	CardSlot* m_hovered_slot;
	CardSlot* m_selected_slot;
	CardSlot* m_play_card_slot;
	CardSlot* m_target_slot;

private:
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	void AddGameObjectVectorToVector(vector<GameObject*> vector_slave, vector<GameObject*>* vector_master);
	void TransferGameObjectLayeredTempToSceneObjectsLayered(LayerType layer);
	void TransferGameObjectTypedTempToSceneObjectsTyped(GameObjectType collider_type);
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
	std::vector<GameObject*> m_sceneGameObjectsLayeredTemp[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTypedTemp[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
	std::vector<SFText*> m_garbageTexts;
};

#endif // GAME_H_INCLUDED