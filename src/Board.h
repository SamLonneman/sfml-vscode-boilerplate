#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include "Tile.h"
#include <unordered_set>
using namespace std;


class Board
{
	sf::Sprite timeButton;
	sf::Sprite BFSbutton;
	sf::Sprite DFSbutton;
	sf::Sprite DjikstraButton;
	sf::Sprite resetButton;
	sf::Sprite BellmanFordButton;
	vector<vector<Tile>> tiles;
	sf::Text text;


public:
	Board();
	void Draw(sf::RenderWindow& window);
	void leftClick(sf::Vector2i mousePos, sf::RenderWindow& window, vector<unordered_set<int>>& adjList, int src, int end);
	void resetGame();
	void addTile(Tile& tile, int x, int y);
	void setMaze(vector<vector<bool>> matrix);
	void runBFS(sf::RenderWindow& window, vector<unordered_set<int>>& adjList, int src, int end);
	void runDFS(sf::RenderWindow& window, vector<unordered_set<int>>& adjList, int src, int end);
	void runDjikstra(sf::RenderWindow& window, vector<unordered_set<int>>& adjList, int src, int end);
	void runBellmanFord(sf::RenderWindow& window, vector<unordered_set<int>>& adjList, int src, int end);
	void reDraw(sf::RenderWindow& window, int x, int y);
};
