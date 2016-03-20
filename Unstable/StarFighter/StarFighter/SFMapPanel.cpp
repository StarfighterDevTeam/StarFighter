#include "SFMapPanel.h"

extern Game* CurrentGame;

//COMPONENTS
StellarHub::StellarHub()
{
	setSize(sf::Vector2f(24, 24));
	setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	setFillColor(sf::Color::Red);

	m_distance_to_current_position = 0;
	m_coordinates = sf::Vector2f(0, 0);
	m_feedback_state = StellarComponent_NormalState;
}

StellarHub::StellarHub(string hub_name) : StellarHub()
{
	m_display_name = hub_name;
}

bool StellarHub::Update(GameObject& cursor, bool forbid_collision)
{
	bool cursor_colliding = SFPanel::IsCursorCollidingWithRectangle(cursor, *this);
	if (cursor_colliding && !forbid_collision)
	{
		if (m_feedback_state != StellarComponent_HighlightState)
		{
			m_feedback_state = StellarComponent_HighlightState;
			setFillColor(sf::Color::Yellow);
		}
	}
	else if (m_feedback_state != StellarComponent_NormalState)
	{
		m_feedback_state = StellarComponent_NormalState;
		setFillColor(sf::Color::Red);
	}

	return cursor_colliding && !forbid_collision;
}

StellarNode::StellarNode()
{
	setRadius(4);
	setOrigin(sf::Vector2f(getRadius(), getRadius()));
	setFillColor(sf::Color(120, 120, 255, 255));//blue-grey

	m_coordinates = sf::Vector2f(0, 0);
	m_feedback_state = StellarComponent_NormalState;
}

StellarSegment::StellarSegment(bool vertical, float segment_size)
{
	m_size_on_stellar_map = segment_size;
	sf::Vector2f size = vertical ? sf::Vector2f(STELLAR_SEGMENT_THICKNESS, STELLARMAP_SCALE * segment_size) : sf::Vector2f(STELLARMAP_SCALE * segment_size, STELLAR_SEGMENT_THICKNESS);
	setSize(size);
	setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	setFillColor(sf::Color::White);

	m_coordinates = sf::Vector2f(0, 0);
	m_feedback_state = StellarComponent_NormalState;
	m_max_hazard_unlocked = HAZARD_LEVEL_1 + 1;
}

bool StellarSegment::Update(GameObject& cursor, bool forbid_collision)
{
	bool cursor_colliding = SFPanel::IsCursorCollidingWithRectangle(cursor, *this);

	if (cursor_colliding && !forbid_collision)
	{
		if (m_feedback_state != StellarComponent_HighlightState)
		{
			m_feedback_state = StellarComponent_HighlightState;
			setFillColor(sf::Color::Yellow);
		}
	}
	else if (m_feedback_state != StellarComponent_NormalState)
	{
		m_feedback_state = StellarComponent_NormalState;
		setFillColor(sf::Color::White);
	}

	return cursor_colliding && !forbid_collision;
}

StellarBranch::StellarBranch()
{
	m_hub = NULL;
}

void StellarBranch::SetPosition(sf::Vector2f position)
{
	if (m_hub)
	{
		m_hub->setPosition(sf::Vector2f(position.x + m_hub->m_coordinates.x * STELLARMAP_SCALE, position.y - m_hub->m_coordinates.y * STELLARMAP_SCALE));
	}
	if (!m_segments.empty())
	{
		size_t segmentsVectorSize = m_segments.size();
		for (size_t i = 0; i < segmentsVectorSize; i++)
		{
			m_segments[i]->setPosition(sf::Vector2f(position.x + m_segments[i]->m_coordinates.x * STELLARMAP_SCALE, position.y - m_segments[i]->m_coordinates.y * STELLARMAP_SCALE));
		}
	}
	if (!m_nodes.empty())
	{
		size_t nodesVectorSize = m_nodes.size();
		for (size_t i = 0; i < nodesVectorSize; i++)
		{
			m_nodes[i]->setPosition(sf::Vector2f(position.x + m_nodes[i]->m_coordinates.x * STELLARMAP_SCALE, position.y - m_nodes[i]->m_coordinates.y * STELLARMAP_SCALE));
		}
	}
}

void StellarBranch::DrawHub(sf::RenderTexture& screen)
{
	if (m_hub)
	{
		screen.draw(*m_hub);
	}
}

void StellarBranch::DrawNodes(sf::RenderTexture& screen)
{
	if (!m_nodes.empty())
	{
		size_t nodesVectorSize = m_nodes.size();
		for (size_t i = 0; i < nodesVectorSize; i++)
		{
			screen.draw(*m_nodes[i]);
		}
	}
}

void StellarBranch::DrawSegments(sf::RenderTexture& screen)
{
	if (!m_segments.empty())
	{
		size_t segmentsVectorSize = m_segments.size();
		for (size_t i = 0; i < segmentsVectorSize; i++)
		{
			screen.draw(*m_segments[i]);
		}
	}
}

//STELLAR INFO PANEL
SFStellarInfoPanel::SFStellarInfoPanel(sf::Vector2f position, sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_MapInfo)
{
	m_playerShip = playerShip;

	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	setFillColor(sf::Color(20, 20, 20, 230));//dark grey
	setOutlineThickness(0);

	setPosition(sf::Vector2f(position.x + size.x / 2 + STELLARMAP_INFO_PANEL_POSITION_X, position.y + STELLARMAP_INFO_PANEL_POSITION_Y));

	//texts
	m_title_text.setCharacterSize(18);
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
}

SFStellarInfoPanel::SFStellarInfoPanel(StellarHub* hub, int teleportation_cost, sf::Vector2f size, Ship* playerShip) : SFStellarInfoPanel(hub->getPosition(), size, playerShip)
{
	if (hub)
	{
		m_arrow = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_ARROW_FILENAME, sf::Vector2f(INTERACTION_ARROW_WIDTH, INTERACTION_ARROW_HEIGHT),
			sf::Vector2f(INTERACTION_ARROW_WIDTH / 2, INTERACTION_ARROW_HEIGHT / 2));

		//text content
		m_title_text.setString(hub->m_display_name.c_str());

		bool teleportation_available = teleportation_cost > 0;
		ostringstream ss;
		if (teleportation_available)
		{
			ss << "Teleportation: $" << teleportation_cost;
			if (playerShip->m_money < teleportation_cost)
			{
				m_text.setColor(sf::Color(80, 80, 80, 255));//greyed
				ss << " (insufficient credits)";
			}
		}
		else
		{
			ss << "Current location";
		}
		m_text.setString(ss.str());
		
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x * teleportation_available, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getGlobalBounds().height + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x * teleportation_available, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_text.getGlobalBounds().height / 2;
		m_arrow.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES - getSize().x / 2, m_text.getPosition().y + m_text.getCharacterSize() / 2 + 3);//+3 because fuck this, I can't get it positionned properly
		m_arrow.m_visible = teleportation_available;
	}
}

SFStellarInfoPanel::SFStellarInfoPanel(StellarSegment* segment, sf::Vector2f size, Ship* playerShip) : SFStellarInfoPanel(segment->getPosition(), size, playerShip)
{
	if (segment)
	{
		//text content
		m_title_text.setString(segment->m_display_name.c_str());

		ostringstream ss;
		ss << "Hazard level unlocked: " << segment->m_max_hazard_unlocked + 1 << " / " << NB_HAZARD_LEVELS;
		m_text.setString(ss.str());

		m_arrow.m_visible = false;

		//text position
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	}
}

void SFStellarInfoPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		screen.draw(m_text);

		if (m_arrow.m_visible)
		{
			screen.draw(m_arrow);
		}
	}
}

sf::Vector2f SFStellarInfoPanel::GetRealCoordinates(sf::Vector2f rendered_coordinates, sf::Vector2f panel_position, sf::Vector2f panel_size)
{
	float x = rendered_coordinates.x - panel_position.x + panel_size.x / 2;
	float y = rendered_coordinates.y - panel_position.y + panel_size.y / 2;

	return sf::Vector2f(x, y);
}

sf::Vector2f SFStellarInfoPanel::GetFakeCoordinates(sf::Vector2f rendered_coordinates, sf::Vector2f panel_position, sf::Vector2f panel_size)
{
	float x = rendered_coordinates.x + panel_position.x - panel_size.x / 2;
	float y = rendered_coordinates.y + panel_position.y - panel_size.y / 2;

	return sf::Vector2f(x, y);
}

//MAP PANEL
SFMapPanel::SFMapPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Map)
{
	m_playerShip = playerShip;
	m_info_panel = NULL;
	m_teleportation_cost = 0;

	//panel position and color
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));
	setFillColor(sf::Color(20, 20, 20, 240));//dark grey
	setOutlineThickness(0);

	//panel content
	//cursor
	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));
	m_cursor.setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

	//ship miniature
	sf::Vector2f ship_size = playerShip->m_fake_ship ? playerShip->m_fake_ship->m_size : playerShip->m_size;
	string ship_texture = playerShip->m_fake_ship ? playerShip->m_fake_ship->m_textureName : playerShip->m_textureName;
	int frameNumber = playerShip->m_fake_ship ? playerShip->m_fake_ship->m_frameNumber : playerShip->m_frameNumber;
	int animationNumber = playerShip->m_fake_ship ? playerShip->m_fake_ship->m_animationNumber : playerShip->m_animationNumber;

	m_ship = GameObject(this->getPosition(), sf::Vector2f(0, 0), ship_texture, ship_size, sf::Vector2f(ship_size.x/2, ship_size.y/2), animationNumber, frameNumber);
	m_ship.setPosition(sf::Vector2f(this->getPosition()));
	m_ship.setScale(STELLARMAP_SHIP_MINIATURE_SCALE, STELLARMAP_SHIP_MINIATURE_SCALE);

	//texts
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	float text_height = INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height / 2;
	m_title_text.setString("STELLAR MAP");
	m_title_text.setPosition(sf::Vector2f(getPosition().x - m_title_text.getGlobalBounds().width / 2, getPosition().y - getSize().y / 2 + text_height));

	text_height += m_title_text.getGlobalBounds().height/2 + INTERACTION_INTERBLOCK;
	ostringstream ss_text;
	ss_text << "Current location: " << playerShip->m_currentScene_name;
	m_text.setString(ss_text.str());
	m_text.setPosition(sf::Vector2f(getPosition().x - m_text.getGlobalBounds().width / 2, getPosition().y - getSize().y / 2 + text_height));

	ostringstream ss_helpNavigation;
	ss_helpNavigation << "\n\n\Slowmotion: exit\nBrake: center map view\nMove cursor on borders: scroll map\nSelect location, then press Fire: teleport";
	m_actions_text.setString(ss_helpNavigation.str());
	m_actions_text.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + getSize().y / 2 - 2 * INTERACTION_INTERBLOCK - m_actions_text.getGlobalBounds().height));

	//render texture
	m_texture.create((unsigned int)size.x, (unsigned int)size.y);
	m_scroll_offset = sf::Vector2f(0, 0);

	//CONSTRUCTION OF THE MAP
	//Loading the list of all scenes, contained in SCENES_FILE
	m_scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));
	m_scenesConfigSize = m_scenesConfig.size();

	//start with current hub
	StellarBranch* mother_branch = new StellarBranch();
	m_current_hub = new StellarHub();
	mother_branch->m_hub = m_current_hub;
	mother_branch->m_hub->m_display_name = m_playerShip->m_currentScene_name;
	mother_branch->SetPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));
	m_branches.push_back(mother_branch);
	//m_checked_hubs.push_back(m_current_hub);

	//scan branches around
	ScanBranches(mother_branch->m_hub->m_display_name, NO_DIRECTION, sf::Vector2f(0,0));

	//get size of the known area
	m_map_content_area = GetStellarMapKnownSize();

	//set position of all elements, based on their computed coordinates
	UpdateBranchesPosition(false, false);
}

SFMapPanel::~SFMapPanel()
{

}

void SFMapPanel::SetMapViewOffset(sf::Vector2f offset)
{
	m_scroll_offset = offset;
	m_cursor.setPosition(getPosition().x + offset.x, getPosition().y + offset.y);
}

void SFMapPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//Scrolling
	GetScrollingInput(m_cursor, deltaTime);
	UpdateBranchesPosition(false, false);
	if (m_current_hub)
	{
		m_ship.setPosition(m_current_hub->getPosition());
	}

	//check collisions, with priority on hubs over segments, and only chose one element to highlight
	bool already_colllided_with_a_hub = false;
	bool already_colllided_with_a_segment = false;

	if (!m_branches.empty())
	{
		if (!m_branches.empty())
		{
			size_t branchesVectorSize = m_branches.size();
			for (size_t i = 0; i < branchesVectorSize; i++)
			{
				if (m_branches[i]->m_hub)
				{
					//flag if we have already collided with a hub
					if (!already_colllided_with_a_hub)
					{
						already_colllided_with_a_hub = m_branches[i]->m_hub->Update(m_cursor, false);
						//create info panel
						if (already_colllided_with_a_hub)
						{
							//delete old panel if existing
							if (m_info_panel)
							{
								delete m_info_panel;
							}
							m_teleportation_cost = ComputeTeleportationCost(m_branches[i]->m_hub);
							m_targeted_location = m_branches[i]->m_hub->m_display_name;
							m_info_panel = new SFStellarInfoPanel(m_branches[i]->m_hub, m_teleportation_cost, sf::Vector2f(STELLARMAP_INFO_PANEL_SIZE_X, STELLARMAP_INFO_PANEL_SIZE_Y), m_playerShip);
						}
					}
					else
					{
						m_branches[i]->m_hub->Update(m_cursor, true);
					}
				}
			}
			
			for (size_t i = 0; i < branchesVectorSize; i++)
			{
				size_t segmentsVectorSize = m_branches[i]->m_segments.size();
				for (size_t j = 0; j < segmentsVectorSize; j++)
				{
					//flag if we have already collided with a segment
					if (!already_colllided_with_a_hub && !already_colllided_with_a_segment)
					{
						already_colllided_with_a_segment = m_branches[i]->m_segments[j]->Update(m_cursor, false);
						//create info panel
						if (already_colllided_with_a_segment)
						{
							//delete old panel if existing
							if (m_info_panel)
							{
								delete m_info_panel;
							}
							m_teleportation_cost = -1;
							m_targeted_location = "";
							m_info_panel = new SFStellarInfoPanel(m_branches[i]->m_segments[j], sf::Vector2f(STELLARMAP_INFO_PANEL_SIZE_X, STELLARMAP_INFO_PANEL_SIZE_Y), m_playerShip);
						}
					}
					else
					{
						m_branches[i]->m_segments[j]->Update(m_cursor, true);
					}
				}
			}
		}
	}

	//Update cursor state
	if (!already_colllided_with_a_hub && !already_colllided_with_a_segment)
	{
		m_cursor.setAnimationLine(Cursor_NormalState);
	}
	else
	{
		m_cursor.setAnimationLine(Cursor_HighlightState);
	}

	//Delete stellar map info panel if not needed anymore
	if (!already_colllided_with_a_hub && !already_colllided_with_a_segment)
	{
		if (m_info_panel)
		{
			delete m_info_panel;
			m_info_panel = NULL;
			m_teleportation_cost = -1;
			m_targeted_location = "";
		}
	}
}

void SFMapPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		screen.draw(m_text);
		screen.draw(m_actions_text);

		//Scrollable content
		m_texture.clear(sf::Color(0, 0, 0, 50));

		//specific content to be drawn on a separate RenderTexture
		UpdateBranchesPosition(true, false);//offset coordinates to match the RenderTexture coordinates
	
		if (!m_branches.empty())
		{
			size_t branchesVectorSize = m_branches.size();
			for (size_t i = 0; i < branchesVectorSize; i++)
			{
				m_branches[i]->DrawSegments(m_texture);
			}
			for (size_t i = 0; i < branchesVectorSize; i++)
			{
				m_branches[i]->DrawNodes(m_texture);
				m_branches[i]->DrawHub(m_texture);
			}
		}

		m_texture.display();
		UpdateBranchesPosition(false, false);//undo offset to get match into real coordinates for calculations
		sf::Sprite temp(m_texture.getTexture());
		unsigned int sizex = m_texture.getSize().x;
		float posx = this->getPosition().x;
		temp.setOrigin(this->getOrigin());
		temp.setPosition(this->getPosition());
		screen.draw(temp);

		screen.draw(m_ship);
		screen.draw(m_cursor);

		if (m_info_panel)
		{
			m_info_panel->Draw(screen);
		}
	}
}

GameObject* SFMapPanel::GetCursor()
{
	return &m_cursor;
}

int SFMapPanel::GetTeleportationCost()
{
	return m_teleportation_cost;
}

string SFMapPanel::GetTeleportationDestination()
{
	return m_targeted_location;
}

void SFMapPanel::GetScrollingInput(GameObject& cursor, sf::Time deltaTime)
{
	if (!cursor.m_visible)
	{
		return;
	}

	sf::FloatRect inside_rect = FloatRect(getPosition().x - getSize().x / 2 + STELLARMAP_SCROLLING_AREA_WIDTH, getPosition().y - getSize().y / 2 + STELLARMAP_SCROLLING_AREA_WIDTH,
		getSize().x - 2 * STELLARMAP_SCROLLING_AREA_WIDTH, getSize().y - 2 * STELLARMAP_SCROLLING_AREA_WIDTH);

	if (!inside_rect.contains(cursor.getPosition()))
	{
		sf::Vector2f speed = sf::Vector2f(cursor.getPosition().x - this->getPosition().x, cursor.getPosition().y - this->getPosition().y);
		GameObject::NormalizeSpeed(&speed, STELLARMAP_SCROLLING_SPEED);
		if (abs(speed.x) > STELLARMAP_SCROLLING_MIN_SPEED)
		{
			m_scroll_offset.x -= speed.x * deltaTime.asSeconds();
		}
		if (abs(speed.y) > STELLARMAP_SCROLLING_MIN_SPEED)
		{
			m_scroll_offset.y -= speed.y * deltaTime.asSeconds();
		}
	}

	//constraints
	float left_border = -getSize().x / 2 + m_map_content_area.left + STELLARMAP_MARGIN_SIDES_WIDTH;
	float right_border = getSize().x / 2 - (m_map_content_area.width + m_map_content_area.left) - STELLARMAP_MARGIN_SIDES_WIDTH;
	float up_border = -getSize().y / 2 + m_map_content_area.top + STELLARMAP_MARGIN_SIDES_WIDTH;
	float down_border = getSize().y / 2 - (m_map_content_area.height - m_map_content_area.top) - STELLARMAP_MARGIN_SIDES_WIDTH;

	if (m_scroll_offset.x < left_border)
	{
		m_scroll_offset.x = left_border;
	}
	if (m_scroll_offset.x > right_border)
	{
		m_scroll_offset.x = right_border;
	}
	if (m_scroll_offset.y < up_border)
	{
		m_scroll_offset.y = up_border;
	}
	if (m_scroll_offset.y > down_border)
	{
		m_scroll_offset.y = down_border;
	}
}

void SFMapPanel::UpdateBranchesPosition(bool into_real_coordinates, bool into_fake_coordinates)
{
	if (!m_branches.empty())
	{
		size_t branchesVectorSize = m_branches.size();
		for (size_t i = 0; i < branchesVectorSize; i++)
		{
			sf::Vector2f pos = sf::Vector2f(this->getPosition().x + m_scroll_offset.x, this->getPosition().y + m_scroll_offset.y);
			if (into_real_coordinates)
			{
				m_branches[i]->SetPosition(SFStellarInfoPanel::GetRealCoordinates(pos, getPosition(), getSize()));
			}
			else if (into_fake_coordinates)
			{
				m_branches[i]->SetPosition(SFStellarInfoPanel::GetFakeCoordinates(pos, getPosition(), getSize()));
			}
			else
			{
				m_branches[i]->SetPosition(pos);
			}
		}
	}
}

void SFMapPanel::GetMaxCoordinates(sf::Vector2f* current_max_horizontal, sf::Vector2f* current_max_vertical, sf::Vector2f object_coordinates)
{
	if (object_coordinates.x < current_max_horizontal->x)
	{
		current_max_horizontal->x = object_coordinates.x;
	}
	if (object_coordinates.x > current_max_horizontal->y)
	{
		current_max_horizontal->y = object_coordinates.x;
	}
	if (object_coordinates.y < current_max_vertical->x)
	{
		current_max_vertical->x = object_coordinates.y;
	}
	if (object_coordinates.y > current_max_vertical->y)
	{
		current_max_vertical->y = object_coordinates.y;
	}
}

sf::FloatRect SFMapPanel::GetStellarMapKnownSize()
{
	sf::Vector2f max_vertical = sf::Vector2f(0, 0);
	sf::Vector2f max_horizontal = sf::Vector2f(0, 0);

	if (!m_branches.empty())
	{
		size_t branchesVectorSize = m_branches.size();
		for (size_t i = 0; i < branchesVectorSize; i++)
		{
			if (m_branches[i]->m_hub)
			{
				GetMaxCoordinates(&max_horizontal, &max_vertical, m_branches[i]->m_hub->m_coordinates);
			}

			size_t segmentsVectorSize = m_branches[i]->m_segments.size();
			for (size_t j = 0; j < segmentsVectorSize; j++)
			{
				bool vertical_segment = m_branches[i]->m_segments[j]->m_vertical;
				bool minimum_value_modified = vertical_segment ? m_branches[i]->m_segments[j]->m_coordinates.y < 0 : m_branches[i]->m_segments[j]->m_coordinates.x < 0;
				int axis = minimum_value_modified ? -1 : 1;

				GetMaxCoordinates(&max_horizontal, &max_vertical, sf::Vector2f(m_branches[i]->m_segments[j]->m_coordinates.x + 0.5*axis*!vertical_segment, m_branches[i]->m_segments[j]->m_coordinates.y + 0.5*axis*vertical_segment));
			}

			size_t nodesVectorSize = m_branches[i]->m_nodes.size();
			for (size_t j = 0; j < nodesVectorSize; j++)
			{
				GetMaxCoordinates(&max_horizontal, &max_vertical, m_branches[i]->m_nodes[j]->m_coordinates);
			}
		}
	}

	float width = (max_horizontal.y - max_horizontal.x) * STELLARMAP_SCALE;
	float height = (max_vertical.y - max_vertical.x) * STELLARMAP_SCALE;

	return sf::FloatRect(max_horizontal.x * STELLARMAP_SCALE, max_vertical.y * STELLARMAP_SCALE, width, height);
}

int SFMapPanel::ComputeTeleportationCost(StellarHub* destination)
{
	if (destination && m_current_hub)
	{
		int diff_x = m_current_hub->m_coordinates.x - destination->m_coordinates.x;
		int diff_y = m_current_hub->m_coordinates.y - destination->m_coordinates.y;

		int cost = (diff_x + diff_y) * (diff_x + diff_y) * STELLARMAP_TELEPORTATION_COST;//exponential cost: distance as the crow flies ^4)

		return cost;
	}
	else
	{
		return -1;
	}
}

void SFMapPanel::ScanBranches(string starting_scene, Directions direction, sf::Vector2f starting_coordinates)
{
	if (starting_scene.empty())
	{
		printf("ERROR: <!> loading an empty stellar hub or with an empty name, in SFMapPanel::ScanBranches().\n");
		return;
	}

	//scene already checked?
	if (IsSceneAlreadyChecked(starting_scene, true))
	{
		//scene has already been scanned, no need to created a new branch
		return;
	}

	//get linked scenes
	string links[NO_DIRECTION] = { "0", "0", "0", "0" };
	for (int i = 0; i < m_scenesConfigSize; i++)
	{
		if (m_scenesConfig[i][SCENE_NAME].compare(starting_scene) == 0)
		{
			//all directions
			if (direction == NO_DIRECTION)
			{
				links[DIRECTION_UP] = m_scenesConfig[i][SCENE_LINK_UP];
				links[DIRECTION_DOWN] = m_scenesConfig[i][SCENE_LINK_DOWN];
				links[DIRECTION_RIGHT] = m_scenesConfig[i][SCENE_LINK_RIGHT];
				links[DIRECTION_LEFT] = m_scenesConfig[i][SCENE_LINK_LEFT];
			}
			//forced direction
			else
			{
				links[direction] = m_scenesConfig[i][SCENE_LINK_UP + direction];
			}
			break;
		}
	}

	//scan linked scenes
	for (int direction = 0; direction < NO_DIRECTION; direction++)
	{
		sf::Vector2f starting_coordinates_ = starting_coordinates;
		if (!links[direction].empty() && links[direction].compare("0") != 0)
		{
			//scene already checked?
			if (IsSceneAlreadyChecked(links[direction], false))
			{
				//scene has already been scanned, no need to created a new branch
				continue;
			}
			//scene has been scouted by the player?
			//else if (!IsSceneKnownByThePlayer(links[direction]))
			//{
			//	//scene is not known by the plauer
			//	continue;
			//}

			//create a new branch
			StellarBranch* new_branch = new StellarBranch();
			m_branches.push_back(new_branch);
			//printf("\n\nBranch created (from %s)\n", starting_scene.c_str());

			//start looping until reaching a hub (= branch ending)
			vector<string> scenes_to_scan;
			scenes_to_scan.push_back(links[direction]);
			while (!scenes_to_scan.empty())
			{
				links[direction] = scenes_to_scan.front();//update loop to match currently scanned segment
				scenes_to_scan.clear();
				//scene already checked?
				if (IsSceneAlreadyChecked(links[direction], false))
				{
					//scene has already been scanned
					continue;
				}
				else if (!IsSceneKnownByThePlayer(links[direction]) && !m_branches.back()->m_segments.empty())
				{
					//scene is not known by the plauer
					continue;
				}

				//Get filename of the next scene to scan
				string scene_filename;
				for (int i = 0; i < m_scenesConfigSize; i++)
				{
					if (m_scenesConfig[i][SCENE_NAME].compare(links[direction]) == 0)
					{
						scene_filename = m_scenesConfig[i][SCENE_FILENAME];
						break;
					}
				}

				//Scan the file
				if (scene_filename.empty())
				{
					continue;
				}
				//Hub found?
				//printf("Scene scanned: %s (direction: %d)\n", links[direction].c_str(), direction);
				if (ScanScene(scene_filename, links[direction], (Directions)direction, starting_coordinates_))
				{
					//if hub is not known yet: SCAN-CEPTION: repeat process with this new hub, until we reached all known hubs
					if (m_branches.back()->m_hub &&!links[direction].empty() && links[direction].compare("0") != 0)
					{
						ScanBranches(links[direction], NO_DIRECTION, starting_coordinates_);
					}
				}
				else
				{
					//hub not found: new segment
					StellarSegment* segment = m_branches.back()->m_segments.back();//get segment freshly created and update it
					float size_of_new_segment = segment->m_size_on_stellar_map;
					starting_coordinates_.x += (((Directions)direction == DIRECTION_RIGHT) - ((Directions)direction == DIRECTION_LEFT)) * size_of_new_segment;
					starting_coordinates_.y += (((Directions)direction == DIRECTION_UP) - ((Directions)direction == DIRECTION_DOWN)) * size_of_new_segment;
					segment->m_vertical = ((Directions)direction == DIRECTION_UP || (Directions)direction == DIRECTION_DOWN);

					//get next linked scene name
					string next_scene_name;
					for (size_t i = 0; i < m_scenesConfigSize; i++)
					{
						if (m_scenesConfig[i][SCENE_NAME].compare(links[direction]) == 0)
						{
							//Loading the linked scene names
							next_scene_name = m_scenesConfig[i][SCENE_LINK_UP + direction];
							break;
						}
					}

					//register the name of the next scene and register it as a scene to scan
					if (!next_scene_name.empty())
					{
						//segment->m_display_name = links[direction];
						scenes_to_scan.push_back(next_scene_name);
					}

					//printf("Segment created: %s\n", segment->m_display_name.c_str());

					m_checked_scenes.push_back(links[direction]);
					segment = NULL;
				}
			}
		}
	}
}

bool SFMapPanel::ScanScene(string scene_filename, string scene, Directions direction, sf::Vector2f starting_coordinates)
{
	//Loading the particular scene that we want to load
	vector<vector<string>> config = *(FileLoaderUtils::FileLoader(scene_filename));
	for (std::vector<vector<string>>::iterator it = config.begin(); it != config.end(); it++)
	{
		if ((*it)[0].compare("bg") == 0)
		{
			bool hub = (*it)[BACKGROUND_VERTICAL].compare("H") != 0 && (*it)[BACKGROUND_VERTICAL].compare("V") != 0;
			bool vertical = (*it)[BACKGROUND_VERTICAL].compare("V") == 0;

			//case hub reached
			if (hub)
			{
				//create new hub
				StellarHub* new_hub = new StellarHub(scene);
				new_hub->m_coordinates = starting_coordinates;
				new_hub->m_display_name = scene;
				m_branches.back()->m_hub = new_hub;

				//printf("Hub created: %s\n", scene.c_str());

				return true;
			}
			//case new segment reached
			else
			{
				//if previous scanned scene was a segment too, create a node
				if (!m_branches.back()->m_segments.empty())
				{
					StellarNode* new_node = new StellarNode();
					m_branches.back()->m_nodes.push_back(new_node);
					new_node->m_coordinates = starting_coordinates;

					//printf("Node created\n");
				}

				//create new segment
				float segment_size = atof((*it)[BACKGROUND_SIZE_ON_STELLARMAP].c_str());
				StellarSegment* new_segment = new StellarSegment(vertical, segment_size);
				m_branches.back()->m_segments.push_back(new_segment);
				new_segment->m_display_name = scene;

				new_segment->m_size_on_stellar_map = segment_size;
				new_segment->m_coordinates.x = starting_coordinates.x + ((direction == DIRECTION_RIGHT) - (direction == DIRECTION_LEFT)) * new_segment->m_size_on_stellar_map / 2;
				new_segment->m_coordinates.y = starting_coordinates.y + ((direction == DIRECTION_UP) - (direction == DIRECTION_DOWN)) * new_segment->m_size_on_stellar_map / 2;

				new_segment->m_max_hazard_unlocked = GetMaxHazardLevelUnlocked(scene);
				//scenes that have not been explored yet, but that has been scouted (player has seen the portal leading to it), should be displayed as the minimal hazard unlocked (instead of the -1 error code)
				if (new_segment->m_max_hazard_unlocked < 0)
				{
					new_segment->m_max_hazard_unlocked = 0;
				}

				return false;
			}
		}
	}
}

bool SFMapPanel::IsSceneAlreadyChecked(string new_scene, bool add_if_not_checked)
{
	size_t checkedScenesVectorSize = m_checked_scenes.size();
	for (size_t i = 0; i < checkedScenesVectorSize; i++)
	{
		//element found
		if (m_checked_scenes[i].compare(new_scene) == 0)
		{
			return true;
		}
	}

	//element not found
	if (add_if_not_checked)
	{
		m_checked_scenes.push_back(new_scene);
	}
	return false;
}

bool SFMapPanel::IsSceneKnownByThePlayer(string new_scene)
{
	size_t knownScenesVectorSize = m_playerShip->m_knownScenes.size();

	map<string, int>::iterator it = m_playerShip->m_knownScenes.find(new_scene);

	//if scene not already known
	return it != m_playerShip->m_knownScenes.end();
}

int SFMapPanel::GetMaxHazardLevelUnlocked(string new_scene)
{
	size_t knownScenesVectorSize = m_playerShip->m_knownScenes.size();

	map<string, int>::iterator it = m_playerShip->m_knownScenes.find(new_scene);

	//if scene not already known
	if (it != m_playerShip->m_knownScenes.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}