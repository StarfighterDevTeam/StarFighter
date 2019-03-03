#ifndef CONTEXTUALORDER_H_INCLUDED
#define CONTEXTUALORDER_H_INCLUDED

#include "GameEntity.h"

enum ContextualOrderType
{
	Order_None,
	Order_Move,
	Order_Repair,
	Order_Weapon,
	Order_Engine,
	Order_Navigation,
	Order_Heal,
	Order_Sail,
	Order_Dock,
	Order_Engage,
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