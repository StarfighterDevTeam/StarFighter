#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::White , sf::Vector2f(64, 64));
	//(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);
	(*CurrentGame).m_playerShip->setColor(sf::Color(255, 255, 255, 0));

	//init scenario
	ResetGame();
}

Gameloop::~Gameloop()
{
	
}

void Gameloop::Update(sf::Time deltaTime)
{
	//AI metrics - clock
	(*CurrentGame).m_clock += deltaTime.asSeconds();

	//Get mouse & keyboard inputs
	(*CurrentGame).m_hovered_entities.clear();
	(*CurrentGame).GetMouseInputs(deltaTime);
	if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		for (vector<L16Entity*>::iterator it = (*CurrentGame).m_selected_entities.begin(); it != (*CurrentGame).m_selected_entities.end(); it++)
		{
			(*it)->m_selected = false;
			(*it)->ResetColor();
		}
		(*CurrentGame).m_selected_entities.clear();
	}

	//Update objects
	(*CurrentGame).updateScene(deltaTime);

	//Create new links on right click
	L16Entity* selected = NULL;
	L16Entity* hovered = NULL;
	if ((*CurrentGame).m_selected_entities.size() == 1)
		selected = (*CurrentGame).m_selected_entities.front();

	if ((*CurrentGame).m_hovered_entities.size() == 1)
		hovered = (*CurrentGame).m_hovered_entities.front();

	if (hovered != NULL && selected != NULL && (*CurrentGame).m_mouse_click == Mouse_RightClick && selected != hovered && selected->m_alliance == PlayerAlliance && hovered->m_alliance == PlayerAlliance && hovered->m_L16_type == L16Entity_Node && selected->m_L16_type == L16Entity_Node)
	{
		//link already existing?
		bool found = false;
		Node* hovered_node = (Node*)hovered;
		Node* selected_node = (Node*)selected;
		for (vector<L16Entity*>::iterator it = hovered_node->m_linked_entities.begin(); it != hovered_node->m_linked_entities.end(); it++)
		{
			if ((*it) == selected_node)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			CreateLink(hovered_node, selected_node);
		}
	}

	//Scroll camera
	UpdateCamera(deltaTime);

	//L16 AI : win/lose conditions
	if (m_wing->m_visible == true)
	{
		printf("GAME OVER\n");
		ResetGame();
	}
	else if (GetDistanceBetweenPositions(m_wing->getPosition(), m_finish->getPosition()) < m_finish->getRadius())
	{
		printf("WIN (SCORE: %d): shots: %d, time: %d\n", (int)(*CurrentGame).m_clock + (*CurrentGame).m_shots_fired, (*CurrentGame).m_shots_fired, (int)(*CurrentGame).m_clock);
		ResetGame();
	}
}

void Gameloop::ResetGame()
{
	//clean previous scenario
	for (int i = 0; i < NB_ALLIANCE_TYPES; i++)
		for (int j = 0; j < NB_CIRCLE_TYPES; j++)
		{
			for (CircleObject* object : (*CurrentGame).m_sceneCircleObjects[i][j])
				delete object;

			(*CurrentGame).m_sceneCircleObjects[i][j].clear();
		}

	(*CurrentGame).m_L16_entities.clear();
	(*CurrentGame).m_waves.clear();

	//entities
	m_wing = CreateWing(sf::Vector2f(500, 400), PlayerAlliance, 0);
	m_wing->m_selected = true;

	m_finish = CreateTerminal(sf::Vector2f(1920 - 100, 200), PlayerAlliance);

	CreateTerminal(sf::Vector2f(1920 - 700, 700), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 700, 400), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 700, 200), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 700, 900), EnemyAlliance);
	
	CreateTerminal(sf::Vector2f(1920 - 400, 0), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 400, 200), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 400, 400), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 400, 700), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 400, 900), EnemyAlliance);
	CreateTerminal(sf::Vector2f(1920 - 400, 1100), EnemyAlliance);

	//AI metrics
	(*CurrentGame).m_clock = 0;
	(*CurrentGame).m_shots_fired = 0;
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

//Liaison 16
Node* Gameloop::CreateNode(sf::Vector2f position, AllianceType alliance)
{
	Node* node = new Node(position, alliance);
	(*CurrentGame).AddCircleObject(node);

	return node;
}

Terminal* Gameloop::CreateTerminal(sf::Vector2f position, AllianceType alliance)
{
	Terminal* terminal = new Terminal(position, alliance);
	(*CurrentGame).AddCircleObject(terminal);

	return terminal;
}

Link* Gameloop::CreateLink(Node* node_a, Node* node_b)
{
	Link* link = new Link(node_a, node_b);
	(*CurrentGame).AddLineObject(link);
	(*CurrentGame).m_links.push_back(link);

	node_a->m_linked_entities.push_back(node_b);
	node_b->m_linked_entities.push_back(node_a);

	return link;
}

Wing* Gameloop::CreateWing(sf::Vector2f position, AllianceType alliance, float heading)
{
	Wing* wing = new Wing(position, alliance, heading);
	(*CurrentGame).AddCircleObject(wing);
	
	return wing;
}