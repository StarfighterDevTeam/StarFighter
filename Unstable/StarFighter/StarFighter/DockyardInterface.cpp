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

	for (vector<GameEntity*>::iterator it = m_items.begin(); it != m_items.end(); it++)
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
	string ss_narrative = "Welcome to your dockyard.\nPlease suit yourself and buy all what you need for your ship.";
	
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
	for (int i = 0; i < 2; i++)
	{
		GameEntity* item = new GameEntity(UI_ShopItem);
		item->m_shape_container.setSize(sf::Vector2f(SHOP_ITEM_CONTAINER_SIZE_X, SHOP_ITEM_CONTAINER_SIZE_Y));
		item->m_shape_container.setOrigin(sf::Vector2f(item->m_shape_container.getSize().x * 0.5, item->m_shape_container.getSize().y * 0.5));
		item->m_shape_container.setFillColor(sf::Color::Black);
		item->m_shape_container.setOutlineThickness(2);
		item->m_shape_container.setOutlineColor(sf::Color::White);
		item->m_shape_container.setPosition(sf::Vector2f(m_narrative_text.getPosition().x + item->m_shape_container.getSize().x * 0.5, offset_y + SHOP_ITEM_CONTAINER_SIZE_Y * 0.5));

		item->m_shape.setSize(sf::Vector2f(UPGRADE_ICON_SIZE, UPGRADE_ICON_SIZE));
		item->m_shape.setOrigin(sf::Vector2f(item->m_shape.getSize().x * 0.5, item->m_shape.getSize().y * 0.5));
		item->m_shape.setFillColor(sf::Color::Black);
		item->m_shape.setOutlineThickness(2);
		item->m_shape.setOutlineColor(sf::Color::White);
		item->m_shape.setPosition(sf::Vector2f(m_narrative_text.getPosition().x + item->m_shape.getSize().x * 0.5 + 8, item->m_shape_container.getPosition().y));

		Upgrade* sonar;
		if (i == 0)
			sonar = new Upgrade(Upgrade_SonarI);
		else if (i == 1)
			sonar = new Upgrade(Upgrade_SonarII);

		string texture_name = sonar->m_texture_name;
		Texture* texture = TextureLoader::getInstance()->loadTexture(texture_name, UPGRADE_ICON_SIZE, UPGRADE_ICON_SIZE);
		string display_name = sonar->m_display_name;
		delete sonar;
		item->setAnimation(texture, 1, 1);
		item->setPosition(item->m_shape.getPosition());

		item->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		item->m_text.setCharacterSize(20);
		item->m_text.setStyle(sf::Text::Bold);
		item->m_text.setColor(sf::Color::White);
		item->m_text.setString(display_name);
		item->m_text.setPosition(sf::Vector2f(item->m_shape.getPosition().x + item->m_shape.getSize().x * 0.5 + 12, item->getPosition().y - item->m_text.getCharacterSize() * 0.65));

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

void DockyardInterface::Update(sf::Time deltaTime)
{
	//leave button's hovering feedback
	m_leave_button->ButtonUpdate();

	//detail interface for the focused (hovered) item
	bool focus = false;
	for (vector<GameEntity*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		if ((*it)->ButtonUpdate() == true)
		{
			//if not already focused, create a detail interface for this item
			if (m_focused_item != *it)
			{
				DestroyDetail();	
				InitDetail(Upgrade_SonarI);
				m_focused_item = *it;
			}

			focus = true;
		}
	}

	//no item focused => not detail interface
	if (focus == false && m_focused_item != NULL)
	{
		DestroyDetail();
	}
}

void DockyardInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	screen.draw(m_narrative_text);

	m_leave_button->Draw(screen);

	for (vector<GameEntity*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		(*it)->Draw(screen);
	}

	if (m_focused_item != NULL)
	{
		m_detail_panel->Draw(screen);
		screen.draw(m_detail_title);
		screen.draw(m_detail_body);
	}
}

void DockyardInterface::InitDetail(UpgradeType upgrade)
{
	m_detail_panel = new GameEntity(UI_None);
	m_detail_panel->m_shape_container.setSize(sf::Vector2f(DOCKYARDINTERFACE_DETAIL_SIZE_X, DOCKYARDINTERFACE_DETAIL_SIZE_Y));
	m_detail_panel->m_shape_container.setOrigin(sf::Vector2f(m_detail_panel->m_shape_container.getSize().x * 0.5, m_detail_panel->m_shape_container.getSize().y * 0.5));
	m_detail_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_detail_panel->m_shape_container.setOutlineThickness(2);
	m_detail_panel->m_shape_container.setOutlineColor(sf::Color::White);
	m_detail_panel->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x + m_detail_panel->m_shape_container.getSize().x * 0.5, m_items[0]->getPosition().y - m_items[0]->m_shape_container.getSize().y * 0.5 + m_detail_panel->m_shape_container.getSize().y * 0.5));
	m_detail_panel->m_position = m_detail_panel->m_shape_container.getPosition();

	m_detail_title.setString("TITRE");
	m_detail_title.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_detail_title.setCharacterSize(16);
	m_detail_title.setStyle(sf::Text::Bold);
	m_detail_title.setColor(sf::Color::Green);
	m_detail_title.setPosition(sf::Vector2f(m_detail_panel->m_position.x - m_detail_panel->m_shape_container.getSize().x * 0.5 + 8, m_detail_panel->m_position.y - m_detail_panel->m_shape_container.getSize().y * 0.5 + 8));

	m_detail_body.setString("Texte\navec\ndes lignes");
	m_detail_body.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_detail_body.setCharacterSize(16);
	m_detail_body.setStyle(sf::Text::Bold);
	m_detail_body.setColor(sf::Color::White);
	m_detail_body.setPosition(sf::Vector2f(m_detail_panel->m_position.x - m_detail_panel->m_shape_container.getSize().x * 0.5 + 8, m_detail_title.getPosition().y + m_detail_title.getCharacterSize() + 10));
}

void DockyardInterface::DestroyDetail()
{
	delete m_detail_panel;
	m_detail_panel = NULL;

	m_detail_title.setString("");
	m_detail_body.setString("");

	m_focused_item = NULL;
}