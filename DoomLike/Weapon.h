#ifndef DOOMSTRIKE_WEAPON_H
#define DOOMSTRIKE_WEAPON_H

#include <SFML/Graphics.hpp>
#include "settings.h"

class Weapon
{
protected:
	sf::Texture T_texture;
	std::string s_texture;
	bool texture_loaded = false;
public:
	virtual void Draw(sf::RenderWindow& window);

	sf::Texture& loadTexture();
};

#endif