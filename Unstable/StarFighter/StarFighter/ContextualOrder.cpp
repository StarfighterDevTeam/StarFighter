#include "ContextualOrder.h"

extern Game* CurrentGame;

ContextualOrder::ContextualOrder() : GameEntity(UI_None)
{
	m_type = Order_None;

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setFillColor(sf::Color::Black);

	m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);

	m_mouse_click = new GameEntity(UI_None);
	sf::Texture* textureL = TextureLoader::getInstance()->loadTexture("2D/mouse_click.png", 10, 32);
	m_mouse_click->setAnimation(textureL, 1, 2);
}

ContextualOrder::~ContextualOrder()
{
	delete m_mouse_click;
}

void ContextualOrder::Draw(sf::RenderTexture& screen)
{
	GameEntity::Draw(screen);
	m_mouse_click->Draw(screen);
}

void ContextualOrder::SetContextualOrder(ContextualOrderType order, sf::Vector2f position, bool is_possible, int value, int value2)
{
	//change order type?
	if (m_type != order)
	{
		m_type = order;

		string textureName;
		ostringstream ss_order;
		sf::Color text_color = sf::Color::White;
		bool right_click = true;

		switch (order)
		{
			case Order_None:
			{
				break;
			}
			case Order_Sail:
			{
				ss_order << "Sail: " << value << " days";
				textureName = "2D/order_sail.png";
				text_color = is_possible == true ? text_color : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Engage:
			{
				ss_order << "Engage ship: " << value << " days" << " (" << value2 << ")";
				textureName = "2D/order_melee.png";
				text_color = is_possible == true ? text_color : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Interact:
			{
				ss_order << "Interact: " << value << " days";
				textureName = "2D/order_sail.png";
				text_color = is_possible == true ? (*CurrentGame).m_dico_colors[Color_Green_System] : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Dock:
			{
				ss_order << "Dock: " << value << " days";
				textureName = "2D/order_sail.png";
				text_color = is_possible == true ? (*CurrentGame).m_dico_colors[Color_Green_System] : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Move:
			{
				ss_order << "Move";
				textureName = "2D/order_move.png";
				text_color = is_possible == true ? text_color : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Swim:
			{
				ss_order << "Swim";
				textureName = "2D/order_move.png";
				text_color = is_possible == true ? text_color : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Heal:
			{
				ss_order << "Go heal";
				textureName = "2D/order_heal.png";
				break;
			}
			case Order_TargetRoom:
			{
				ss_order << "Target room";
				textureName = "2D/order_target.png";
				break;
			}
			case Order_TargetHull:
			{
				ss_order << "Target hull";
				textureName = "2D/order_target.png";
				text_color = (*CurrentGame).m_dico_colors[Color_Red_Target_Locked];
				break;
			}
			case Order_Weapon:
			{
				ss_order << "Operate weapon";
				textureName = "2D/order_system.png";
				text_color = is_possible == true ? (*CurrentGame).m_dico_colors[Color_Green_System] : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Engine:
			{
				ss_order << "Operate engine";
				textureName = "2D/order_system.png";
				text_color = is_possible == true ? (*CurrentGame).m_dico_colors[Color_Green_System] : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Rudder:
			{
				ss_order << "Operate rudder";
				textureName = "2D/order_system.png";
				text_color = is_possible == true ? (*CurrentGame).m_dico_colors[Color_Green_System] : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_Flee:
			{
				ss_order << "Flee";
				textureName = "2D/order_sail.png";
				text_color = is_possible == true ? (*CurrentGame).m_dico_colors[Color_Green_System] : (*CurrentGame).m_dico_colors[Color_Red_Impossible];
				break;
			}
			case Order_RepairHull:
			{
				ss_order << "Repair hull";
				textureName = "2D/order_repair.png";
				text_color = (*CurrentGame).m_dico_colors[Color_Green_System];
				break;
			}
			case Order_OpenRoomConnexion:
			{
				ss_order << "Open door";
				textureName = "2D/order_open.png";
				text_color = sf::Color::White;
				right_click = false;
				break;
			}
			case Order_CloseRoomConnexion:
			{
				ss_order << "Close door";
				textureName = "2D/order_close.png";
				text_color = sf::Color::White;
				right_click = false;
				break;
			}
		}

		if (textureName.empty() == false)
		{
			sf::Texture* texture = TextureLoader::getInstance()->loadTexture(textureName, (int)CONTEXTUAL_ORDER_SIZE, (int)CONTEXTUAL_ORDER_SIZE);
			setAnimation(texture, 1, 1);
		}
		
		m_text.setFillColor(text_color);
		m_text.setString(ss_order.str());

		m_shape.setSize(sf::Vector2f(m_text.getGlobalBounds().width + 12 + 20, m_text.getCharacterSize() + 8.f));
		
		m_mouse_click->setAnimationLine(right_click == false ? 0 : 1);
	}

	//position
	setPosition(sf::Vector2f(position.x, position.y + (WATERTILE_SIZE - CONTEXTUAL_ORDER_SIZE) * 0.5f - 4));
	m_text.setPosition(sf::Vector2f(position.x - m_text.getGlobalBounds().width * 0.5f, position.y - (WATERTILE_SIZE * 0.5f) - 2));
	m_shape.setPosition(sf::Vector2f(m_text.getPosition().x - (6 + 20), m_text.getPosition().y));
	m_mouse_click->setPosition(sf::Vector2f(m_text.getPosition().x - (6 + 20) * 0.5f, m_text.getPosition().y + m_shape.getSize().y * 0.5f));
}