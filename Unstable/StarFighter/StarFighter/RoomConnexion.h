#ifndef ROOMCONNEXION_H_INCLUDED
#define ROOMCONNEXION_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "RoomTile.h"

class Room;//forward declaration
class CrewMember;


#define CONNEXION_WIDTH			4.f
#define CONNEXION_OPEN_LENGTH	4.f		

class RoomConnexion : public GameEntity
{
public:
	RoomConnexion(pair<RoomTile*, RoomTile*> tiles, bool open, bool minimized, Ship* ship);
	
	pair<RoomTile*, RoomTile*> m_tiles;
	bool m_open;
	bool m_locked;
	bool m_destroyed;

	void Update(Time deltaTime);
	void SetLock(bool locked);
	void Destroy();

	Ship* m_ship;
};

#endif //ROOMCONNEXION_H_INCLUDED