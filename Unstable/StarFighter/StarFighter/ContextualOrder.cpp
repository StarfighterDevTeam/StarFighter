#include "ContextualOrder.h"

extern Game* CurrentGame;

ContextualOrder::ContextualOrder() : GameEntity(UI_None)
{
	m_type = Order_None;

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::Black);
}

ContextualOrder::~ContextualOrder()
{
	
}

void ContextualOrder::SetContextualOrder(ContextualOrderType order, sf::Vector2f position)
{
	//change order type?
	if (m_type != order)
	{
		m_type = order;

		string textureName;
		ostringstream ss_order;
		sf::Color text_color = sf::Color::Black;

		switch (order)
		{
		case Order_None:
		{
			break;
		}
		case Order_Sail:
		{
			ss_order << "Sail";
			textureName = "2D/order_sail.png";
			//text_color = (*CurrentGame).m_dico_colors[Color_Blue_Pierced];
			break;
		}
		case Order_Engage:
		{
			ss_order << "Engage ship";
			textureName = "2D/order_engage.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Red_Target_Locked];
			break;
		}
		case Order_Dock:
		{
			ss_order << "Dock";
			textureName = "2D/order_sail.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Green_System];
			break;
		}
		case Order_Move:
		{
			ss_order << "Move";
			textureName = "2D/order_sail.png";
			text_color = sf::Color::White;
			break;
		}
		case Order_Swim:
		{
			ss_order << "Swim";
			textureName = "2D/order_sail.png";
			text_color = sf::Color::White;
			break;
		}
		case Order_Heal:
		{
			ss_order << "Go heal";
			textureName = "2D/order_sail.png";
			text_color = sf::Color::White;
			break;
		}
		case Order_TargetRoom:
		{
			ss_order << "Target room";
			textureName = "2D/order_engage.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Red_Target_Locked];
			break;
		}
		case Order_TargetHull:
		{
			ss_order << "Target hull";
			textureName = "2D/order_engage.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Red_Target_Locked];
			break;
		}
		case Order_Weapon:
		{
			ss_order << "Operate weapon";
			textureName = "2D/order_engage.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Red_Target_Locked];
			break;
		}
		case Order_Engine:
		{
			ss_order << "Operate engine";
			textureName = "2D/order_sail.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Green_System];
			break;
		}
		case Order_Rudder:
		{
			ss_order << "Operate rudder";
			textureName = "2D/order_sail.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Green_System];
			break;
		}
		case Order_RepairHull:
		{
			ss_order << "Repair hull";
			textureName = "2D/order_sail.png";
			text_color = (*CurrentGame).m_dico_colors[Color_Blue_Pierced];
			break;
		}
		//Order_None,
		//Order_Move,
		//Order_Repair,
		//Order_Weapon,
		//Order_Engine,
		//Order_Navigation,
		//Order_Heal,
		//Order_Sail,
		//Order_Seaport,
		//Order_Engage,
		}

		if (textureName.empty() == false)
		{
			sf::Texture* texture = TextureLoader::getInstance()->loadTexture(textureName, (int)CONTEXTUAL_ORDER_SIZE, (int)CONTEXTUAL_ORDER_SIZE);
			setAnimation(texture, 1, 1);
		}
		
		m_text.setColor(text_color);
		m_text.setString(ss_order.str());
	}

	//position
	setPosition(sf::Vector2f(position.x, position.y + (WATERTILE_SIZE - CONTEXTUAL_ORDER_SIZE) * 0.5 - 4));
	m_text.setPosition(sf::Vector2f(position.x - m_text.getGlobalBounds().width * 0.5, position.y - (WATERTILE_SIZE * 0.5) - 2));
}