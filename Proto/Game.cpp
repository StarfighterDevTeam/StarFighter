#include "Game.h"

void Game::init(RenderWindow* window)
{
	SEGame::init(window);

	//fonts
	m_font[Font_Terminator] = new sf::Font();
	if (!m_font[Font_Terminator]->loadFromFile(makePath("Fonts/terminator_real_nfi.ttf")))
	{
		// error
		//TODO: font loader
	}

	m_font[Font_Arial] = new sf::Font();
	if (!m_font[Font_Arial]->loadFromFile(makePath("Fonts/arial.ttf")))
	{
		// error
		//TODO: font loader
	}

	//Sounds
	LoadSFX();

	//Music
	LOGGER_WRITE(Logger::DEBUG, "Loading Musics");
	m_Music_Activated = true;
	m_music_fader = 0;
	m_asking_music_fade_out = false;
	PlayMusic(Music_Main);
}

int Game::LoadSFX()
{
	if (!m_soundBuffers[0].loadFromFile(makePath("Sounds/laser.ogg")))
		return -1;

	m_soundsLaser[0].setBuffer(m_soundBuffers[0]);

	m_soundsLaser[0].setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);
	//soundsSwitch.setVolume(DEFAULT_SFX_VOLUME * m_SFX_Activated);

	return 0;
}
