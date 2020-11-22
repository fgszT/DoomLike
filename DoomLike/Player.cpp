#include "Player.h"

Player::Player()
{
	world_length = 0;
	world_width = 0;
	p_position = 0;
	d_direction = 0;
	d_fieldOfView = PI / 2;
	distancesSegments = 0;
	d_walkSpeed = 0;
	currentWeapon = 0;
}

Player::Player(Point2D _position, double _direction, const int& _world_length, const int& _world_width, int _distancesSegments)
{
	world_length = _world_length;
	world_width = _world_width;
	p_position = _position;
	d_direction = _direction;
	d_fieldOfView = PI / 2;
	distancesSegments = _distancesSegments;
	d_walkSpeed = 1;
	currentWeapon = 1;
	a_weapons[1] = new Gun();
}

Player::Player(double _x, double _y, double _direction, const int& _world_length, const int& _world_width, int _distancesSegments)
{
	Point2D _position = { 0, 0 };
	world_length = _world_length;
	world_width = _world_width;
	p_position = _position;
	d_direction = _direction;
	d_fieldOfView = PI / 2;
	distancesSegments = _distancesSegments;
	d_walkSpeed = 1;
	currentWeapon = 1;
	a_weapons[1] = new Gun();
}

int sign(double number)
{
	return number >= 0 ? 1 : -1;
}

double cross(Point2D p1, Point2D p2)
{
	return p1.x * p2.y - p1.y * p2.x;
}

double SegmentLength(const Point2D& pointA, const Point2D& pointB)
{
	return (std::sqrt(std::pow(pointA.x - pointB.x, 2) + std::pow(pointA.y - pointB.y, 2)));
}

bool areCrossing(std::pair<Point2D, Point2D>& segment1, std::pair<Point2D, Point2D>& segment2, double& distance, double& texturePosition, double _world_length)
{
	Point2D cut1 = segment1.second - segment1.first;
	Point2D cut2 = segment2.second - segment2.first;
	double prod1;
	double prod2;

	prod1 = cross(cut1, (segment2.first - segment1.first));
	prod2 = cross(cut1, (segment2.second - segment1.first));
	if (sign(prod1) == sign(prod2) || (prod1 == 0) || (prod2 == 0))
	{
		return false;
	}

	prod1 = cross(cut2, (segment1.first - segment2.first));
	prod2 = cross(cut2, (segment1.second - segment2.first));
	if (sign(prod1) == sign(prod2) || (prod1 == 0) || (prod2 == 0))
	{
		return false;
	}

	double x = (segment1.first.x + cut1.x * std::abs(prod1) / std::abs(prod2 - prod1));
	double y = (segment1.first.y + cut1.y * std::abs(prod1) / std::abs(prod2 - prod1));

	distance = std::sqrt(std::pow(segment1.first.x - x, 2) + std::pow(segment1.first.y - y, 2));
	texturePosition = std::sqrt(std::pow(segment2.first.x - x, 2) + std::pow(segment2.first.y - y, 2));

	return true;
}

double CrossingDistance(std::pair<Point2D, Point2D>& directionSegment, const std::vector<Object2D*>& v_objects, int& objectNum, double& texturePosition, double _world_length)
{
	double distance = MAX_DISTANCE;
	int index = 0;
	for (auto object : v_objects)
	{
		for (int i = 0; i < object->getVPointsSize(); i++)
		{
			double tmpDistance;
			double tmpTexturePosition;
			std::pair<Point2D, Point2D> objectSegment = { object->getPoint(i), object->getPoint(i + 1) };
			if (areCrossing(directionSegment, objectSegment, tmpDistance, tmpTexturePosition, _world_length))
			{
				if (tmpDistance < distance)
				{
					distance = tmpDistance;
					objectNum = index;
					texturePosition = tmpTexturePosition;
				}
			}
		}
		index++;
	}
	return distance;
}

void Player::UpdateDistances(const std::vector<Object2D*>& v_objects)
{
	v_distances.clear();
	double left = d_direction - d_fieldOfView / 2;
	double right = d_direction + d_fieldOfView / 2;
	for (int i = 0; i < distancesSegments; i++)
	{
		int objectNum = -1;
		double texturePosition = 0;
		double direction = d_direction + ((double)i / distancesSegments - 0.5) * d_fieldOfView;
		std::pair<Point2D, Point2D> directionSegment = { p_position, { p_position.x + (double)MAX_DISTANCE * cos(direction), p_position.y + (double)MAX_DISTANCE * sin(direction) } };
		double distance = CrossingDistance(directionSegment, v_objects, objectNum, texturePosition, world_length);
		SRayCast ray = { distance, objectNum, texturePosition };
		v_distances.push_back(ray);
	}
}

double dot(Point2D vectorA, Point2D vectorB)
{
	return vectorA.x * vectorB.x + vectorA.y * vectorB.y;
}

Point2D newWalkPoint(std::pair<Point2D, Point2D>& segment1, std::pair<Point2D, Point2D>& segment2)
{
	Point2D point;

	double x = 0;
	double y = 0;

	if (dot({ segment1.second - segment2.first }, { segment2.second - segment2.first }) <= 0)
	{
		x = segment2.first.x;
		y = segment2.first.y;
	}
	else
	{
		if (dot({ segment1.second - segment2.second }, { segment2.first - segment2.second }) <= 0)
		{
			x = segment2.second.x;
			y = segment2.second.y;
		}
		else
		{
			if (segment2.first.x == segment2.second.x)
			{
				x = segment2.first.x;
				y = segment1.second.y;
			}
			else
			{
				if (segment2.first.y == segment2.second.y)
				{
					x = segment1.second.x;
					y = segment2.first.y;
				}
				else
				{
					x = (segment2.first.x * pow(segment2.second.y - segment2.first.y, 2) + segment1.second.x * pow(segment2.second.x - segment2.first.x, 2) + (segment2.second.x - segment2.first.x) * (segment2.second.y - segment2.first.y) * (segment1.second.y - segment2.first.y)) / (pow(segment2.second.y - segment2.first.y, 2) + pow(segment2.second.x - segment2.first.x, 2));
					y = ((segment2.second.x - segment2.first.x) * (segment1.second.x - x)) / (segment2.second.y - segment2.first.y) + segment1.second.y;
				}
			}
		}
	}


	point.x = x + (x - segment1.second.x) * 0.25;
	point.y = y + (y - segment1.second.y) * 0.25;
	return point;
}

bool areCrossing(std::pair<Point2D, Point2D>& segment1, std::pair<Point2D, Point2D>& segment2, Point2D& walkPoint, double& distance)
{
	Point2D cut1 = segment1.second - segment1.first;
	Point2D cut2 = segment2.second - segment2.first;
	double prod1;
	double prod2;

	prod1 = cross(cut1, (segment2.first - segment1.first));
	prod2 = cross(cut1, (segment2.second - segment1.first));
	if (sign(prod1) == sign(prod2) || (prod1 == 0) || (prod2 == 0))
	{
		return false;
	}

	prod1 = cross(cut2, (segment1.first - segment2.first));
	prod2 = cross(cut2, (segment1.second - segment2.first));
	if (sign(prod1) == sign(prod2) || (prod1 == 0) || (prod2 == 0))
	{
		return false;
	}

	double x = (segment1.first.x + cut1.x * std::abs(prod1) / std::abs(prod2 - prod1));
	double y = (segment1.first.y + cut1.y * std::abs(prod1) / std::abs(prod2 - prod1));

	distance = SegmentLength(segment1.first, { x, y });

	walkPoint = newWalkPoint(segment1, segment2);

	return true;
}

bool Player::PlayerControl(sf::RenderWindow& window, const std::vector<Object2D*>& v_objects)
{
	double dx = 0;
	double dy = 0;

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		dx += sin(d_direction);
		dy += -cos(d_direction);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		dx += -sin(d_direction);
		dy += cos(d_direction);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		dx += cos(d_direction);
		dy += sin(d_direction);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		dx += -cos(d_direction);
		dy += -sin(d_direction);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		std::exit(0);
		//return false;
	}
	if (sf::Mouse::getPosition(window).x != localMousePosition.x)
	{
		float difference = sf::Mouse::getPosition(window).x - localMousePosition.x;
		sf::Mouse::setPosition({ (int)(screen_width / 2), (int)(screen_height / 2) });
		localMousePosition = sf::Mouse::getPosition(window);
		d_direction += 0.005 * difference;
	}

	Point2D walkPoint = { p_position.x + d_walkSpeed * dx, p_position.y + d_walkSpeed * dy };
	if (!noclip) //� ����� �������� ������ �����
	{
		double distance = SegmentLength(p_position, walkPoint);
		std::pair<Point2D, Point2D> walkSegment = { p_position, walkPoint };
		for (auto object : v_objects)
		{
			for (int i = 0; i < object->getVPointsSize(); i++)
			{
				Point2D tmpWalkPoint = walkPoint;
				double tmpDistance = distance;
				std::pair<Point2D, Point2D> objectSegment = { object->getPoint(i), object->getPoint(i + 1) };
				if (areCrossing(walkSegment, objectSegment, tmpWalkPoint, tmpDistance))
				{
					if (tmpDistance < distance)
					{
						distance = tmpDistance;
						walkPoint = tmpWalkPoint;
					}
				}
			}
		}
	}
	p_position = walkPoint;

	return true;
}

void Player::Draw(sf::RenderWindow& window)
{
	sf::CircleShape circle;
	sf::ConvexShape triangle;

	circle.setPosition((p_position.x - PLAYER_RADIUS) / world_length * window.getSize().x * SCALE, (p_position.y - PLAYER_RADIUS) / world_length * window.getSize().x * SCALE);

	circle.setRadius((double)PLAYER_RADIUS / world_length * window.getSize().x * SCALE);
	circle.setFillColor(FILL_PLAYER_COLOR);
	circle.setOutlineColor(OUTLINE_PLAYER_COLOR);
	circle.setOutlineThickness((double)OUTLINE_PLAYER_THICKNESS / world_length * window.getSize().x * SCALE);

	double left = d_direction - d_fieldOfView / 2;
	double right = d_direction + d_fieldOfView / 2;

	triangle.setPointCount(CONVEX_NUMBER + 2);

	triangle.setPoint(0, sf::Vector2f(0, 0));
	triangle.setPoint(1, sf::Vector2f((v_distances[0].d_distance * cos(left)) / world_length * window.getSize().x * SCALE, (v_distances[0].d_distance * sin(left)) / world_length * window.getSize().x * SCALE));
	triangle.setPoint(CONVEX_NUMBER + 1, sf::Vector2f((v_distances[distancesSegments - 1].d_distance * cos(right)) / world_length * window.getSize().x * SCALE, (v_distances[distancesSegments - 1].d_distance * sin(right)) / world_length * window.getSize().x * SCALE));
	for (int i = 1; i < CONVEX_NUMBER; i++)
	{
		triangle.setPoint(i + 1, sf::Vector2f((v_distances[(int)(distancesSegments / CONVEX_NUMBER * i)].d_distance * cos(left + (right - left) * i / CONVEX_NUMBER)) / world_length * window.getSize().x * SCALE, (v_distances[(int)(distancesSegments / CONVEX_NUMBER * i)].d_distance * sin(left + (right - left) * i / CONVEX_NUMBER)) / world_length * window.getSize().x * SCALE));
	}

	triangle.setOutlineColor(OUTLINE_CAMERA_COLOR);
	triangle.setFillColor(FILL_CAMERA_COLOR);
	triangle.setOutlineThickness(OUTLINE_CAMERA_THICKNESS);
	triangle.setPosition(p_position.x / world_length * window.getSize().x * SCALE, p_position.y / world_length * window.getSize().x * SCALE);

	window.draw(triangle);
	window.draw(circle);
}

void Player::DrawPlayerView(sf::RenderWindow& window, const std::vector<Object2D*>& v_objects)
{

	if (b_textures)
	{
		double direction = d_direction - d_fieldOfView / 2;

		sf::Sprite sky;
		if (!b_skyTexture)
		{
			T_skyTexture.setRepeated(true);
			T_skyTexture.loadFromFile(SKY_TEXTURE);
			b_skyTexture = true;
		}
		sky.setTexture(T_skyTexture);
		sky.setTextureRect(sf::IntRect(d_direction * window.getSize().x / 2, 0, window.getSize().x, window.getSize().y / 2));
		sky.setPosition(sf::Vector2f(0, 0));
		window.draw(sky);

		/*sf::Sprite floor;
		floor.setTextureRect(sf::IntRect(0, 0, window.getSize().x, window.getSize().y / 2));
		floor.setPosition(0, window.getSize().y / 2);
		floor.setColor({ 0, 0, 0 });
		window.draw(floor);*/

		/*
		sf::Sprite floor;
		if (!b_floorTexture)
		{
			T_floorTexture.setRepeated(true);
			T_floorTexture.loadFromFile(FLOOR_TEXTURE);
			b_floorTexture = true;
		}
		floor.setTexture(T_floorTexture);
		floor.setTextureRect(sf::IntRect(0, 0, window.getSize().x, window.getSize().y / 5));
		floor.setPosition(0, window.getSize().y / 2);
		//floor.setScale(1, 0.5);
		window.draw(floor);*/


		for (int i = 0; i < v_distances.size() - 1; i++)
		{
			double heightLeftTop = (1 - 5 / (v_distances[i].d_distance * cos(direction - d_direction))) * window.getSize().y / 2;
			double heightLeftBottom = (1 + 5 / (v_distances[i].d_distance * cos(direction - d_direction))) * window.getSize().y / 2;

			double scaleFactor = (double)(heightLeftBottom - heightLeftTop) / window.getSize().y;
			sf::Sprite sprite;
			if (v_distances[i].objectNum >= 0)
			{
				sprite.setTexture(v_objects[v_distances[i].objectNum]->loadTexture(b_textures));
			}
			sprite.setTextureRect(sf::IntRect(v_distances[i].texturePosition * 150, 0, ceil(window.getSize().x / distancesSegments + 0.5), window.getSize().y)); // ����� ����������� �����, � �� 150
			sprite.setPosition(sf::Vector2f((double)i * window.getSize().x / distancesSegments, heightLeftTop));
			sprite.scale(1, scaleFactor);

			window.draw(sprite);
		}

		if (currentWeapon > 0 && a_weapons[currentWeapon] != NULL)
		{
			a_weapons[currentWeapon]->Draw(window);
		}
	}
	else
	{
		double direction = d_direction - d_fieldOfView / 2;

		for (int i = 0; i < v_distances.size() - 1; i++)
		{
			sf::ConvexShape shape;
			shape.setPointCount(4);

			double heightLeftTop = (1 - 1 / (v_distances[i].d_distance * cos(direction - d_direction))) * window.getSize().y / 2;
			double heightLeftBottom = (1 + 1 / (v_distances[i].d_distance * cos(direction - d_direction))) * window.getSize().y / 2;
			double heightRightTop = (1 - 1 / (v_distances[i + 1].d_distance * cos(direction - d_direction))) * window.getSize().y / 2;
			double heightRightBottom = (1 + 1 / (v_distances[i + 1].d_distance * cos(direction - d_direction))) * window.getSize().y / 2;

			shape.setPoint(0, sf::Vector2f(0, heightLeftTop));
			shape.setPoint(1, sf::Vector2f(0, heightLeftBottom));
			shape.setPoint(2, sf::Vector2f((double)window.getSize().x / distancesSegments, heightRightBottom));
			shape.setPoint(3, sf::Vector2f((double)window.getSize().x / distancesSegments, heightRightTop));

			shape.setFillColor({ 255, 174, 174 });
			shape.setOutlineThickness(0);
			shape.setPosition((double)i * window.getSize().x / distancesSegments, 0);

			window.draw(shape);

			direction += d_fieldOfView / distancesSegments;
		}
	}
}

void Player::setWalkSpeed(double _walkSpeed)
{
	d_walkSpeed = _walkSpeed;
}

void Player::setBTextures(bool _b_textures)
{
	b_textures = _b_textures;
}