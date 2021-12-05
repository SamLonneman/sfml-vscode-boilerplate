#pragma once
#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include <string>

class Tile
{
	int xposition;
	int yposition;
	sf::Vector2i position;
	sf::Sprite crossed;
	sf::Sprite finalPath;
	bool isPath;
	bool isCrossed;
	bool isFinalPath;

public:
	sf::Sprite path;
	sf::Sprite wall;
	Tile();
	void Draw(sf::RenderWindow& window);
	void makePath(bool yes);
	void makeCrossed(bool yes);
	void makeFinalPath(bool yes);
	bool isCrossedTrue();
	void setSpritePositions();
	void setPosition(int x, int y);
};
