#include "SFContextInfoPanel.h"

extern Game* CurrentGame;

SFContextInfoPanel::SFContextInfoPanel(sf::Vector2f size, SFPanelTypes panel_type, Player* player) : SFPanel(size, panel_type)
{
	m_player = player;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts strings
	ostringstream ss_title;
	StockEntity* entity = NULL;
	if (player->m_hovered_object)
	{
		entity = (StockEntity*)player->m_hovered_object;
		if (entity->m_identified)
		{
			ss_title << entity->m_display_name;
		}
		else
		{
			ss_title << "???";
		}
		
	}
	m_unit = entity;

	if (!m_unit)
	{
		if (player->m_is_asking_SFContextPanel_string.compare("Scout") == 0)
		{
			sf::Vector2u zone_index = (*CurrentGame).m_stellarmap->GetZoneIndex(player->m_mouse_pos);
			ss_title << "Zone " << zone_index.x << "_" << zone_index.y;
		}
		else
		{
			return;
		}	
	}
	
	m_title_text.setString(ss_title.str());

	//text
	ostringstream ss_text;
	ss_text << player->m_is_asking_SFContextPanel_string;

	if (player->m_selected_object->m_collider_type == StarshipObject)
	{
		Starship* starship = (Starship*)player->m_selected_object;

		if (!entity)
		{
			if (!starship->CheckIfEnoughFuelToDestination(player->m_mouse_pos))
			{
				ss_text << " (insufficient fuel: " << starship->GetPropulsionAvailable() << " / " << starship->GetFuelCostToDestination(player->m_mouse_pos) << " required)";
				m_text.setColor(sf::Color(255, 0, 0, 255));
			}
			else
			{
				size_t fuel_cost = starship->GetFuelCostToDestination(player->m_mouse_pos);
				if (fuel_cost > 0)
					ss_text << " (fuel cost: )" << fuel_cost;
			}
		}
		else if (entity->m_collider_type == LocationObject)
		{
			Location* location = (Location*)entity;
			if (!starship->CheckIfEnoughFuelToDestination(location->getPosition()))
			{
				ss_text << " (insufficient fuel: " << starship->GetPropulsionAvailable() << " / " << starship->GetFuelCostToDestination(location->getPosition()) << " required)";
				m_text.setColor(sf::Color(255, 0, 0, 255));
			}
			else
			{
				size_t fuel_cost = starship->GetFuelCostToDestination(location->getPosition());
				if (fuel_cost > 0)
					ss_text << " (fuel cost: " << fuel_cost << ")";
			}
		}
	}
	
	m_text.setString(ss_text.str());


	//options texts
	//m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	//m_actions->SetString("Enter", ActionButton_A);
	//m_actions->SetString("Hazard down", ActionButton_Y);
	//m_actions->SetString("Hazard up", ActionButton_X);
	//
	//m_actions_with_selection = new SFActionBoxWithSelection();
	//for (int i = 0; i < 4; i++)
	//{
	//	stringstream ss;
	//	ss << "Action " << i;
	//	m_actions_with_selection->AddOption(ss.str(), (*CurrentGame).m_font[Font_Arial]);
	//}

	sf::Vector2f position = sf::Vector2f(size.x / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + player->getPosition().x, size.y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + player->getPosition().y);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	//m_title_text.setPosition(m_unit->getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, m_unit->getPosition().y - getSize().y / 2 + text_height);

	//texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	//m_text.setPosition(sf::Vector2f(m_unit->getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, m_unit->getPosition().y - getSize().y / 2 + text_height));

	//options texts
	//text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	//m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//dynamic size
	sf::Vector2f target_position = (m_unit ? m_unit->getPosition() : player->m_mouse_pos) - sf::Vector2f(0, 120.f);
	text_height += INTERACTION_INTERBLOCK + m_text.getGlobalBounds().height;
	setSize(sf::Vector2f(getSize().x, text_height));
	setOrigin(sf::Vector2f(getOrigin().x, text_height / 2));
	sf::Vector2f position_new = sf::Vector2f(target_position.x, target_position.y);
	setPosition(position_new.x, position_new.y);

	//texts' placement  (recursively, now that we know the panel's actual size)
	text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(target_position.x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, target_position.y - getSize().y / 2 + text_height);

	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	m_text.setPosition(sf::Vector2f(target_position.x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, target_position.y - getSize().y / 2 + text_height));
}

SFContextInfoPanel::~SFContextInfoPanel()
{
	
}

void SFContextInfoPanel::Update(sf::Time deltaTime)
{
	
}

void SFContextInfoPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		screen.draw(m_text);

		//if (m_actions)
		//{
		//	m_actions->Draw(screen);
		//}
	}
}

GameObject* SFContextInfoPanel::GetUnit()
{
	return m_unit;
}