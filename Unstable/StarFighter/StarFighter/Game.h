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
#include "Hud.h"

class Ship;

using namespace sf;

enum TargetScan
{
	NO_TARGET_FOUND,//0
	TARGET_OUT_OF_RANGE,//1
	TARGET_IN_RANGE,//2
};

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
	void updateHud(int armor, int armor_max, int shield, int shield_max, int money, int graze_count, int hazard_level, std::string scene_name, int level, int level_max, int xp, int xp_max, sf::Time deltaTime, bool hub,
		int focused_item_type = -1, string f_name = "", int f_level = 1, int f_xp = 0, float f_max_speed = 0, float f_hyperspeed = 1, int f_armor = 1, int f_shield = 0, int f_shield_regen = 0, 
		int f_damage = 0, bool f_bot = false, float f_ammo_speed = 0, PatternType f_pattern = NoMovePattern,
		int f_multishot = 1, int f_xspread = 0, float f_rate_of_fire = 1, ShotMode f_shot_mode = NoShotMode, float f_dispersion = 0, int f_rafale = 0, float f_rafale_cooldown = 0, TargetSeaking f_target_seaking = NO_SEAKING);
	void drawScene();
	void drawHud();
	void colisionChecksV2();
	void cleanGarbage();
	void collectGarbage();
	void garbageLayer(LayerType m_layer, bool only_offscene = false);
	void killGameObjectLayer(GameObjectType m_layer);
	void SetLayerRotation(LayerType m_layer, float angle);

	sf::RenderTexture m_mainScreen;
	sf::RenderTexture m_hubScreen;
	PlayerHud m_hud;
	sf::Vector2i m_screen_size;
	float m_hyperspeedMultiplier;
	float m_vspeed;

	//methods v2
	bool InsertObjectInGrid(ObjectGrid& grid, GameObject& object, int index = 0);
	bool SwapObjectBetweenGrids(ObjectGrid& grid, ObjectGrid& grid2, int index1, int index2);

	//methods v1
	bool InsertObjectInShipGrid(GameObject& object, int index = 0);
	bool InsertObjectInEquipmentGrid(GameObject& object, int index=-1);
	bool SwapEquipObjectInShipGrid(int index_ship, int index_equipment);

	void GarbageObjectInGrid(int grid_id, int index);
	GameObject* getHudFocusedItem();
	sf::Vector2i getHudFocusedGridAndIndex();
	sf::Vector2i getHudFocusedIndexWithinGrid(HudGridsIndex grid_);
	void setRemovingCursorAnimation(CursorFeedbackStates animation_index);

	bool isLastEnemyDead();
	int getHazard();
	void resetHazard(int hazard_overkill = 0);

	sf::Vector2f m_scale_factor;
	Directions m_direction;

	int m_hazard;
	int m_hazardSpawned;
	float m_BeastScoreBonus;

	Ship* playerShip;
	void SetPlayerShip(Ship* m_playerShip);
	void GetBeastScoreBonus(float m_playerShipBeastScore, float m_sceneBeastScore);

	TargetScan FoundNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	float GetAngleToNearestGameObject(GameObjectType type, sf::Vector2f ref_position, float range = 0);
	void WakeUpEnemiesWithName(string m_display_name);

	InteractionPanel* m_interactionPanel;
	GameObject* UpdateInteractionPanel (InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime);
	void SetSelectedIndex(int index_);
	void SetSelectedDirection(Directions direction_);
	void SetSelectedDestination(string destination_);
	int GetSelectedIndex();
	void SetShopMenu(ShopMenus menu);
	ShopMenus GetShopMenu();

	int GetPlayerStatsMultiplierForLevel(int level_);
	int GetEnemiesStatsMultiplierForLevel(int level_);
	int GetBonusStatsMultiplierToBeOnParForLevel(int level_);

private:
	void SetMovementFromPattern(Vector2f* move, float delta_t, int movepattern_type);
	void AddGameObjectToVector(GameObject* pGameObject, vector<GameObject*>* vector);
	bool isVectorEmpty(vector <GameObject*>* vector);
	Clock m_sceneChronometer;

	float m_movementClock;
	RenderWindow *m_window;

	std::list<RectangleShape*> m_sceneFeedbackBars;
	std::list<Text*> m_sceneFeedbackTexts;
	std::vector<GameObject*> m_sceneGameObjects;
	std::vector<GameObject*> m_sceneGameObjectsLayered[NBVAL_Layer];
	std::vector<GameObject*> m_sceneGameObjectsTyped[NBVAL_GameObject];
	std::vector<GameObject*> m_garbage;
};

#endif // GAME_H_INCLUDED