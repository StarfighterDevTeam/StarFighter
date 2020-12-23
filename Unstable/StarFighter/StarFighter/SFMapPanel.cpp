#include "SFMapPanel.h"

extern Game* CurrentGame;

//COMPONENTS
StellarHub::StellarHub()
{
	setSize(sf::Vector2f(24, 24));
	setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	setFillColor(sf::Color::Red);

	m_distance_to_current_position = 0;
	//m_level = 1;
	m_coordinates = sf::Vector2f(0, 0);
	m_feedback_state = StellarComponent_NormalState;
}

StellarHub::StellarHub(string hub_name) : StellarHub()
{
	m_display_name = hub_name;
}

StellarHub::StellarHub(string hub_name, sf::Vector2f coordinates) : StellarHub()
{
	m_display_name = hub_name;
	m_coordinates = coordinates;
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

StellarNode::StellarNode(sf::Vector2f coordinates) : StellarNode()
{
	m_coordinates = coordinates;
}

StellarSegment::StellarSegment(bool vertical, float segment_size)
{
	m_size = segment_size;
	sf::Vector2f size = vertical ? sf::Vector2f(STELLAR_SEGMENT_THICKNESS, STELLARMAP_SCALE * segment_size) : sf::Vector2f(STELLARMAP_SCALE * segment_size, STELLAR_SEGMENT_THICKNESS);
	setSize(size);
	setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	setFillColor(sf::Color::White);

	m_coordinates = sf::Vector2f(0, 0);
	m_feedback_state = StellarComponent_NormalState;
	m_max_hazard_unlocked = HAZARD_LEVEL_1 + 1;
}

StellarSegment::StellarSegment(string segment_name, Directions direction, float segment_size, sf::Vector2f coordinates) : StellarSegment(direction == DIRECTION_UP || direction == DIRECTION_DOWN, segment_size)
{
	if (direction == DIRECTION_UP)
		m_coordinates = sf::Vector2f(coordinates.x, coordinates.y - segment_size * 0.5);
	else if (direction == DIRECTION_DOWN)
			m_coordinates = sf::Vector2f(coordinates.x, coordinates.y + segment_size * 0.5);
	else if (direction == DIRECTION_LEFT)
		m_coordinates = sf::Vector2f(coordinates.x + segment_size * 0.5, coordinates.y);
	else if (direction == DIRECTION_RIGHT)
		m_coordinates = sf::Vector2f(coordinates.x - segment_size * 0.5, coordinates.y);

	m_display_name = segment_name;
}

bool StellarSegment::Update(GameObject& cursor, bool forbid_collision, vector<StellarSegment*> branch_segments)
{
	bool cursor_colliding = SFPanel::IsCursorCollidingWithRectangle(cursor, *this);

	//highlighting one segment results in highlighting all segments of the branch
	size_t branchSegmentsVectorSize = branch_segments.size();

	if (cursor_colliding && !forbid_collision)
	{
		if (m_feedback_state != StellarComponent_HighlightState)
		{
			for (size_t i = 0; i < branchSegmentsVectorSize; i++)
			{
				branch_segments[i]->m_feedback_state = StellarComponent_HighlightState;
				branch_segments[i]->setFillColor(sf::Color::Yellow);
			}
		}
	}
	else if (m_feedback_state != StellarComponent_NormalState)
	{
		for (size_t i = 0; i < branchSegmentsVectorSize; i++)
		{
			branch_segments[i]->m_feedback_state = StellarComponent_NormalState;
			branch_segments[i]->setFillColor(sf::Color::White);
		}
	}

	return cursor_colliding && !forbid_collision;
}

StellarBranch::StellarBranch()
{
	m_hub = NULL;
}

StellarBranch::StellarBranch(string name) : StellarBranch()
{
	m_name = name;
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
SFStellarInfoPanel::SFStellarInfoPanel(sf::Vector2f position, sf::Vector2f size, Ship* playership) : SFPanel(size, SFPanel_MapInfo)
{
	m_playership = playership;

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

SFStellarInfoPanel::SFStellarInfoPanel(StellarHub* hub, int teleportation_cost, sf::Vector2f size, Ship* playership) : SFStellarInfoPanel(hub->getPosition(), size, playership)
{
	if (hub)
	{
		m_location_name = hub->m_display_name;

		//text content
		m_title_text.setString(SFReplaceTexts(hub->m_display_name.c_str(), -1));

		//bool teleportation_available = teleportation_cost > 0;
		ostringstream ss;
		//if (teleportation_available)
		//{
		//	m_actions = new SFActionBox ((*CurrentGame).m_font[Font_Arial]);
		//
		//	ss << "Teleportation: $" << teleportation_cost;
		//	if (playership->m_money < teleportation_cost)
		//	{
		//		ss << " (insufficient credits)";
		//	}
		//
		//	m_actions->SetString(ss.str(), ActionButton_A);
		//	if (playership->m_money < teleportation_cost)
		//	{
		//		m_actions->m_texts[ActionButton_A].setColor(sf::Color(255, 50, 50, 255));//red
		//	}
		//}
		//else
		//{
			ss << "Current location";
			m_text.setString(ss.str());
		//}
		
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getGlobalBounds().height + INTERACTION_INTERBLOCK;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_text.getGlobalBounds().height;
		
		if (m_actions)
		{
			m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		}
	}
}

SFStellarInfoPanel::SFStellarInfoPanel(StellarSegment* segment, sf::Vector2f size, Ship* playership) : SFStellarInfoPanel(segment->getPosition(), size, playership)
{
	if (segment)
	{
		m_location_name = segment->m_display_name;

		//text content
		m_title_text.setString(segment->m_display_name.c_str());

		//ostringstream ss;
		//ss << "Hazard level unlocked: " << segment->m_max_hazard_unlocked + 1 << " / " << NB_HAZARD_LEVELS;
		//m_text.setString(ss.str());

		//text position
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		//text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		//m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	}
}

void SFStellarInfoPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		//screen.draw(m_text);

		if (m_actions)
		{
			m_actions->Draw(screen);
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
SFMapPanel::SFMapPanel(sf::Vector2f size, Ship* playership) : SFPanel(size, SFPanel_Map)
{
	m_playership = playership;
	m_info_panel = NULL;
	m_teleportation_cost = 0;
	m_hightlighted_branch = NULL;

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
	sf::Vector2f ship_size = playership->m_fake_ship ? playership->m_fake_ship->m_size : playership->m_size;
	string ship_texture = playership->m_fake_ship ? playership->m_fake_ship->m_textureName : playership->m_textureName;
	int frameNumber = playership->m_fake_ship ? playership->m_fake_ship->m_frameNumber : playership->m_frameNumber;
	int animationNumber = playership->m_fake_ship ? playership->m_fake_ship->m_animationNumber : playership->m_animationNumber;

	m_ship = GameObject(this->getPosition(), sf::Vector2f(0, 0), ship_texture, ship_size, sf::Vector2f(ship_size.x / 2, ship_size.y / 2), animationNumber, frameNumber);
	m_ship.setPosition(sf::Vector2f(this->getPosition()));
	m_ship.setScale(STELLARMAP_SHIP_MINIATURE_SCALE, STELLARMAP_SHIP_MINIATURE_SCALE);

	//texts
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//buttons
	m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	m_actions->SetString("Center map", ActionButton_X);
	m_actions->SetString("Exit", ActionButton_B);

	float text_height = INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height / 2;
	m_title_text.setString("STELLAR MAP");
	m_title_text.setPosition(sf::Vector2f(getPosition().x - m_title_text.getGlobalBounds().width / 2, getPosition().y - getSize().y / 2 + text_height));

	text_height += m_title_text.getGlobalBounds().height / 2 + INTERACTION_INTERBLOCK;
	ostringstream ss_text;
	ss_text << "Current location: " << SFReplaceTexts(playership->m_currentScene_name, -1);
	m_text.setString(ss_text.str());
	m_text.setPosition(sf::Vector2f(getPosition().x - m_text.getGlobalBounds().width / 2, getPosition().y - getSize().y / 2 + text_height));

	float text_height2 = getPosition().y + getSize().y / 2 - 4 * INTERACTION_INTERBLOCK;// -m_actions_text.getGlobalBounds().height;
	m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + 2 * INTERACTION_INTERBLOCK, text_height2));

	//render texture
	m_texture.create((unsigned int)size.x, (unsigned int)size.y);
	m_scroll_offset = sf::Vector2f(0, 0);

	//CONSTRUCTION OF THE MAP
	CreateStellarMap_v2();
}

SFMapPanel::~SFMapPanel()
{

}

void SFMapPanel::CreateStellarMap_v2()
{
	float min_x = 0;
	float max_x = 0;
	float min_y = 0;
	float max_y = 0;

	vector<pair<string, sf::Vector2f> > branches_to_create;
	vector<string> branches_already_processed;
	branches_to_create.push_back(pair<string, sf::Vector2f>(STARTING_SCENE_STELLAR_MAP, sf::Vector2f(0, 0)));
	branches_already_processed.push_back(STARTING_SCENE_STELLAR_MAP);

	while (branches_to_create.empty() == false)
	{
		string name = branches_to_create.back().first;
		sf::Vector2f coordinates = branches_to_create.back().second;

		StellarBranch* new_branch = new StellarBranch(name);
		m_branches.push_back(new_branch);
		branches_to_create.pop_back();

		//hub or segment?
		if ((bool)stoi((*CurrentGame).m_generalScenesConfig[name][SCENE_IS_HUB]) == true)
		{
			//hub
			StellarHub* new_hub = new StellarHub(name, coordinates);
			new_branch->m_hub = new_hub;
			new_branch->m_direction = NO_DIRECTION;

			//flag current hub
			if (name.compare(m_playership->m_currentScene_name) == 0)
				m_current_hub = new_branch->m_hub;

			//next scenes to create
			for (int i = 0; i < NO_DIRECTION; i++)
				if ((*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_UP + i].compare("0") != 0)
				{
					bool found = false;
					for (vector<string>::iterator it = branches_already_processed.begin(); it != branches_already_processed.end(); it++)
					{
						if (it->compare((*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_UP + i]) == 0)
						{
							found = true;
							break;
						}
					}

					if (found == false)
					{
						branches_to_create.push_back(pair<string, sf::Vector2f>((*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_UP + i], coordinates));
						branches_already_processed.push_back((*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_UP + i]);
					}
				}
					
		}
		else//segment
		{
			bool known_scene = IsSceneKnownByThePlayer(name);

			//get segment's direction
			for (int i = 0; i < NO_DIRECTION; i++)
			{
				if ((*CurrentGame).m_generalScenesConfig[name][SCENE_LINK_UP + i].compare("0") != 0)
				{
					new_branch->m_direction = (Directions)i;
					break;
				}
			}

			//linked to a hub (stop there) or to another segment (->create a node and loop)
			bool next_scene_is_a_hub = false;
			string segment_name = name;
			while (next_scene_is_a_hub == false)
			{
				//create segment
				int size = stoi((*CurrentGame).m_generalScenesConfig[segment_name][SCENE_SIZE_IN_STELLAR_MAP]);
				coordinates.x += ((new_branch->m_direction == DIRECTION_RIGHT) - (new_branch->m_direction == DIRECTION_LEFT)) * size;
				coordinates.y += ((new_branch->m_direction == DIRECTION_UP) - (new_branch->m_direction == DIRECTION_DOWN)) * size;

				//save min-max coordinates
				min_x = coordinates.x < min_x ? coordinates.x : min_x;
				min_y = coordinates.y < min_y ? coordinates.y : min_y;
				max_x = coordinates.x > max_x ? coordinates.x : max_x;
				max_y = coordinates.y > max_y ? coordinates.y : max_y;

				StellarSegment* new_segment = new StellarSegment(segment_name, new_branch->m_direction, size, coordinates);
				new_branch->m_segments.push_back(new_segment);

				string next_segment_name = (*CurrentGame).m_generalScenesConfig[segment_name][SCENE_LINK_UP + (int)new_branch->m_direction];

				if ((bool)stoi((*CurrentGame).m_generalScenesConfig[next_segment_name][SCENE_IS_HUB]) == false && known_scene == true && IsSceneKnownByThePlayer(next_segment_name) == true)
				{
					StellarNode* new_node = new StellarNode(coordinates);
					new_branch->m_nodes.push_back(new_node);
					segment_name = (*CurrentGame).m_generalScenesConfig[segment_name][SCENE_LINK_UP + (int)new_branch->m_direction];
				}
				else if (known_scene == true)
				{
					next_scene_is_a_hub = true;

					bool found = false;
					for (vector<string>::iterator it = branches_already_processed.begin(); it != branches_already_processed.end(); it++)
					{
						if (it->compare(next_segment_name) == 0)
						{
							found = true;
							break;
						}
					}

					if (found == false)
					{
						if (IsSceneKnownByThePlayer(next_segment_name) == true)
						{
							branches_to_create.push_back(pair<string, sf::Vector2f>(next_segment_name, coordinates));
							branches_already_processed.push_back(next_segment_name);
						}
					}
				}

				if (known_scene == false)
					next_scene_is_a_hub = true;//stop after the first unknown scene
			}
		}
	}

	//get size of the known area
	m_map_content_area = sf::FloatRect(sf::Vector2f(min_x * STELLARMAP_SCALE, max_y * STELLARMAP_SCALE), sf::Vector2f((max_x - min_x) * STELLARMAP_SCALE, (max_y - min_y) * STELLARMAP_SCALE));//   GetStellarMapKnownSize();

	//set position of all elements, based on their computed coordinates
	UpdateBranchesPosition(false, false);
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
		m_ship.setPosition(m_current_hub->getPosition());

	//Collision between cursor and stellarmap elements
	StellarBranch* highlighted_branch = NULL;
	for (StellarBranch* branch : m_branches)
	{
		if (branch->m_hub != NULL)//check hubs first
		{
			branch->m_hub->m_feedback_state = StellarComponent_NormalState;
			branch->m_hub->setFillColor(sf::Color::Red);

			if (highlighted_branch == NULL && SFPanel::IsCursorCollidingWithRectangle(m_cursor, *branch->m_hub) == true)
			{
				highlighted_branch = branch;
				branch->m_hub->setFillColor(sf::Color::Yellow);
			}
			else
			{
				branch->m_hub->setFillColor(sf::Color::Red);
			}
		}
	}

	for (StellarBranch* branch : m_branches)
	{
		if (branch->m_hub == NULL)//check segments in second
		{
			for (StellarSegment* segment : branch->m_segments)
			{
				segment->setFillColor(sf::Color::White);
				
				if (highlighted_branch == NULL && SFPanel::IsCursorCollidingWithRectangle(m_cursor, *segment) == true)
					highlighted_branch = branch;
			}

			if (highlighted_branch != NULL && highlighted_branch == branch)
				for (StellarSegment* segment : branch->m_segments)
					segment->setFillColor(sf::Color::Yellow);
		}
	}
	
	//info panel
	if (highlighted_branch == NULL)
	{
		delete m_info_panel;
		m_info_panel = NULL;
	}
	else if (highlighted_branch != m_hightlighted_branch)
	{
		delete m_info_panel;
		if (highlighted_branch->m_hub != NULL)
			m_info_panel = new SFStellarInfoPanel(highlighted_branch->m_hub, 0, sf::Vector2f(STELLARMAP_INFO_PANEL_SIZE_X, STELLARMAP_INFO_PANEL_SIZE_Y), m_playership);
		else
			m_info_panel = new SFStellarInfoPanel(highlighted_branch->m_segments[0], sf::Vector2f(STELLARMAP_INFO_PANEL_SIZE_X, STELLARMAP_INFO_PANEL_SIZE_Y), m_playership);
	}

	m_hightlighted_branch = highlighted_branch;

	//Update cursor state
	if (m_hightlighted_branch == NULL)
		m_cursor.setAnimationLine(Cursor_NormalState);
	else
		m_cursor.setAnimationLine(Cursor_HighlightState);
}

void SFMapPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		screen.draw(m_text);

		if (m_actions)
		{
			m_actions->Draw(screen);
		}

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
		GameObject::NormalizeVector(&speed, STELLARMAP_SCROLLING_SPEED);
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
	float left_border = - getSize().x / 2 + m_map_content_area.left + m_current_hub->m_coordinates.x * STELLARMAP_SCALE + STELLARMAP_MARGIN_SIDES_WIDTH;
	float right_border = getSize().x / 2 - (m_map_content_area.left + m_map_content_area.width - m_current_hub->m_coordinates.x * STELLARMAP_SCALE) - STELLARMAP_MARGIN_SIDES_WIDTH;
	float up_border = getSize().y / 2 - m_map_content_area.top - m_current_hub->m_coordinates.y * STELLARMAP_SCALE + STELLARMAP_MARGIN_SIDES_WIDTH;
	float down_border = -getSize().y / 2 + (m_map_content_area.top + m_map_content_area.height - m_current_hub->m_coordinates.y * STELLARMAP_SCALE) - STELLARMAP_MARGIN_SIDES_WIDTH;

	if (m_scroll_offset.x < left_border)
		m_scroll_offset.x = left_border;
	
	if (m_scroll_offset.x > right_border)
		m_scroll_offset.x = right_border;
	
	if (m_scroll_offset.y > up_border)
		m_scroll_offset.y = up_border;
	
	if (m_scroll_offset.y < down_border)
		m_scroll_offset.y = down_border;
}

void SFMapPanel::UpdateBranchesPosition(bool into_real_coordinates, bool into_fake_coordinates)
{ 
	if (!m_branches.empty())
	{
		size_t branchesVectorSize = m_branches.size();
		for (size_t i = 0; i < branchesVectorSize; i++)
		{
			sf::Vector2f pos = sf::Vector2f(getPosition().x + m_scroll_offset.x - m_current_hub->m_coordinates.x * STELLARMAP_SCALE, getPosition().y + m_scroll_offset.y + m_current_hub->m_coordinates.y * STELLARMAP_SCALE);
			if (into_real_coordinates == true)
				m_branches[i]->SetPosition(SFStellarInfoPanel::GetRealCoordinates(pos, getPosition(), getSize()));
			else if (into_fake_coordinates == true)
				m_branches[i]->SetPosition(SFStellarInfoPanel::GetFakeCoordinates(pos, getPosition(), getSize()));
			else
				m_branches[i]->SetPosition(pos);
		}
	}
}

int SFMapPanel::ComputeTeleportationCost(StellarHub* destination)
{
	if (destination && m_current_hub)
	{
		int diff_x = m_current_hub->m_coordinates.x - destination->m_coordinates.x;
		int diff_y = m_current_hub->m_coordinates.y - destination->m_coordinates.y;
		if (diff_x < 0)
		{
			diff_x = -diff_x;
		}
		if (diff_y < 0)
		{
			diff_y = -diff_y;
		}

		int cost = (diff_x + diff_y) * (diff_x + diff_y) * STELLARMAP_TELEPORTATION_COST;//exponential cost: distance as the crow flies ^4)
		//int cost = (diff_x + diff_y) * (*CurrentGame).GetEnemiesStatsMultiplierForLevel(destination->m_level) * 0.01 * STELLARMAP_TELEPORTATION_COST;

		return cost;
	}
	else
	{
		return -1;
	}
}

bool SFMapPanel::IsSceneAlreadyChecked(string new_scene, bool add_if_not_checked)
{
	size_t checkedScenesVectorSize = m_checked_scenes.size();
	for (size_t i = 0; i < checkedScenesVectorSize; i++)
		if (m_checked_scenes[i].compare(new_scene) == 0)
			return true;

	//element not found
	if (add_if_not_checked)
		m_checked_scenes.push_back(new_scene);
	
	return false;
}

bool SFMapPanel::IsSceneKnownByThePlayer(string new_scene)
{
	size_t knownScenesVectorSize = m_playership->m_knownScenes.size();

	map<string, int>::iterator it = m_playership->m_knownScenes.find(new_scene);

	//if scene not already known
	return it != m_playership->m_knownScenes.end();
}

int SFMapPanel::GetMaxHazardLevelUnlocked(string new_scene)
{
	size_t knownScenesVectorSize = m_playership->m_knownScenes.size();

	map<string, int>::iterator it = m_playership->m_knownScenes.find(new_scene);

	//if scene not already known
	if (it != m_playership->m_knownScenes.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}