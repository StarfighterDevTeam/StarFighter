#ifndef ROOMCONNEXION_H_INCLUDED
#define ROOMCONNEXION_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "RoomTile.h"

class Room;//forward declaration
class CrewMember;


#define CONNEXION_WIDTH			8.f	

class RoomConnexion : public GameEntity
{
public:
	RoomConnexion(pair<RoomTile*, RoomTile*> tiles, bool locked, Ship* ship);
	
	pair<RoomTile*, RoomTile*> m_tiles;
	bool m_is_locked;
	bool m_destroyed;

	void Update(Time deltaTime);
	void SetLock(bool locked);
	void Destroy();

	Ship* m_ship;
};

#endif //ROOMCONNEXION_H_INCLUDED