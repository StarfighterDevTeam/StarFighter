#include "SFUnitInfoPanel.h"

extern Game* CurrentGame;

SFUnitInfoPanel::SFUnitInfoPanel(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	/*m_playerShip = playerShip;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts strings
	ostringstream ss_title;
	StockEntity* entity = NULL;
	if (playerShip->m_hovered_object)
	{
		entity = (StockEntity*)playerShip->m_hovered_object;
		if (entity->m_identified)
		{
			ss_title << entity->m_display_name;
		}
		else
		{
			ss_title << "???";
		}
		
	}
	else if (playerShip->m_selected_object)
	{
		entity = (StockEntity*)playerShip->m_selected_object;
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
		return;
	}
	
	m_title_text.setString(ss_title.str());

	//text
	ostringstream ss_text;

	if (!entity->m_identified)
	{
		ss_text << "Unknown object";
	}
	else
	{
		ostringstream ss_ore;
		string contains_ore;
		ostringstream ss_fuel;
		string contains_fuel;

		//ore stocked
		for (map<string, size_t>::iterator i = entity->m_ore_stock.begin(); i != entity->m_ore_stock.end(); i++)
		{
			ss_ore << "\n" << i->first << ": " << i->second;
		}
		contains_ore = ss_ore.str();
		if (!contains_ore.empty())
		{
			ss_text << "Ore: " << entity->m_current_ore_stock << " / " << entity->GetOreMax() << contains_ore;
		}

		//fuel
		if (entity->m_collider_type == StarshipObject)
		{
			Starship* starship = (Starship*)entity;
			for (map<string, size_t>::iterator i = entity->m_fuel_stock.begin(); i != entity->m_fuel_stock.end(); i++)
			{
				//ss_fuel << "\n" << i->first << ": " << i->second + starship->m_fuel_assigned[i->first];
			}
		}
		else
		{
			for (map<string, size_t>::iterator i = entity->m_fuel_stock.begin(); i != entity->m_fuel_stock.end(); i++)
			{
				ss_fuel << "\n" << i->first << ": " << i->second;
			}
		}

		contains_fuel = ss_fuel.str();
		if (!contains_fuel.empty())
		{
			if (!contains_ore.empty())
			{
				ss_text << "\n\n";
			}
			ss_text << "Fuel: " << entity->m_current_fuel_stock << " / " << entity->GetFuelMax() << contains_fuel;
		}

		if (entity->m_collider_type == StarshipObject)
		{
			Starship* starship = (Starship*)entity;
			//ss_text << "\nPropulsion available: " << starship->GetPropulsionAvailable();
		}
		
		//drill rates
		if (entity->m_collider_type == LocationObject)
		{
			ss_text << "\n\nRessources:";
			for (map<string, float>::iterator i = entity->m_ore_presence_rates.begin(); i != entity->m_ore_presence_rates.end(); i++)
			{
				ss_text << "\n" << i->first << ": " << i->second * 100 << ("%");
			}
		}

		//buildings
		if (entity->m_collider_type == LocationObject && entity->CanSupplyFuel())
		{
			Planet* planet = (Planet*)entity;
			size_t buildingsVectorSize = planet->m_buildings.size();
			ss_text << "\n\nBuildings: " << buildingsVectorSize << "/" << planet->m_building_slots;
			for (size_t i = 0; i < buildingsVectorSize; i++)
			{
				if (planet->m_buildings[i])
				{
					ss_text << "\n" << planet->m_buildings[i]->m_display_name;
				}
			}
		}

		//productions
		if (entity->m_collider_type == LocationObject && entity->CanSupplyFuel())
		{
			Planet* planet = (Planet*)entity;
			ss_text << "\n\nProductions:";
			for (map<string, size_t>::iterator i = planet->m_starship_productions.begin(); i != planet->m_starship_productions.end(); i++)
			{
				ss_text << "\n" << i->first << " (level " << i->second << ")";
			}
		}

		//ship stats
		if (entity->m_collider_type == StarshipObject)
		{
			Starship* starship = (Starship*)entity;
			if (starship->m_nb_drills > 0)
			{
				ss_text << "\n\nDrill attempts: " << starship->m_current_drill_attempts << "/" << starship->m_nb_drills;
			}
			ss_text << "\nStatus: ";
			switch (starship->m_state)
			{
				case StarshipState_Idle:
				{
					if (starship->m_current_destination)
					{
						ss_text << "waiting for fuel";
					}
					else
					{
						ss_text << "waiting for orders";
					}

					break;
				}
				case StarshipState_Drilling:
				{
					ss_text << "drilling...";
					break;
				}
				case StarshipState_Searching:
				{
					ss_text << "searching drill location";
					break;
				}
				case StarshipState_Unloading:
				{
					ss_text << "unloading";
					break;
				}
				case StarshipState_MovingToLocation:
				{
					ss_text << "moving to location";
					break;
				}
				case StarshipState_Extracting:
				{
					ss_text << "extracting: " << starship->m_ore_found->m_display_name;
					break;
				}
				case StarshipState_Scanning:
				{
					ss_text.precision(4);
					ss_text << "scanning: " << starship->m_scan_clock.getElapsedTime().asSeconds() / SCAN_DURATION * 100 << "%";
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	//distance to destination
	if (playerShip->m_selected_object && playerShip->m_selected_object->m_collider_type == StarshipObject)
	{
		Starship* starship = (Starship*)playerShip->m_selected_object;
		GameObject* destination = entity != starship && entity->m_collider_type == LocationObject ? (GameObject*)entity : starship->m_current_destination;

		size_t distance = GameObject::GetLightYearsBetweenObjects(playerShip->m_selected_object, destination);
		ss_text << "\n\nDistance: ";

		if (distance > 0)
		{
			ss_text << distance;
			if (playerShip->m_selected_object->m_collider_type == StarshipObject)
			{
				if (distance > 0)
				{
					Starship* starship = (Starship*)playerShip->m_selected_object;
					//size_t distance_return = GameObject::GetLightYearsBetweenObjects(destination, starship->m_mission_base_location);
					//size_t propulsion_required = entity->CanSupplyFuel() ? distance : distance + distance_return;//prepare for a back and forth if destination cannot supply fuel
					//size_t propulsion_available = starship->GetPropulsionAvailable();
					//size_t propulsion_required = starship->GetPropulsionRequired(destination);
					//
					//size_t propulsion_missing = starship->m_propulsion + starship->m_propulsion_assigned > propulsion_required ? 0 : propulsion_required - starship->m_propulsion - starship->m_propulsion_assigned;
					//ss_text << "\nPropulsion available: " << propulsion_available << " / " << propulsion_required << " required";
					//
					//if (!starship->m_arrived_at_distination)
					//{
					//	string current_fuel = starship->GetBestAssignedPropulsionAvailable();
					//	if (!current_fuel.empty())
					//	{
					//		ss_text << "\nCurrent fuel used: " << current_fuel << " (" << starship->m_fuel_assigned[current_fuel] << ")";
					//	}
					//}
				}
			}
		}
		else
		{
			ss_text << "current location";
		}

		//location
		ss_text << "\n\nBase location: ";
		if (starship->m_mission_base_location)
		{
			ss_text << starship->m_mission_base_location->m_display_name;
		}
		else
		{
			ss_text << "none";
		}

		ss_text << "\nTask location: ";
		if (starship->m_mission_task_location)
		{
			ss_text << starship->m_mission_task_location->m_display_name;
		}
		else
		{
			ss_text << "none";
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

	sf::Vector2f position = sf::Vector2f(size.x / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + (*CurrentGame).m_view.getCenter().x, size.y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + (*CurrentGame).m_view.getCenter().y);
	//sf::Vector2f position = sf::Vector2f((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_view.getCenter().y);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	m_text.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//options texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	//m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//dynamic size
	text_height += m_text.getGlobalBounds().height;
	setSize(sf::Vector2f(getSize().x, text_height));
	setOrigin(sf::Vector2f(getOrigin().x, text_height / 2));
	sf::Vector2f position_new = sf::Vector2f(getPosition().x, getSize().y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + (*CurrentGame).m_view.getCenter().y);
	setPosition(position_new.x, position_new.y);

	*/
}

SFUnitInfoPanel::~SFUnitInfoPanel()
{
	
}

void SFUnitInfoPanel::Update(sf::Time deltaTime)
{

}

void SFUnitInfoPanel::Draw(sf::RenderTexture& screen)
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

GameObject* SFUnitInfoPanel::GetUnit()
{
	return m_unit;
}