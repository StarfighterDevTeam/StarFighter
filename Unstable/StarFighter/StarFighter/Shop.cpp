#include "Shop.h"

extern Game* CurrentGame;

Shop::Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_frameNumber, int m_animationNumber) : GameObject(position, speed, textureName, size, sf::Vector2f(0.5 * size.x, 0.5 * size.y), m_frameNumber, m_animationNumber)
{
	m_collider_type = ShopObject;
	m_layer = PortalLayer;

	m_DontGarbageMe = true;

	m_grid_v2 = new Grid(sf::Vector2f(0, 0), sf::Vector2i(NBVAL_Equipment + 1, SHOP_GRID_NB_LINES), NULL);
}

Shop::~Shop()
{
	//Shop::SaveShop(NULL);//refresh shop content by saving an empty shop

	Shop::SaveShopUpgrades(NULL);

	delete m_grid_v2;
}

void Shop::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	AnimatedSprite::update(deltaTime);
}

int Shop::SaveShopUpgrades(Shop* shop)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving shop upgrades in profile.\n");

	ofstream data(string(getSavesPath()) + SHOP_UPGRADES_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		if (shop == NULL)
		{
			data << "-1";
			data << endl;
		}
		else
		{
			for (int i = 0; i < NB_UPGRADE_CHOICES; i++)
			{
				data << shop->m_upgrades[i] << " " << (int)shop->m_sold_out[i];
				data << endl;
			}
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "ERROR: No save file found for shop upgrades. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Shop::LoadShopUpgrades(Shop* shop)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading shop from profile.\n");

	if (shop == NULL)
		return false;

	std::ifstream data(string(getSavesPath()) + SHOP_UPGRADES_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		//load save
		std::string line;
		int i = 0;
		while (std::getline(data, line))
		{
			if (i == NB_UPGRADE_CHOICES)
			{
				printf("ERROR: loading more Shop upgrades from saved file that authorized (%d). Check the save file.\n", i);
				break;
			}

			string upgrade_name;
			bool sold_out;
			std::istringstream(line) >> upgrade_name >> (bool)sold_out;

			if (upgrade_name.compare("-1") == 0)
				return false;

			shop->m_upgrades[i] = upgrade_name;
			shop->m_sold_out[i] = sold_out;

			i++;
		}

		data.close();  // on ferme le fichier

		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "No SHOP UPGRADES SAVE FILE found.\n" << endl;
		return false;
	}
}