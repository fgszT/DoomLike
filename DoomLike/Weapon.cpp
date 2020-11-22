#include "Weapon.h"

void Weapon::Draw(sf::RenderWindow& window)
{
	sf::Sprite weapon;
	weapon.setTexture(this->loadTexture());
	weapon.setScale(sf::Vector2f(0.3 * window.getSize().x / 800, 0.3 * window.getSize().x / 800));
	weapon.setPosition(window.getSize().x / 2, window.getSize().y / 2);
	window.draw(weapon);
}

sf::Texture& Weapon::loadTexture()
{
	if (texture_loaded)
	{
		return T_texture;
	}
	else
	{
		texture_loaded = true;
		if (!T_texture.loadFromFile(s_texture))
		{
			texture_loaded = false;
		}
		T_texture.setRepeated(true);
		return T_texture;
	}
}