#pragma once
#include "Tile.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
using namespace std;

class Board
{
	sf::Sprite timeButton;
	sf::Sprite BFSbutton;
	sf::Sprite DFSbutton;
	sf::Sprite DijkstraButton;
	sf::Sprite resetButton;
	sf::Sprite newMazeButton;
	sf::Sprite BellmanFordButton;
	sf::Text text;
	vector<vector<Tile>> tiles;
	vector<unordered_set<int>> adjList;

public:
	Board();
	void Draw(sf::RenderWindow& window);
	void leftClick(sf::Vector2i mousePos, sf::RenderWindow& window);
	void resetGame();
	void addTile(Tile& tile, int x, int y);
	void setBoard();
	void runBFS(sf::RenderWindow& window, int src, int end);
	void runDFS(sf::RenderWindow& window, int src, int end);
	void runDijkstra(sf::RenderWindow& window, int src, int end);
	void runAStar(sf::RenderWindow& window, int src, int end);
	static int h(int v);
	static unsigned int f(vector<int> d, int v);
	void runBellmanFord(sf::RenderWindow& window, int src, int end);
	void reDraw(sf::RenderWindow& window, int x, int y);
	static vector<unordered_set<int>> generateGraph(int H, int W);
	vector<vector<bool>> graphToMaze(int H, int W);
};
