#ifndef SFMAPPANEL_H_INCLUDED
#define SFMAPPANEL_H_INCLUDED

#include "SFUpgradesPanel.h"

#define STELLARMAP_SCALE			70
#define STELLAR_SEGMENT_THICKNESS	4

enum StellarComponentStates
{
	StellarComponent_NormalState,
	StellarComponent_HighlightState,
};

class StellarHub : public sf::RectangleShape
{
public:
	StellarHub();
	StellarHub(string hub_name);
	StellarHub(string hub_name, sf::Vector2f coordinates);
	sf::Vector2f m_coordinates;
	string m_display_name;
	float m_distance_to_current_position;
	StellarComponentStates m_feedback_state;
	//int m_level;

	bool Update(GameObject& cursor, bool forbid_collision);
};

class StellarSegment : public sf::RectangleShape
{
public:
	StellarSegment(bool vertical, float segment_size);
	StellarSegment(string segment_name, Directions direction, float segment_size, sf::Vector2f coordinates);
	sf::Vector2f m_coordinates;
	string m_display_name;
	int m_max_hazard_unlocked;
	float m_size;
	StellarComponentStates m_feedback_state;
	bool m_vertical;

	bool Update(GameObject& cursor, bool forbid_collision, vector<StellarSegment*> branch_segments);
};

class StellarNode : public sf::CircleShape
{
public:
	StellarNode();
	StellarNode(sf::Vector2f coordinates);
	sf::Vector2f m_coordinates;
	StellarComponentStates m_feedback_state;
};

class StellarBranch
{
public:
	StellarBranch();
	StellarBranch(string name);

	StellarHub* m_hub;
	vector<StellarSegment*> m_segments;
	vector<StellarNode*> m_nodes;
	Directions m_direction;
	string m_name;

	void SetPosition(sf::Vector2f position);
	void DrawNodes(sf::RenderTexture& screen);
	void DrawSegments(sf::RenderTexture& screen);
	void DrawHub(sf::RenderTexture& screen);
};


class SFStellarInfoPanel : public SFPanel
{
public:
	SFStellarInfoPanel(sf::Vector2f position, sf::Vector2f size, Ship* playership);
	SFStellarInfoPanel(StellarHub* hub, int teleportation_cost, sf::Vector2f size, Ship* playership);
	SFStellarInfoPanel(StellarSegment* segment, sf::Vector2f size, Ship* playership);
	void Draw(sf::RenderTexture& screen) override;
	static sf::Vector2f GetRealCoordinates(sf::Vector2f rendered_coordinates, sf::Vector2f panel_position, sf::Vector2f panel_size);
	static sf::Vector2f GetFakeCoordinates(sf::Vector2f rendered_coordinates, sf::Vector2f panel_position, sf::Vector2f panel_size);

	string m_location_name;
};

class SFMapPanel : public SFPanel
{
public:
	SFMapPanel(sf::Vector2f size, Ship* playership);
	~SFMapPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	GameObject* GetCursor() override;
	int GetTeleportationCost() override;
	string GetTeleportationDestination() override;
	void SetMapViewOffset(sf::Vector2f offset) override;

	void GetScrollingInput(GameObject& cursor, sf::Time deltaTime);

	SFStellarInfoPanel* m_info_panel;
	StellarHub* m_current_hub;
	vector<StellarBranch*> m_branches;

	void UpdateBranchesPosition(bool into_real_coordinates, bool into_fake_coordinates);

	GameObject m_cursor;
	GameObject m_ship;
	int m_teleportation_cost;
	string m_targeted_location;

	void CreateStellarMap_v2();

	StellarBranch* m_hightlighted_branch;

private:
	vector<string> m_checked_scenes;
	bool IsSceneAlreadyChecked(string new_scene, bool add_if_not_checked);
	bool IsSceneKnownByThePlayer(string new_scene);
	int GetMaxHazardLevelUnlocked(string new_scene);
	int ComputeTeleportationCost(StellarHub* destination);

	sf::RenderTexture m_texture;
	sf::Vector2f m_scroll_offset;
	sf::FloatRect m_map_content_area;
};

#endif // SFMAPPANEL_H_INCLUDED