#ifndef CONTEXTUALORDER_H_INCLUDED
#define CONTEXTUALORDER_H_INCLUDED

#include "GameEntity.h"

enum ContextualOrderType
{
	Order_None,
	Order_Move,
	Order_MoveImpossible,
	Order_Swim,
	Order_SwimImpossible,
	Order_RepairHull,
	Order_Weapon,
	Order_TargetRoom,
	Order_TargetHull,
	Order_Engine,
	Order_Flee,
	Order_FleeImpossible,
	Order_Rudder,
	Order_Heal,
	Order_Sail,
	Order_Dock,
	Order_Engage,
	Order_OpenRoomConnexion,
	Order_CloseRoomConnexion,
};

class ContextualOrder : public GameEntity
{
public:
	ContextualOrder();
	~ContextualOrder();

	ContextualOrderType m_type;
	void SetContextualOrder(ContextualOrderType order, sf::Vector2f position);
};

#endif //CONTEXTUALORDER_H_INCLUDED