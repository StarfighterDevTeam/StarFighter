#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"
#include "Room.h"

#define CREWMEMBER_SIZE							24.f
#define CREWMEMBER_SPEED						100.f
#define CREWMEMBER_SPEED_FLOOD_FACTOR			0.5f
#define CREWMEMBER_SPEED_PRISONER_FACTOR		0.5f
#define CREWMEMBER_HEALTH_MAX					20
#define CREWMEMBER_MELEE_ROF					1.f
#define CREWMEMBER_MELEE_DAMAGE					1
#define PRISONER_HEALTH_MIN_TO_ESCAPE			5

class CrewMember : public GameEntity
{
public:
	CrewMember(CrewMemberType type, ShipAlliance alliance, CrewMemberRace race = NB_CREW_RACES, bool prisoner = false);
	~CrewMember();

	CrewMemberType m_type;
	ShipAlliance m_alliance;
	CrewMemberRace m_race;

	int m_skills[NB_CREW_SKILLS];
	int m_skills_max[NB_CREW_SKILLS];
	int m_upkeep_cost[3];

	void Update(Time deltaTime);
	void UpdateLifeBar();

	RoomTile* GetFreeRoomTile(Room* room);
	void MoveToRoomTile(RoomTile* tile);

	bool Imprison(RoomTile* prison_cell);
	void UpdateMelee(sf::Time deltaTime);
	float UpdateAndGetMeleeRof();

	sf::Vector2f m_speed;
	float m_ref_speed;
	int m_health;
	int m_health_max;

	RoomTile* m_tile;
	RoomTile* m_destination;
	vector<RoomTile*> m_current_path;

	float m_repair_timer;
	float m_drowning_timer;
	float m_healing_timer;

	bool m_is_prisoner;
	float m_prisoner_roaming_timer;

	CrewMember* m_melee_opponent;
	float m_melee_rof_timer;
	float m_melee_rof;
	int m_melee_damage;

	GameEntity* m_lifebar;
	string m_display_name;
	string m_texture_name;
	string m_texture_big_name;

	CrewMember* m_crew_linked;
	CrewMember* Clone();

	int m_fidelity;

private:
	bool FindShortestPath(RoomTile* tileA, RoomTile* tileB);
	void IteratePathFinding(RoomTile* tileA, RoomTile* tileB);
	void IterateEnemyPathFinding(RoomTile* tileA, RoomTile* tileB);
	list<RoomTile*> m_closed_list_pathfind;
	list<RoomTile*> m_open_list_pathfind;
};

#endif //CREWMEMBER_H_INCLUDED