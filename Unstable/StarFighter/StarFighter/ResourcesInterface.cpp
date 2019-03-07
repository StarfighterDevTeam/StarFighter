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

		m_resources[i]->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_SIZE_Y));
		m_resources[i]->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_SIZE_Y * 0.5f));
		m_resources[i]->m_shape.setFillColor(sf::Color::Red);
		m_resources[i]->m_shape.setPosition(sf::Vector2f(RESOURCES_INTERFACE_OFFSET_X + i * RESOURCES_INTERFACE_SIZE_X + (4 + RESOURCES_ICON_SIZE * 0.5), RESOURCES_INTERFACE_PANEL_SIZE_Y * 0.5f));

		m_resources[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_resources[i]->m_text.setCharacterSize(16);
		m_resources[i]->m_text.setStyle(sf::Text::Bold);
		m_resources[i]->m_text.setColor(sf::Color::White);
		m_resources[i]->m_text.setString("999999");
		m_resources[i]->m_text.setPosition(sf::Vector2f(m_resources[i]->m_shape.getPosition().x + RESOURCES_ICON_SIZE * 0.5 + 6, m_resources[i]->m_shape.getPosition().y - m_resources[i]->m_text.getCharacterSize() * 0.60));
	}
}

void ResourcesInterface::Update()
{
	for (int i = 0; i < 3; i++)
	{
		ostringstream ss_resource;
		
		switch (i)
		{
			case Resource_Gold:
			{
				ss_resource << m_warship->m_gold;
				break;
			}
			case Resource_Fish:
			{
				ss_resource << m_warship->m_fish;
				break;
			}
			case Resource_Mech:
			{
				ss_resource << m_warship->m_mech;
				break;
			}
		}

		m_resources[i]->m_text.setString(ss_resource.str());
	}
}

void ResourcesInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);

	for (int i = 0; i < 3; i++)
	{
		m_resources[i]->Draw(screen);
	}
}