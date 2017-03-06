#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Item.h"

class Agent : public GameObject
{
public :
	Agent();
	Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Agent();
	void update(sf::Time deltaTime) override;

	vector<Item*> m_items;
	float m_vision_range;
	float m_vision_angle;
	GameObject* m_vision_cone;
	float m_max_current_awareness;
	float m_angular_speed;
	float m_desired_angle;
	sf::Vector2f m_desired_speed;

	bool TurnToDesiredAngle(sf::Time deltaTime);
	void GenerateItems();
	Stratagem* GetRandomStratagem(ItemType item_type);

	void Roaming();

	//pathfind
	void FindShortedPath(size_t start_index, size_t target_index);
	void IteratePathFindingOnIndex(size_t index, size_t target_index);
	int GetTilePosX(size_t tile_index);
	int GetTilePosY(size_t tile_index);
	size_t GetTileIndex(int pos_x, int pos_y);
	list<size_t> m_closed_list_pathfind;
	list<size_t> m_open_list_pathfind;
	Tile m_tiles[42 + 1];
	int m_nb_tile_lines;
	int m_nb_tile_rows;
	int m_tile_size;
	size_t m_current_tile;
	list<size_t> m_current_path;
};

#endif // AGENT_H_INCLUDED
