#include "Tile.h"
#include <iostream>

Tile::Tile()
{
	sf::Sprite temp(TextureManager::getTexture("mazePath"));
	path = temp;
	temp.setTexture(TextureManager::getTexture("mazeCrossed"));
	crossed = temp;
	temp.setTexture(TextureManager::getTexture("mazeWall"));
	wall = temp;
	temp.setTexture(TextureManager::getTexture("finalPath"));
	finalPath = temp;
	isPath = false;
	isCrossed = false;
	isFinalPath = false;
}

void Tile::Draw(sf::RenderWindow& window)
{
	window.draw(wall);
	if (isPath == true)
	{
		window.draw(path);
		if (isCrossed == true)
			window.draw(crossed);
		if (isFinalPath == true)
			window.draw(finalPath);
	}
}

void Tile::makePath(bool yes)
{
	isPath = yes;
}

void Tile::makeCrossed(bool yes)
{
	isCrossed = yes;
}

void Tile::makeFinalPath(bool yes)
{
	isFinalPath = yes;
}

bool Tile::isCrossedTrue()
{
	return isCrossed;
}

void Tile::setSpritePositions()
{
	path.setPosition(xposition, yposition);
	crossed.setPosition(xposition, yposition);
	wall.setPosition(xposition, yposition);
	finalPath.setPosition(xposition, yposition);
}

void Tile::setPosition(int x, int y)
{
	xposition = x;
	yposition = y;
	position = sf::Vector2i(x, y);
	setSpritePositions();
}
