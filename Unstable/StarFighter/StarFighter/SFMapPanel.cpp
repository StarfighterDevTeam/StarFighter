#include "SFMapPanel.h"

extern Game* CurrentGame;

//COMPONENTS
StellarHub::StellarHub()
{
	setSize(sf::Vector2f(16, 16));
	setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	setFillColor(sf::Color::Red);

	m_distance_to_current_position = 0;
	m_coordinates = sf::Vector2f(0, 0);
}

StellarHub::StellarHub(string hub_name) : StellarHub()
{
	m_display_name = hub_name;
}

StellarNode::StellarNode()
{
	setRadius(4);
	setOrigin(sf::Vector2f(getRadius(), getRadius()));
	setFillColor(sf::Color::Red);

	m_coordinates = sf::Vector2f(0, 0);
}

StellarSegment::StellarSegment(bool vertical, float segment_size)
{
	m_size_on_stellar_map = segment_size;
	sf::Vector2f size = vertical ? sf::Vector2f(STELLAR_SEGMENT_THICKNESS, STELLARMAP_SCALE * segment_size) : sf::Vector2f(STELLARMAP_SCALE * segment_size, STELLAR_SEGMENT_THICKNESS);
	setSize(size);
	setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	setFillColor(sf::Color::White);

	m_coordinates = sf::Vector2f(0, 0);
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

//MAP PANEL
SFMapPanel::SFMapPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Map)
{
	m_playerShip = playerShip;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	setOutlineThickness(2);
	
	//panel position and color
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));
	setFillColor(sf::Color(20, 20, 20, 240));//dark grey

	//CONSTRUCTION OF THE MAP
	//Loading the list of all scenes, contained in SCENES_FILE
	m_scenesConfig = *(FileLoaderUtils::FileLoader(SCENES_FILE));
	m_scenesConfigSize = m_scenesConfig.size();

	//start with current hub
	StellarBranch* mother_branch = new StellarBranch();
	m_current_hub = new StellarHub();
	mother_branch->m_hub = m_current_hub;
	mother_branch->m_hub->m_display_name = (*CurrentGame).m_currentScene->m_name;
	mother_branch->SetPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));
	m_branches.push_back(mother_branch);
	//m_checked_hubs.push_back(m_current_hub);

	//scan branches around
	ScanBranches(mother_branch->m_hub->m_display_name, NO_DIRECTION, sf::Vector2f(0,0));

	//set position of all elements, based on their computed coordinates
	UpdateBranchesPosition();
}

SFMapPanel::~SFMapPanel()
{
	
}

void SFMapPanel::UpdateBranchesPosition()
{
	if (!m_branches.empty())
	{
		size_t branchesVectorSize = m_branches.size();
		for (size_t i = 0; i < branchesVectorSize; i++)
		{
			m_branches[i]->SetPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));
		}
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
				if (IsSceneAlreadyChecked(links[direction],  false))
				{
					//scene has already been scanned
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

void SFMapPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{

}

void SFMapPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		//screen.draw(m_title_text);

		//specific content to be drawn on a separate RenderTexture
		if (!m_branches.empty())
		{
			size_t branchesVectorSize = m_branches.size();
			for (size_t i = 0; i < branchesVectorSize; i++)
			{
				//m_branches[i]->Draw(screen);
				m_branches[i]->DrawSegments(screen);
			}
			for (size_t i = 0; i < branchesVectorSize; i++)
			{
				//m_branches[i]->Draw(screen);
				m_branches[i]->DrawNodes(screen);
				m_branches[i]->DrawHub(screen);
			}
		}
	}
}