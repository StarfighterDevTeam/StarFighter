#include "Shop.h"

extern Game* CurrentGame;

Shop::Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber, int m_animationNumber) : GameObject(position, speed, textureName, size, origin, m_frameNumber, m_animationNumber)
{
	m_collider_type = ShopObject;
	m_layer = PortalLayer;

	m_DontGarbageMe = true;
	m_level = 1;

	m_grid_v2 = new Grid(sf::Vector2f(0, 0), sf::Vector2i(NBVAL_Equipment + 1, SHOP_GRID_NB_LINES), NULL);
}

Shop::~Shop()
{
	Shop::SaveShop(NULL);//refresh shop content by saving an empty shop

	delete m_grid_v2;
}

void Shop::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	AnimatedSprite::update(deltaTime);
}

int Shop::SaveShop(Shop* shop)
{
	return 0;
	LOGGER_WRITE(Logger::DEBUG, "Saving shop in profile.\n");

	ofstream data(string(getSavesPath()) + SHOP_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		if (shop == NULL)
		{
			data << "-1";
			data << endl;
		}
		else
		{
			for (GridElement* element : shop->m_grid_v2->m_elements)
			{
				if (element->m_object == NULL)
				{
					data << "0";
					data << endl;
				}
				else if (element->m_object->m_equipment_loot != NULL)
					Enemy::SaveEquipmentData(data, element->m_object->m_equipment_loot, false);
				else
					Enemy::SaveWeaponData(data, element->m_object->m_weapon_loot, false);
			}
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "ERROR: No save file found for shop items. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Shop::LoadShop(Shop* shop)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading shop from profile.\n");

	if (shop == NULL)
		return false;

	std::ifstream  data(string(getSavesPath()) + SHOP_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		//load save
		std::string line;
		int i = 0;
		while (std::getline(data, line))
		{
			string equipment_type;
			string display_name;
			std::istringstream(line) >> equipment_type;

			//Checking if a shop was saved
			bool checked_content_presence = false;
			if (checked_content_presence == false && equipment_type.compare("-1") == 0)
				return false;
			else
				checked_content_presence = true;

			//Loading shop
			if (equipment_type.compare("0") != 0)
			{
				if (equipment_type.compare("Weapon") == 0)
				{
					Weapon* weapon = Enemy::LoadSavedWeaponFromLine(line);
					GameObject* capsule = Enemy::CloneWeaponIntoGameObject(weapon);
					shop->m_grid_v2->InsertObject(capsule, i, false);
					delete weapon;
				}
				else
				{
					Equipment* equipment = Enemy::LoadSavedEquipmentFromLine(line);
					GameObject* capsule = Enemy::CloneEquipmentIntoGameObject(equipment);
					shop->m_grid_v2->InsertObject(capsule, i, false);
					delete equipment;
				}
			}

			i++;
		}

		data.close();  // on ferme le fichier

		return i == SHOP_GRID_NB_ROWS * SHOP_GRID_NB_LINES;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "No SHOP SAVE FILE found.\n" << endl;
		return false;
	}
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