#include "ResourcesInterface.h"

extern Game* CurrentGame;

ResourcesInterface::ResourcesInterface()
{
	m_warship = NULL;
	m_save_button = NULL;
	m_load_button = NULL;

	m_panel = NULL;
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		m_resources[i] = NULL;
		m_resources_container[i] = NULL;
	}

	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		m_upkeep[i] = NULL;
		m_upkeep_container[i] = NULL;
	}
}

ResourcesInterface::~ResourcesInterface()
{
	delete m_panel;
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		delete m_resources[i];
		delete m_resources_container[i];
	}

	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		delete m_upkeep[i];
		delete m_upkeep_container[i];
	}

	delete m_save_button;
	delete m_load_button;
}

void ResourcesInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		delete m_resources[i];
		m_resources[i] = NULL;

		delete m_resources_container[i];
		m_resources_container[i] = NULL;
	}

	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		delete m_upkeep[i];
		m_upkeep[i] = NULL;

		delete m_upkeep_container[i];
		m_upkeep_container[i] = NULL;
	}

	delete m_save_button;
	m_save_button = NULL;

	delete m_load_button;
	m_load_button = NULL;
}

void ResourcesInterface::Init(Warship* warship)
{
	m_warship = warship;

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X, RESOURCES_INTERFACE_PANEL_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5f, RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5));
	m_panel->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();

	//resources stocks
	for (int i = (int)Resource_Gold; i <= (int)Resource_Mech; i++)
	{
		//resource container
		m_resources_container[i] = new GameEntity(UI_None);
		m_resources_container[i]->m_shape_container.setSize(sf::Vector2f(RESOURCES_INTERFACE_STOCK_SIZE_X, RESOURCES_INTERFACE_STOCK_SIZE_Y));
		m_resources_container[i]->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5, RESOURCES_INTERFACE_STOCK_SIZE_Y * 0.5));
		m_resources_container[i]->m_shape_container.setFillColor(sf::Color::Black);
		m_resources_container[i]->m_shape_container.setOutlineThickness(2);
		m_resources_container[i]->m_shape_container.setOutlineColor(sf::Color::White);
		m_resources_container[i]->m_shape_container.setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5 + i * RESOURCES_INTERFACE_STOCK_SIZE_X, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5));

		m_resources_container[i]->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_STOCK_SIZE_Y));
		m_resources_container[i]->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_STOCK_SIZE_Y * 0.5f));
		m_resources_container[i]->m_shape.setFillColor(sf::Color::Black);
		m_resources_container[i]->m_shape.setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + i * RESOURCES_INTERFACE_STOCK_SIZE_X + (RESOURCES_ICON_SIZE * 0.5), RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5));
		
		m_resources_container[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_resources_container[i]->m_text.setCharacterSize(16);
		m_resources_container[i]->m_text.setStyle(sf::Text::Bold);
		m_resources_container[i]->m_text.setColor(sf::Color::White);
		m_resources_container[i]->m_text.setString("999999");
		m_resources_container[i]->m_text.setPosition(sf::Vector2f(m_resources_container[i]->m_shape_container.getPosition().x - RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5 + RESOURCES_ICON_SIZE + 8, m_resources_container[i]->m_shape_container.getPosition().y - m_resources_container[i]->m_text.getCharacterSize() * 0.60));
	
		//resource icon
		sf::Texture* texture = TextureLoader::getInstance()->loadTexture((*CurrentGame).m_dico_resources_textures[i], RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
		m_resources[i] = new Resource((ResourceType)i, UI_Resource);
		m_resources[i]->setAnimation(texture, 1, 1);
		m_resources[i]->setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + i * RESOURCES_INTERFACE_STOCK_SIZE_X + (RESOURCES_ICON_SIZE * 0.5), RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5));
		m_resources[i]->m_position = m_resources[i]->getPosition();
	}

	//crew fidelity
	m_resources_container[Resource_Fidelity] = new GameEntity(UI_Resource);
	m_resources_container[Resource_Fidelity]->m_shape_container.setSize(sf::Vector2f(RESOURCES_CREWFIDELITY_SIZE_X, RESOURCES_CREWFIDELITY_SIZE_Y));
	m_resources_container[Resource_Fidelity]->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_CREWFIDELITY_SIZE_X * 0.5f, RESOURCES_CREWFIDELITY_SIZE_Y * 0.5));
	m_resources_container[Resource_Fidelity]->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Red_CrewFidelity_Negative]);
	m_resources_container[Resource_Fidelity]->m_shape_container.setOutlineThickness(2.f);
	m_resources_container[Resource_Fidelity]->m_shape_container.setOutlineColor(sf::Color::White);
	m_resources_container[Resource_Fidelity]->m_shape_container.setPosition(sf::Vector2f(m_resources_container[Resource_Fidelity - 1]->m_shape_container.getPosition().x + RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5 + 24 + RESOURCES_CREWFIDELITY_SIZE_X * 0.5, RESOURCES_CREWFIDELITY_SIZE_Y * 0.5 + 8));//RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f));

	m_resources_container[Resource_Fidelity]->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_CrewFidelity_Positive]);

	m_resources_container[Resource_Fidelity]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_resources_container[Resource_Fidelity]->m_text.setCharacterSize(14);
	m_resources_container[Resource_Fidelity]->m_text.setStyle(sf::Text::Italic);
	m_resources_container[Resource_Fidelity]->m_text.setColor(sf::Color::White);

	m_resources[Resource_Fidelity] = new Resource(Resource_Fidelity, UI_Resource);
	m_resources[Resource_Fidelity]->m_shape_container.setSize(m_resources_container[Resource_Fidelity]->m_shape_container.getSize());
	m_resources[Resource_Fidelity]->m_shape_container.setOrigin(m_resources_container[Resource_Fidelity]->m_shape_container.getOrigin());
	m_resources[Resource_Fidelity]->m_shape_container.setPosition(m_resources_container[Resource_Fidelity]->m_shape_container.getPosition());
	m_resources[Resource_Fidelity]->m_position = m_resources[Resource_Fidelity]->m_shape_container.getPosition();

	//days ahead of royal navy
	m_text_royalnavy.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text_royalnavy.setCharacterSize(14);
	m_text_royalnavy.setStyle(sf::Text::Italic);
	m_text_royalnavy.setColor(sf::Color::White);
	m_text_royalnavy.setString("Royal Navy at");
	m_text_royalnavy.setPosition(sf::Vector2f(m_resources_container[Resource_Fidelity]->m_shape_container.getPosition().x + RESOURCES_CREWFIDELITY_SIZE_X * 0.5 + 30, m_resources_container[Resource_Fidelity]->m_shape_container.getPosition().y));
	
	m_resources_container[Resource_Days] = new GameEntity(UI_None);
	m_resources_container[Resource_Days]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_resources_container[Resource_Days]->m_text.setCharacterSize(28);
	m_resources_container[Resource_Days]->m_text.setStyle(sf::Text::Bold);
	m_resources_container[Resource_Days]->m_text.setColor(sf::Color::White);
	m_resources_container[Resource_Days]->m_text.setPosition(sf::Vector2f(m_text_royalnavy.getPosition().x + m_text_royalnavy.getGlobalBounds().width + 8, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5f - m_resources_container[Resource_Days]->m_text.getCharacterSize() * 0.6));
	m_resources_container[Resource_Days]->m_text.setString("99 DAYS");

	m_resources[Resource_Days] = new Resource(Resource_Days, UI_Resource);
	m_resources[Resource_Days]->m_shape_container.setSize(sf::Vector2f(m_resources_container[Resource_Days]->m_text.getGlobalBounds().width, m_resources_container[Resource_Days]->m_text.getCharacterSize()));
	m_resources[Resource_Days]->m_shape_container.setOrigin(sf::Vector2f(m_resources[Resource_Days]->m_shape_container.getSize().x * 0.5, m_resources[Resource_Days]->m_shape_container.getSize().y * 0.5));
	m_resources[Resource_Days]->m_shape_container.setPosition(sf::Vector2f(m_resources_container[Resource_Days]->m_text.getPosition().x + m_resources_container[Resource_Days]->m_text.getGlobalBounds().width * 0.5, m_resources_container[Resource_Days]->m_text.getPosition().y + m_resources_container[Resource_Days]->m_text.getCharacterSize() * 0.5));
	m_resources[Resource_Days]->m_position = m_resources[Resource_Days]->m_shape_container.getPosition();

	//upkeep
	m_text_upkeep.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text_upkeep.setCharacterSize(14);
	//m_text_upkeep.setStyle(sf::Text::Italic);
	m_text_upkeep.setColor(sf::Color::White);
	m_text_upkeep.setString("Daily crew upkeep");
	m_text_upkeep.setPosition(sf::Vector2f(m_text_royalnavy.getPosition().x + m_text_royalnavy.getGlobalBounds().width + 8 + m_text_upkeep.getGlobalBounds().width + 40, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5f - m_text_upkeep.getCharacterSize() * 0.6));

	for (int i = (int)Resource_Gold; i <= (int)Resource_Fish; i++)
	{
		m_upkeep_container[i] = new GameEntity(UI_None);
		m_upkeep_container[i]->m_shape_container.setSize(sf::Vector2f(RESOURCES_INTERFACE_STOCK_SIZE_X, RESOURCES_INTERFACE_STOCK_SIZE_Y));
		m_upkeep_container[i]->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5, RESOURCES_INTERFACE_STOCK_SIZE_Y * 0.5));
		m_upkeep_container[i]->m_shape_container.setFillColor(sf::Color::Black);
		m_upkeep_container[i]->m_shape_container.setOutlineThickness(2);
		m_upkeep_container[i]->m_shape_container.setOutlineColor(sf::Color::White);
		m_upkeep_container[i]->m_shape_container.setPosition(sf::Vector2f(sf::Vector2f(m_text_upkeep.getPosition().x + m_text_upkeep.getGlobalBounds().width + RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5 + i * RESOURCES_INTERFACE_STOCK_SIZE_X + 16, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5)));
		m_upkeep_container[i]->m_position = m_upkeep_container[i]->m_shape_container.getPosition();

		m_upkeep_container[i]->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_STOCK_SIZE_Y));
		m_upkeep_container[i]->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_STOCK_SIZE_Y * 0.5f));
		m_upkeep_container[i]->m_shape.setFillColor(sf::Color::Black);
		m_upkeep_container[i]->m_shape.setPosition(m_upkeep_container[i]->getPosition());

		m_upkeep_container[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_upkeep_container[i]->m_text.setCharacterSize(16);
		m_upkeep_container[i]->m_text.setStyle(sf::Text::Bold);
		m_upkeep_container[i]->m_text.setColor(sf::Color::White);
		m_upkeep_container[i]->m_text.setString("999999");
		m_upkeep_container[i]->m_text.setPosition(sf::Vector2f(m_upkeep_container[i]->m_shape_container.getPosition().x - RESOURCES_INTERFACE_STOCK_SIZE_X * 0.5 + RESOURCES_ICON_SIZE + 8, m_resources_container[i]->m_shape_container.getPosition().y - m_resources_container[i]->m_text.getCharacterSize() * 0.60));
	
		sf::Texture* texture = TextureLoader::getInstance()->loadTexture((*CurrentGame).m_dico_resources_textures[i], RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
		m_upkeep[i] = new Resource((ResourceType)i, UI_ResourceUpkeep);
		m_upkeep[i]->setAnimation(texture, 1, 1);
		m_upkeep[i]->setPosition(sf::Vector2f(sf::Vector2f(m_text_upkeep.getPosition().x + m_text_upkeep.getGlobalBounds().width + RESOURCES_ICON_SIZE * 0.5 + i * RESOURCES_INTERFACE_STOCK_SIZE_X + 16, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5)));
		m_upkeep[i]->m_position = m_upkeep[i]->getPosition();
	}

	//holds
	m_text_holds.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text_holds.setCharacterSize(16);
	m_text_holds.setStyle(sf::Text::Bold);
	m_text_holds.setColor(sf::Color::White);
	m_text_holds.setString("Holds:");
	m_text_holds.setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y + (RESOURCES_INTERFACE_PANEL_SIZE_Y - RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y) * 0.5 - m_text_holds.getCharacterSize() * 0.65));

	//save & load buttons
	m_save_button = new GameEntity(UI_None);
	m_save_button->m_shape_container.setSize(sf::Vector2f(RESOURCES_BUTTON_SIZE_X, RESOURCES_BUTTON_SIZE_Y));
	m_save_button->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_BUTTON_SIZE_X * 0.5, RESOURCES_BUTTON_SIZE_Y * 0.5));
	m_save_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_save_button->m_shape_container.setOutlineThickness(2);
	m_save_button->m_shape_container.setOutlineColor(sf::Color::White);
	m_save_button->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - (2 * (RESOURCES_BUTTON_SIZE_X + 8)), RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5));

	m_save_button->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_save_button->m_text.setCharacterSize(16);
	m_save_button->m_text.setStyle(sf::Text::Bold);
	m_save_button->m_text.setColor(sf::Color::White);
	m_save_button->m_text.setString("Save (F4)");
	m_save_button->m_text.setPosition(sf::Vector2f(m_save_button->m_shape_container.getPosition().x - m_save_button->m_text.getGlobalBounds().width * 0.5, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5 - m_save_button->m_text.getCharacterSize() * 0.6));

	m_load_button = new GameEntity(UI_None);
	m_load_button->m_shape_container.setSize(sf::Vector2f(RESOURCES_BUTTON_SIZE_X, RESOURCES_BUTTON_SIZE_Y));
	m_load_button->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_BUTTON_SIZE_X * 0.5, RESOURCES_BUTTON_SIZE_Y * 0.5));
	m_load_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_load_button->m_shape_container.setOutlineThickness(2);
	m_load_button->m_shape_container.setOutlineColor(sf::Color::White);
	m_load_button->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - (RESOURCES_BUTTON_SIZE_X + 8), RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5));

	m_load_button->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_load_button->m_text.setCharacterSize(16);
	m_load_button->m_text.setStyle(sf::Text::Bold);
	m_load_button->m_text.setColor(sf::Color::White);
	m_load_button->m_text.setString("Reload");
	m_load_button->m_text.setPosition(sf::Vector2f(m_load_button->m_shape_container.getPosition().x - m_load_button->m_text.getGlobalBounds().width * 0.5, RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y * 0.5 - m_load_button->m_text.getCharacterSize() * 0.6));
}

void ResourcesInterface::Update()
{
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		ostringstream ss_resource;
		
		switch (i)
		{
			case Resource_Gold:
			case Resource_Fish:
			case Resource_Mech:
			{
				ss_resource << m_warship->m_resources[i];
				//m_resources[i]->m_text.setColor(m_warship->m_resources[i] > 0 ? sf::Color::White : sf::Color::Red);
				break;
			}
			case Resource_Fidelity:
			{
				ss_resource << "Crew fidelity " << m_warship->m_resources[i] << "%";
				break;
			}
			case Resource_Days:
			{
				ss_resource << m_warship->m_resources[i] << " DAYS";
				break;
			}
		}

		m_resources_container[i]->m_text.setString(ss_resource.str());

		m_resources[i]->GetHoveredState();
	}

	//crew fidelity ratio
	m_resources_container[Resource_Fidelity]->m_shape.setSize(sf::Vector2f(1.0f * RESOURCES_CREWFIDELITY_SIZE_X * m_warship->m_resources[Resource_Fidelity] / 100, RESOURCES_CREWFIDELITY_SIZE_Y));
	m_resources_container[Resource_Fidelity]->m_shape.setOrigin(sf::Vector2f(m_resources_container[Resource_Fidelity]->m_shape.getSize().x * 0.5, RESOURCES_CREWFIDELITY_SIZE_Y * 0.5));
	m_resources_container[Resource_Fidelity]->m_shape.setPosition(sf::Vector2f(m_resources_container[Resource_Fidelity]->m_shape_container.getPosition().x - RESOURCES_CREWFIDELITY_SIZE_X * 0.5 + m_resources_container[Resource_Fidelity]->m_shape.getSize().x * 0.5, m_resources_container[Resource_Fidelity]->m_shape_container.getPosition().y));
	m_resources_container[Resource_Fidelity]->m_text.setPosition(sf::Vector2f(m_resources_container[Resource_Fidelity]->m_shape_container.getPosition().x - m_resources_container[Resource_Fidelity]->m_text.getGlobalBounds().width * 0.5, m_resources_container[Resource_Fidelity]->m_shape_container.getPosition().y + RESOURCES_CREWFIDELITY_SIZE_Y * 0.5 + 2));
	
	m_resources[Resource_Fidelity]->GetHoveredState();

	//upkeep costs
	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		ostringstream ss_resource;

		switch (i)
		{
			case Resource_Gold:
			case Resource_Fish:
			{
				ss_resource << "-" << m_warship->m_upkeep_costs[i];
				m_upkeep_container[i]->m_text.setColor(m_warship->m_upkeep_costs[i] > 0 ? sf::Color::Red : sf::Color::White);
				break;
			}
		}

		m_upkeep_container[i]->m_text.setString(ss_resource.str());

		m_upkeep[i]->GetHoveredState();
	}

	//holds
	int k = 0;
	for (vector<Commodity*>::iterator it = m_warship->m_holds.begin(); it != m_warship->m_holds.end(); it++)
	{
		(*it)->setPosition(m_text_holds.getPosition().x + m_text_holds.getGlobalBounds().width + 10 + RESOURCES_ICON_SIZE * 0.5 + (k * (RESOURCES_ICON_SIZE + 8)), RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y + (RESOURCES_INTERFACE_PANEL_SIZE_Y - RESOURCES_INTERFACE_UPPER_PANEL_SIZE_Y) * 0.5);
		(*it)->m_position = (*it)->getPosition();
		(*it)->GetHoveredState();
		k++;
	}

	//save & load buttons' hovering feedback
	m_save_button->ButtonUpdate();
	m_load_button->ButtonUpdate();
}

void ResourcesInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);

	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		m_resources_container[i]->Draw(screen);
		if (i != Resource_Fidelity && i != Resource_Days)
		{
			m_resources[i]->Draw(screen);
		}
	}

	for (int i = 0; i < NB_UPKEEP_COSTS; i++)
	{
		m_upkeep_container[i]->Draw(screen);
		m_upkeep[i]->Draw(screen);
	}
	screen.draw(m_text_upkeep);

	screen.draw(m_text_royalnavy);

	m_save_button->Draw(screen);
	m_load_button->Draw(screen);

	screen.draw(m_text_holds);

	for (vector<Commodity*>::iterator it = m_warship->m_holds.begin(); it != m_warship->m_holds.end(); it++)
	{
		(*it)->Draw(screen);
	}
}