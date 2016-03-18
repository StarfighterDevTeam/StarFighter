#ifndef SFMAPPANEL_H_INCLUDED
#define SFMAPPANEL_H_INCLUDED

#include "Scene.h"

#define STELLARMAP_SCALE			70
#define STELLAR_SEGMENT_THICKNESS	4

class StellarHub : public sf::RectangleShape
{
public:
	StellarHub();
	StellarHub(string hub_name);
	sf::Vector2f m_coordinates;
	string m_display_name;
	float m_distance_to_current_position;
};

class StellarSegment : public sf::RectangleShape
{
public:
	StellarSegment(bool vertical, float segment_size);
	sf::Vector2f m_coordinates;
	string m_display_name;
	int m_max_hazard_unlocked;
	float m_size_on_stellar_map;
};

class StellarNode : public sf::CircleShape
{
public:
	StellarNode();
	sf::Vector2f m_coordinates;
};

class StellarBranch
{
public:
	StellarBranch();

	StellarHub* m_hub;
	vector<StellarSegment*> m_segments;
	vector<StellarNode*> m_nodes;
	Directions m_direction;

	void SetPosition(sf::Vector2f position);
	void DrawNodes(sf::RenderTexture& screen);
	void DrawSegments(sf::RenderTexture& screen);
	void DrawHub(sf::RenderTexture& screen);
};

class SFMapPanel : public SFPanel
{
public:
	SFMapPanel(sf::Vector2f size, Ship* playerShip);
	~SFMapPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;

	StellarHub* m_current_hub;
	vector<StellarBranch*> m_branches;

	void UpdateBranchesPosition();
	void ScanBranches(string starting_scene, Directions direction, sf::Vector2f starting_coordinates);
	bool ScanScene(string scene_filename, string scene, Directions direction, sf::Vector2f starting_coordinates);

	vector<vector<string>> m_scenesConfig;
	size_t m_scenesConfigSize;

private:
	vector <string> m_checked_scenes;
	bool IsSceneAlreadyChecked(string new_scene, bool add_if_not_checked);
};

#endif // SFMAPPANEL_H_INCLUDED