#ifndef SFSTRATAGEMPANEL_H_INCLUDED
#define SFSTRATAGEMPANEL_H_INCLUDED

#include "Game.h"
#include "Ship.h"

enum CodeAnimations
{
	Code_Normal,
	Code_Validated,
	Code_Invalid,
	NBVAL_CodeAnimations,//3
};

class ItemBox
{
public:
	ItemBox();
	void Init(Item* item, GameObject& code_sprite);
	~ItemBox();
	Item* m_item;
	GameObject* m_codes[MAX_CODES];
	GameObject* m_item_image;
	SFText m_item_text;

	void Draw(sf::RenderTexture& screen);
	void SetPosition(sf::Vector2f position);
	void CopyAndDelete(ItemBox& item_to_copy);

	int m_current_code_index;
};

class SFStratagemPanel : public SFPanel
{
public:
	SFStratagemPanel(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip);
	~SFStratagemPanel();
	void Update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen) override;

	Item* m_current_stratagem;
	GameObject m_code;
	ItemBox m_boxes[MAX_ITEMS_PER_AGENT];

	int m_current_code[MAX_CODES];

	Item* CheckCodeInput(int input) override;
};

#endif // SFSTRATAGEMPANEL_H_INCLUDED