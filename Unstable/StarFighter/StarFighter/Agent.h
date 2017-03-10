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
	float m_desired_speed;
	sf::Vector2f m_desired_destination;
	sf::Clock m_angleClock;

	bool TurnToDesiredAngle(sf::Time deltaTime);
	void GoToWaypoint(size_t index);
	void UpdateAngleToDestination();
	void GenerateItems();
	Stratagem* GetRandomStratagem(ItemType item_type);

	void Roaming();

	//pathfind
	void FindShortestPath(size_t start_index, size_t target_index);
	void FindShortestPathTo(size_t target_index);
	void IteratePathFindingOnIndex(size_t index, size_t target_index);
	list<size_t> m_closed_list_pathfind;
	list<size_t> m_open_list_pathfind;
	vector<Tile> m_tiles;
	
	size_t m_current_tile;
	list<size_t> m_current_path;
};

#endif // AGENT_H_INCLUDED
