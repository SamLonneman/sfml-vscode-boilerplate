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
	sf::Sprite AStarButton;
	sf::Sprite returnImage;
	sf::Text text;
	vector<vector<Tile>> tiles;
	vector<unordered_set<int>> adjList;
	vector<unordered_set<int>> bigAdjList;
	int bigH;
	int bigW;

public:
	Board();
	void Draw(sf::RenderWindow& window);
	void reDraw(sf::RenderWindow& window, int x, int y);
	void resetGame();
	void leftClick(sf::Vector2i mousePos, sf::RenderWindow& window);
	static vector<unordered_set<int>> generateGraph(int H, int W);
	vector<vector<bool>> graphToMaze(int h, int w);
	void setBoard();
	void addTile(Tile& tile, int x, int y);
	void runBFS(sf::RenderWindow& window, int src, int end);
	void runDFS(sf::RenderWindow& window, int src, int end);
	void runDijkstra(sf::RenderWindow& window, int src, int end);
	void runAStar(sf::RenderWindow& window, int src, int end);
	void displayData(sf::RenderWindow& window);
	unsigned int F(vector<int>& d, int i);
	void setBigBoard(int h, int w);
	vector<float> runBigBFS();
	vector<float> runBigDFS();
	vector<float> runBigDijkstra();
	vector<float> runBigAStar();
	unsigned int bigF(vector<int>& d, int i);
};