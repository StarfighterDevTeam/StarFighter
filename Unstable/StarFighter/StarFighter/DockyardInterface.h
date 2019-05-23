#ifndef DOCKYARDINTERFACE_H_INCLUDED
#define DOCKYARDINTERFACE_H_INCLUDED

#include "Game.h"
#include "Ship.h"
#include "CrewUnboardInterface.h"

#define DOCKYARDINTERFACE_SIZE_X		CREWUNBOARDINTERFACE_SIZE_X
#define DOCKYARDINTERFACE_SIZE_Y		CREWUNBOARDINTERFACE_SIZE_Y

#define DOCKYARDINTERFACE_DETAIL_SIZE_X		400
#define DOCKYARDINTERFACE_DETAIL_SIZE_Y		500

#define SHOP_ITEM_CONTAINER_SIZE_X			400
#define SHOP_ITEM_CONTAINER_SIZE_Y			80

class ShopItem
{
public:
	ShopItem(UpgradeType upgrade_type);
	~ShopItem();
	Upgrade* m_upgrade;
	GameEntity* m_cost;
};


struct DockyardInterface
{
	DockyardInterface();
	~DockyardInterface();

	void Init(Ship* ship, Location* location);
	void Destroy();

	void InitDetail(Upgrade* upgrade);
	void DestroyDetail();

	UpgradeType Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);

	Ship* m_ship;
	Location* m_location;
	GameEntity* m_panel;
	sf::Text m_narrative_text;

	vector<ShopItem*> m_items;

	GameEntity* m_detail_panel;
	GameEntity* m_focused_item;
	sf::Text m_detail_title;
	sf::Text m_detail_body;

	GameEntity* m_leave_button;
};


#endif //DOCKYARDINTERFACE_H_INCLUDED