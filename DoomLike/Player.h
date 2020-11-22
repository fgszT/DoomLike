#ifndef DOOMSTRIKE_PLAYER_H
#define DOOMSTRIKE_PLAYER_H

#include <Windows.h>
#undef min
#undef max

#include <cmath>
#include <array>
#include "settings.h"
#include "Object2D.h"
#include "Gun.h"

struct SRayCast
{
	double d_distance;
	int objectNum;
	double texturePosition;
};

class Player
{
private:
	int world_length;
	int world_width;

	Point2D p_position;
	double d_direction;
	double d_fieldOfView;

	int distancesSegments;
	std::vector<SRayCast> v_distances;

	double d_walkSpeed;
	sf::Vector2i localMousePosition;

	sf::Texture T_skyTexture;
	sf::Texture T_floorTexture;

	std::array<Weapon*, 9> a_weapons;
	int currentWeapon;

	bool b_textures;
	bool b_skyTexture;
	bool b_floorTexture;

	bool noclip = false;
public:
	Player();
	Player(Point2D _position, double _direction, const int& _world_length, const int& _world_width, int _distancesSegments);
	Player(double _x, double _y, double _direction, const int& _world_length, const int& _world_width, int _distancesSegments);

	void UpdateDistances(const std::vector<Object2D*>& v_objects);

	bool PlayerControl(sf::RenderWindow& window, const std::vector<Object2D*>& v_objects);

	void Draw(sf::RenderWindow& window);
	void DrawPlayerView(sf::RenderWindow& window, const std::vector<Object2D*>& v_objects);

	void setWalkSpeed(double _walkSpeed);
	void setBTextures(bool _b_textures);
};

#endif