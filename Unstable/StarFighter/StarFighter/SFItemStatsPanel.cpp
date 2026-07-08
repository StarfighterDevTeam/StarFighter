#include "SFItemStatsPanel.h"

extern Game* CurrentGame;

//ITEMS STATS PANEL
SFItemStatsPanel::SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playership, FocusedItemStates item_state, GameObject* comparison_object) : SFPanel(size, SFPanel_ItemStats)
{
	//NOTE: this panel's layout/content logic is currently disabled (feature not wired up), but
	//m_comparison_object must still be initialized since it is read (unconditionally) in Draw().
	m_comparison_object = NULL;
}

SFItemStatsPanel::~SFItemStatsPanel()
{

}

void SFItemStatsPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		if (!m_title_text_comparison.getString().isEmpty())
		{
			screen.draw(m_title_text_comparison);
		}
		screen.draw(m_text);
		
		if (m_actions)
		{
			m_actions->Draw(screen);
		}

		if (m_comparison_object != NULL)
		{
			screen.draw(m_overblock);
		}

		//for (size_t i = 0; i < 2; i++)
		//{
		//	screen.draw(m_options_text[i]);
		//}
		//for (size_t i = 0; i < m_number_of_options; i++)
		//{
		//	screen.draw(m_options_text[i]);
		//	screen.draw(m_buttons[i]);
		//}
	}
}

void SFItemStatsPanel::DisplayItemStats(GameObject* object)
{
	//NOTE: disabled along with the constructor above (feature not currently wired up).
}