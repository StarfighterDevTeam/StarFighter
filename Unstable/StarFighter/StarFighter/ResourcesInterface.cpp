#include "ResourcesInterface.h"

extern Game* CurrentGame;

ResourcesInterface::ResourcesInterface()
{
	m_warship = NULL;

	m_panel = NULL;
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		m_resources[i] = NULL;
	}
}

ResourcesInterface::~ResourcesInterface()
{
	delete m_panel;
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		delete m_resources[i];
	}
}

void ResourcesInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		delete m_resources[i];
		m_resources[i] = NULL;
	}
}

void ResourcesInterface::Init(Warship* warship)
{
	m_warship = warship;

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(REF_WINDOW_RESOLUTION_X, RESOURCES_INTERFACE_PANEL_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5f, RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f));
	m_panel->m_position = m_panel->m_shape_container.getPosition();

	//resources stocks
	for (int i = 0; i < 3; i++)
	{
		m_resources[i] = new GameEntity(UI_None);
		m_resources[i]->m_shape_container.setSize(sf::Vector2f(RESOURCES_INTERFACE_SIZE_X, RESOURCES_INTERFACE_SIZE_Y));
		m_resources[i]->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_INTERFACE_SIZE_X * 0.5f, RESOURCES_INTERFACE_SIZE_Y * 0.5f));
		m_resources[i]->m_shape_container.setFillColor(sf::Color::Black);
		m_resources[i]->m_shape_container.setOutlineThickness(2.f);
		m_resources[i]->m_shape_container.setOutlineColor(sf::Color::White);
		m_resources[i]->m_shape_container.setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + RESOURCES_INTERFACE_SIZE_X * 0.5 + i * RESOURCES_INTERFACE_SIZE_X, RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f));
		m_resources[i]->m_position = m_panel->m_shape_container.getPosition();

		sf::Texture* texture;
		switch (i)
		{
			case Resource_Gold:
			{
				texture = TextureLoader::getInstance()->loadTexture("2D/icon_gold.png", RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
				break;
			}
			case Resource_Fish:
			{
				texture = TextureLoader::getInstance()->loadTexture("2D/icon_fish.png", RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
				break;
			}
			case Resource_Mech:
			{
				texture = TextureLoader::getInstance()->loadTexture("2D/icon_mech.png", RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
				break;
			}
			
		}
		m_resources[i]->setAnimation(texture, 1, 1);
		m_resources[i]->setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + i * RESOURCES_INTERFACE_SIZE_X + (RESOURCES_ICON_SIZE * 0.5), RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5));

		m_resources[i]->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_SIZE_Y));
		m_resources[i]->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_SIZE_Y * 0.5f));
		m_resources[i]->m_shape.setFillColor(sf::Color::Black);
		m_resources[i]->m_shape.setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + i * RESOURCES_INTERFACE_SIZE_X + (RESOURCES_ICON_SIZE * 0.5), RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5));
		
		m_resources[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_resources[i]->m_text.setCharacterSize(16);
		m_resources[i]->m_text.setStyle(sf::Text::Bold);
		m_resources[i]->m_text.setColor(sf::Color::White);
		m_resources[i]->m_text.setString("999999");
		m_resources[i]->m_text.setPosition(sf::Vector2f(m_resources[i]->m_shape_container.getPosition().x - RESOURCES_INTERFACE_SIZE_X * 0.5 + RESOURCES_ICON_SIZE + 8, m_resources[i]->m_shape_container.getPosition().y - m_resources[i]->m_text.getCharacterSize() * 0.60));
	}

	//crew fidelity
	m_resources[Resource_Fidelity] = new GameEntity(UI_None);
	m_resources[Resource_Fidelity]->m_shape_container.setSize(sf::Vector2f(RESOURCES_CREWFIDELITY_SIZE_X, RESOURCES_CREWFIDELITY_SIZE_Y));
	m_resources[Resource_Fidelity]->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_CREWFIDELITY_SIZE_X * 0.5f, RESOURCES_CREWFIDELITY_SIZE_Y * 0.5));
	m_resources[Resource_Fidelity]->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Red_CrewFidelity_Negative]);
	m_resources[Resource_Fidelity]->m_shape_container.setOutlineThickness(2.f);
	m_resources[Resource_Fidelity]->m_shape_container.setOutlineColor(sf::Color::White);
	m_resources[Resource_Fidelity]->m_shape_container.setPosition(sf::Vector2f(m_resources[Resource_Fidelity - 1]->m_shape_container.getPosition().x + RESOURCES_INTERFACE_SIZE_X * 0.5 + 24 + RESOURCES_CREWFIDELITY_SIZE_X * 0.5, RESOURCES_CREWFIDELITY_SIZE_Y * 0.5 + 8));//RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f));

	m_resources[Resource_Fidelity]->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_CrewFidelity_Positive]);

	m_resources[Resource_Fidelity]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_resources[Resource_Fidelity]->m_text.setCharacterSize(14);
	m_resources[Resource_Fidelity]->m_text.setStyle(sf::Text::Italic);
	m_resources[Resource_Fidelity]->m_text.setColor(sf::Color::White);

	//days ahead of royal navy
	m_text_royalnavy.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text_royalnavy.setCharacterSize(14);
	m_text_royalnavy.setStyle(sf::Text::Italic);
	m_text_royalnavy.setColor(sf::Color::White);
	m_text_royalnavy.setString("Royal Navy at");
	m_text_royalnavy.setPosition(sf::Vector2f(m_resources[Resource_Fidelity]->m_shape_container.getPosition().x + RESOURCES_CREWFIDELITY_SIZE_X * 0.5 + 30, m_resources[Resource_Fidelity]->m_shape_container.getPosition().y));
	
	m_resources[Resource_Days] = new GameEntity(UI_None);
	m_resources[Resource_Days]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_resources[Resource_Days]->m_text.setCharacterSize(28);
	m_resources[Resource_Days]->m_text.setStyle(sf::Text::Bold);
	m_resources[Resource_Days]->m_text.setColor(sf::Color::White);
	m_resources[Resource_Days]->m_text.setPosition(sf::Vector2f(m_text_royalnavy.getPosition().x + m_text_royalnavy.getGlobalBounds().width + 8, RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f - m_resources[Resource_Days]->m_text.getCharacterSize() * 0.6));
}

void ResourcesInterface::Update()
{
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		ostringstream ss_resource;
		
		switch (i)
		{
			case Resource_Gold:
			{
				ss_resource << m_warship->m_gold;
				m_resources[i]->m_text.setColor(m_warship->m_gold > 0 ? sf::Color::White : sf::Color::Red);
				break;
			}
			case Resource_Fish:
			{
				ss_resource << m_warship->m_fish;
				m_resources[i]->m_text.setColor(m_warship->m_fish > 0 ? sf::Color::White : sf::Color::Red);
				break;
			}
			case Resource_Mech:
			{
				ss_resource << m_warship->m_mech;
				m_resources[i]->m_text.setColor(m_warship->m_mech > 0 ? sf::Color::White : sf::Color::Red);
				break;
			}
			case Resource_Fidelity:
			{
				ss_resource << "Crew fidelity " << (int)(m_warship->m_fidelity * 100) << "%";
				break;
			}
			case Resource_Days:
			{
				ss_resource << m_warship->m_days << " DAYS";
				break;
			}
		}

		m_resources[i]->m_text.setString(ss_resource.str());
	}

	//crew fidelity ratio
	m_resources[Resource_Fidelity]->m_shape.setSize(sf::Vector2f(RESOURCES_CREWFIDELITY_SIZE_X * m_warship->m_fidelity, RESOURCES_CREWFIDELITY_SIZE_Y));
	m_resources[Resource_Fidelity]->m_shape.setOrigin(sf::Vector2f(RESOURCES_CREWFIDELITY_SIZE_X * 0.5f * m_warship->m_fidelity, RESOURCES_CREWFIDELITY_SIZE_Y * 0.5f));
	m_resources[Resource_Fidelity]->m_shape.setPosition(sf::Vector2f(m_resources[Resource_Fidelity]->m_shape_container.getPosition().x - RESOURCES_CREWFIDELITY_SIZE_X * 0.5f + m_resources[Resource_Fidelity]->m_shape.getSize().x * 0.5, m_resources[Resource_Fidelity]->m_shape_container.getPosition().y));

	m_resources[Resource_Fidelity]->m_text.setPosition(sf::Vector2f(m_resources[Resource_Fidelity]->m_shape_container.getPosition().x - m_resources[Resource_Fidelity]->m_text.getGlobalBounds().width * 0.5, m_resources[Resource_Fidelity]->m_shape_container.getPosition().y + RESOURCES_CREWFIDELITY_SIZE_Y * 0.5 + 2));
}

void ResourcesInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);

	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		m_resources[i]->Draw(screen);
	}

	screen.draw(m_text_royalnavy);
}