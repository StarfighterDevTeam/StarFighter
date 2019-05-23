#include "DockyardInterface.h"

extern Game* CurrentGame;

DockyardInterface::DockyardInterface()
{
	m_panel = NULL;
	m_leave_button = NULL;
	m_focused_item = NULL;
	m_detail_panel = NULL;
}

DockyardInterface::~DockyardInterface()
{
	//delete m_panel;
	//delete m_drowning_bar;

	Destroy();
}

void DockyardInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	delete m_leave_button;
	m_leave_button = NULL;

	for (vector<ShopItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		delete *it;
	}
	m_items.clear();

	DestroyDetail();
}

void DockyardInterface::Init(Ship* ship, Location* location)
{
	m_ship = ship;
	m_location = location;

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(DOCKYARDINTERFACE_SIZE_X, DOCKYARDINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(DOCKYARDINTERFACE_SIZE_X * 0.5, DOCKYARDINTERFACE_SIZE_Y * 0.5));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setOutlineThickness(2);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - CREWUNBOARDINTERFACE_SIZE_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	
	//narrative text
	string ss_narrative = "Welcome to your dockyard.\nPlease suit yourself and buy all what you need for your ship (with left-click).";
	
	float offset_y = m_panel->m_position.y - DOCKYARDINTERFACE_SIZE_Y * 0.5;
	offset_y += 20;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_narrative);
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5 + 20, offset_y));

	offset_y += m_narrative_text.getGlobalBounds().height + 50;

	//array of items
	for (vector<string>::iterator it = m_location->m_upgrades.begin(); it != m_location->m_upgrades.end(); it++)
	{
		ShopItem* item = new ShopItem(*it);

		//container
		item->m_upgrade->m_shape_container.setSize(sf::Vector2f(SHOP_ITEM_CONTAINER_SIZE_X, SHOP_ITEM_CONTAINER_SIZE_Y));
		item->m_upgrade->m_shape_container.setOrigin(sf::Vector2f(item->m_upgrade->m_shape_container.getSize().x * 0.5, item->m_upgrade->m_shape_container.getSize().y * 0.5));
		item->m_upgrade->m_shape_container.setFillColor(sf::Color::Black);
		item->m_upgrade->m_shape_container.setOutlineThickness(2);
		item->m_upgrade->m_shape_container.setOutlineColor(sf::Color::White);
		item->m_upgrade->m_shape_container.setPosition(sf::Vector2f(m_narrative_text.getPosition().x + item->m_upgrade->m_shape_container.getSize().x * 0.5, offset_y + SHOP_ITEM_CONTAINER_SIZE_Y * 0.5));

		//icon container
		item->m_upgrade->m_shape.setSize(sf::Vector2f(UPGRADE_ICON_SIZE, UPGRADE_ICON_SIZE));
		item->m_upgrade->m_shape.setOrigin(sf::Vector2f(item->m_upgrade->m_shape.getSize().x * 0.5, item->m_upgrade->m_shape.getSize().y * 0.5));
		item->m_upgrade->m_shape.setFillColor(sf::Color::Black);
		item->m_upgrade->m_shape.setOutlineThickness(2);
		item->m_upgrade->m_shape.setOutlineColor(sf::Color::White);
		item->m_upgrade->m_shape.setPosition(sf::Vector2f(m_narrative_text.getPosition().x + item->m_upgrade->m_shape.getSize().x * 0.5 + 8, item->m_upgrade->m_shape_container.getPosition().y));

		//icon
		Texture* texture = TextureLoader::getInstance()->loadTexture(item->m_upgrade->m_texture_name, UPGRADE_ICON_SIZE, UPGRADE_ICON_SIZE);
		item->m_upgrade->setAnimation(texture, 1, 1);
		item->m_upgrade->setPosition(item->m_upgrade->m_shape.getPosition());

		//text
		item->m_upgrade->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		item->m_upgrade->m_text.setCharacterSize(20);
		item->m_upgrade->m_text.setStyle(sf::Text::Bold);
		item->m_upgrade->m_text.setColor(sf::Color::White);
		item->m_upgrade->m_text.setString(item->m_upgrade->m_display_name);
		item->m_upgrade->m_text.setPosition(sf::Vector2f(item->m_upgrade->m_shape.getPosition().x + item->m_upgrade->m_shape.getSize().x * 0.5 + 12, item->m_upgrade->getPosition().y - item->m_upgrade->m_text.getCharacterSize() * 0.65));

		//cost
		Texture* texture_gold = TextureLoader::getInstance()->loadTexture("2D/icon_gold.png", RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
		item->m_cost->setAnimation(texture_gold, 1, 1);
		item->m_cost->setPosition(sf::Vector2f(item->m_upgrade->m_shape_container.getPosition().x + item->m_upgrade->m_shape_container.getSize().x * 0.5 - 100, item->m_upgrade->getPosition().y));

		item->m_cost->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		item->m_cost->m_text.setCharacterSize(16);
		item->m_cost->m_text.setStyle(sf::Text::Bold);
		item->m_cost->m_text.setColor(sf::Color::White);
		item->m_cost->m_text.setString(to_string(item->m_upgrade->m_cost));
		item->m_cost->m_text.setPosition(sf::Vector2f(item->m_cost->getPosition().x + RESOURCES_ICON_SIZE * 0.5 + 8, item->m_cost->getPosition().y - item->m_cost->m_text.getCharacterSize() * 0.65));
		
		m_items.push_back(item);

		offset_y += SHOP_ITEM_CONTAINER_SIZE_Y;
	}
	
	//leave button
	offset_y += m_narrative_text.getGlobalBounds().height + m_narrative_text.getCharacterSize();
	m_leave_button = new GameEntity(UI_None);
	m_leave_button->m_shape_container.setSize(sf::Vector2f(RESOURCES_BUTTON_SIZE_X, RESOURCES_BUTTON_SIZE_Y));
	m_leave_button->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_BUTTON_SIZE_X * 0.5, RESOURCES_BUTTON_SIZE_Y * 0.5));
	m_leave_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_leave_button->m_shape_container.setOutlineThickness(2);
	m_leave_button->m_shape_container.setOutlineColor(sf::Color::White);
	m_leave_button->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x - m_panel->m_shape_container.getSize().x * 0.25, offset_y));

	m_leave_button->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_leave_button->m_text.setCharacterSize(16);
	m_leave_button->m_text.setStyle(sf::Text::Bold);
	m_leave_button->m_text.setColor(sf::Color::White);
	m_leave_button->m_text.setString("Leave");
	m_leave_button->m_text.setPosition(sf::Vector2f(m_leave_button->m_shape_container.getPosition().x - m_leave_button->m_text.getGlobalBounds().width * 0.5, offset_y - m_leave_button->m_text.getCharacterSize() * 0.65));
}

string DockyardInterface::Update(sf::Time deltaTime)
{
	string upgrade_type = "";

	//detail interface for the focused (hovered) item
	bool focus = false;
	for (vector<ShopItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		//update cost's text color if not enough money to buy
		if (m_ship->m_resources[Resource_Gold] < (*it)->m_upgrade->m_cost)
		{
			(*it)->m_cost->m_text.setColor((*CurrentGame).m_dico_colors[Color_Red_Impossible]);
		}
		else
		{
			(*it)->m_cost->m_text.setColor(sf::Color::White);
		}

		//update textore color if required upgrade not owned
		if (m_ship->HasUpgradeRequiredFor((*it)->m_upgrade->m_type) == true)
		{
			(*it)->m_upgrade->m_text.setColor(sf::Color::White);
		}
		else
		{
			(*it)->m_upgrade->m_text.setColor((*CurrentGame).m_dico_colors[Color_Red_Impossible]);
		}

		//hovering
		if ((*it)->m_upgrade->ButtonUpdate() == true)
		{
			//if not already focused, create a detail interface for this item
			if (m_focused_item != (*it)->m_upgrade)
			{
				DestroyDetail();	
				InitDetail((*it)->m_upgrade);
				m_focused_item = (*it)->m_upgrade;
			}

			focus = true;

			//click = faction
			if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
			{
				if (m_ship->BuyUpgrade((*it)->m_upgrade) == true)
				{
					upgrade_type = (*it)->m_upgrade->m_type;//buying upgrade
				}
			}
		}
	}

	//no item focused => not detail interface
	if (focus == false && m_focused_item != NULL)
	{
		DestroyDetail();
	}

	return upgrade_type;
}

void DockyardInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	screen.draw(m_narrative_text);

	m_leave_button->Draw(screen);

	for (vector<ShopItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		(*it)->m_upgrade->Draw(screen);
		(*it)->m_cost->Draw(screen);
	}

	if (m_focused_item != NULL)
	{
		m_detail_panel->Draw(screen);
		screen.draw(m_detail_title);
		screen.draw(m_detail_body);
		screen.draw(m_detail_requirement);
	}
}

void DockyardInterface::InitDetail(Upgrade* upgrade)
{
	m_detail_panel = new GameEntity(UI_None);
	m_detail_panel->m_shape_container.setSize(sf::Vector2f(DOCKYARDINTERFACE_DETAIL_SIZE_X, DOCKYARDINTERFACE_DETAIL_SIZE_Y));
	m_detail_panel->m_shape_container.setOrigin(sf::Vector2f(m_detail_panel->m_shape_container.getSize().x * 0.5, m_detail_panel->m_shape_container.getSize().y * 0.5));
	m_detail_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_detail_panel->m_shape_container.setOutlineThickness(2);
	m_detail_panel->m_shape_container.setOutlineColor(sf::Color::White);
	m_detail_panel->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x + m_detail_panel->m_shape_container.getSize().x * 0.5, m_items[0]->m_upgrade->getPosition().y - m_items[0]->m_upgrade->m_shape_container.getSize().y * 0.5 + m_detail_panel->m_shape_container.getSize().y * 0.5));
	m_detail_panel->m_position = m_detail_panel->m_shape_container.getPosition();

	m_detail_title.setString(upgrade->m_display_name);
	m_detail_title.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_detail_title.setCharacterSize(16);
	m_detail_title.setStyle(sf::Text::Bold);
	m_detail_title.setColor((*CurrentGame).m_dico_colors[Color_Green_System]);
	m_detail_title.setPosition(sf::Vector2f(m_detail_panel->m_position.x - m_detail_panel->m_shape_container.getSize().x * 0.5 + 8, m_detail_panel->m_position.y - m_detail_panel->m_shape_container.getSize().y * 0.5 + 8));

	string description = upgrade->m_description;
	description = StringCut(description, 44);
	m_detail_body.setString(description);
	m_detail_body.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_detail_body.setCharacterSize(16);
	m_detail_body.setStyle(sf::Text::Bold);
	m_detail_body.setColor(sf::Color::White);
	m_detail_body.setPosition(sf::Vector2f(m_detail_panel->m_position.x - m_detail_panel->m_shape_container.getSize().x * 0.5 + 8, m_detail_title.getPosition().y + m_detail_title.getCharacterSize() + 10));

	if (upgrade->m_upgrade_required.compare("0") != 0)
	{
		string requirement_display_name;
		for (vector<vector<string> >::iterator it = (*CurrentGame).m_upgrades_config.begin(); it != (*CurrentGame).m_upgrades_config.end(); it++)
		{
			if ((*it)[Upgrade_ID].compare(upgrade->m_upgrade_required) == 0)
			{
				requirement_display_name = (*it)[Upgrade_Name];
				requirement_display_name = StringReplace(requirement_display_name, "_", " ");
				m_detail_requirement.setString("Upgrade required: " + requirement_display_name);
				m_detail_requirement.setFont(*(*CurrentGame).m_font[Font_Arial]);
				m_detail_requirement.setCharacterSize(16);
				m_detail_requirement.setStyle(sf::Text::Bold);
				m_detail_requirement.setPosition(sf::Vector2f(m_detail_panel->m_position.x - m_detail_panel->m_shape_container.getSize().x * 0.5 + 8, m_detail_body.getPosition().y + m_detail_body.getGlobalBounds().height + 10));
				
				//set color (red = upgrade required not owned)
				bool found = false;
				for (vector<Upgrade*>::iterator it2 = m_ship->m_upgrades.begin(); it2 != m_ship->m_upgrades.end(); it2++)
				{
					if ((*it)[Upgrade_ID].compare((*it2)->m_type) == 0)
					{
						m_detail_requirement.setColor(sf::Color::White);
						found = true;
						break;
					}
				}
				if (found == false)
				{
					m_detail_requirement.setColor((*CurrentGame).m_dico_colors[Color_Red_Impossible]);
				}
				
				break;
			}
		}
	}
}

void DockyardInterface::DestroyDetail()
{
	delete m_detail_panel;
	m_detail_panel = NULL;

	m_detail_title.setString("");
	m_detail_body.setString("");
	m_detail_requirement.setString("");

	m_focused_item = NULL;
}

ShopItem::ShopItem(string upgrade_type)
{
	m_upgrade = new Upgrade(upgrade_type);
	m_cost = new GameEntity(UI_None);
}


ShopItem::~ShopItem()
{
	delete m_upgrade;
	delete m_cost;
}