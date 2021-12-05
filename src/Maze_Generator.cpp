#include "Board.h"
#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <queue>
#include <stack>
#include <unordered_set>
#include <vector>
using namespace std;

// Helper function prototypes
vector<unordered_set<int>> generateGraph(int H, int W);
vector<vector<bool>> graphToMaze(vector<unordered_set<int>>& adjList, int H, int W);
void printMaze(vector<vector<bool>>& matrix);
void dijkstra(vector<unordered_set<int>>& adjList, int src, int end);
void bellmanFord(vector<unordered_set<int>>& adjList, int src, int end);
void DFS(vector<unordered_set<int>>& adjList, int src, int end);
void printList(vector<unordered_set<int>>& adjList);
void BFS(vector<unordered_set<int>>& adjList, int src, int end);
void setBoard(Board& board);
// Program Driver
int main()
{
	// 2500 x 40 gives 100k vertices and fits in console
	// const int Y = 2500, X = 40;
	//auto bigAdjList = generateGraph(Y, X);
	//auto bigMatrix = graphToMaze(bigAdjList, Y, X);
	//printMaze(bigMatrix);

	// Seed the randomizer
	srand(time(0));

	//SFML START
	const int H = 25, W = 35;
	auto adjList = generateGraph(H, W);
	auto matrix = graphToMaze(adjList, H, W);
	Board board;
	setBoard(board);
	board.setMaze(matrix);
	sf::RenderWindow window(sf::VideoMode(1080, 720), "Maze Runner");
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				// if (event.mouseButton.button == sf::Mouse::Right)
				// {
				// 	board.rightClick(sf::Mouse::getPosition(window));
				// }
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					board.leftClick(sf::Mouse::getPosition(window), window, adjList, 0, 874);
				}
			}
		}
		window.clear(sf::Color(173, 216, 230, 255));
		board.Draw(window);
		window.display();
	}
	TextureManager::Clear();
	return 0;
}

void setBoard(Board& board)
{
	Tile tile;
	for (unsigned int i = 0; i < 51; i++)
	{
		for (unsigned int j = 0; j < 71; j++)
		{
			tile.setPosition(j * 10 + 185, i * 10 + 50);
			board.addTile(tile, j, i);
		}
	}
}

// Generates a random single-solution maze adjacency list using a version of DFS, then removes walls to introduce more solutions
vector<unordered_set<int>> generateGraph(int H, int W)
{
	// Graph adjacency list
	vector<unordered_set<int>> adjList(H * W);
	// Set of visited vertices
	vector<bool> v(H * W);
	// Stack of vertices to visit next...
	// NOTE: the pair consists of first: the vertex to be visited and second: which vertex it is visited from
	stack<pair<int, int>> s;
	//Push first vertex into stack with unused "from" value
	s.emplace(0, -1);

	// Visits each vertex exactly once
	while (!s.empty())
	{

		// Get node to be visited and the node it is visited from
		int u = s.top().first;
		int from = s.top().second;
		s.pop();
		// If it has already been visited, move on
		if (v[u])
			continue;
		// Otherwise, mark it as visited
		v[u] = true;
		// Excluding the first node, add the edge to the adjacency list
		if (from != -1)
		{
			adjList[from].insert(u);
			adjList[u].insert(from);
		}

		// Create vector of unvisited neighbors
		vector<int> neighbors;
		if (u / W > 0 && !v[u - W])
			neighbors.push_back(u - W);
		if (u % W < W - 1 && !v[u + 1])
			neighbors.push_back(u + 1);
		if (u / W < H - 1 && !v[u + W])
			neighbors.push_back(u + W);
		if (u % W > 0 && !v[u - 1])
			neighbors.push_back(u - 1);

		// Push neighbors onto the stack in random order
		if (!neighbors.empty())
		{
			random_shuffle(neighbors.begin(), neighbors.end());
			for (auto neighbor : neighbors)
				s.emplace(neighbor, u);
		}
	}

	// Remove some walls to create more solutions
	for (int i = 0; i < max(H, W); i++)
	{
		// Choose a random vertex
		int u = rand() % (W * H);
		// Create vector of *potential* neighbors of u
		vector<int> neighbors;
		if (u / W > 0 && !adjList[u].count(u - W))
			neighbors.push_back(u - W);
		if (u % W < W - 1 && !adjList[u].count(u + 1))
			neighbors.push_back(u + 1);
		if (u / W < H - 1 && !adjList[u].count(u + W))
			neighbors.push_back(u + W);
		if (u % W > 0 && !adjList[u].count(u - 1))
			neighbors.push_back(u - 1);
		// Knock down a random wall (make a connection with a random neighbor)
		if (!neighbors.empty())
		{
			int v = neighbors[rand() % neighbors.size()];
			adjList[u].insert(v);
			adjList[v].insert(u);
		}
	}

	// Finally, return the completed adjacency list for underlying graph
	return adjList;
}

// Converts a graph adjacency list into a bool matrix representing a maze
vector<vector<bool>> graphToMaze(vector<unordered_set<int>>& adjList, int H, int W)
{
	vector<vector<bool>> matrix(H * 2 + 1, vector<bool>(W * 2 + 1));
	for (unsigned int i = 0; i < adjList.size(); i++)
	{
		int r = i / W;
		int c = i % W;
		int row = r * 2 + 1;
		int col = c * 2 + 1;
		matrix[row][col] = true;
		if (c < W - 1 && adjList[i].count(i + 1))
			matrix[row][col + 1] = true;
		if (r < H - 1 && adjList[i].count(i + W))
			matrix[row + 1][col] = true;
	}
	matrix[1][0] = true;
	matrix[H * 2 - 1][W * 2] = true;
	return matrix;
}

void printList(vector<unordered_set<int>>& adjList)
{
	for (unsigned int i = 0; i < adjList.size(); i++)
	{
		if (!adjList[i].empty())
		{
			cout << i << " -> ";
			for (auto it = adjList[i].begin(); it != adjList[i].end(); ++it)
			{
				cout << *it << " ";
			}
			cout << endl;
		}
	}
}
//bellmanFord
void bellmanFord(vector<unordered_set<int>>& adjList, int src, int end)
{
	auto start = chrono::high_resolution_clock::now();
	vector<int> d(adjList.size(), 9999999);
	vector<int> p(adjList.size(), -1);
	d[src] = 0;
	p[src] = 0;
	for (unsigned int j = 0; j < adjList.size() - 1; j++)
	{
		for (unsigned int i = 0; i < adjList.size(); i++)
		{
			for (auto it = adjList[i].begin(); it != adjList[i].end(); ++it)
			{
				if (d[*it] > d[i] + 1)
				{
					d[*it] = d[i] + 1;
					p[*it] = i;
				}
			}
		}
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - start;
	cout << endl;
	cout << endl;
	cout << "Using Bellman-Ford" << endl;
	cout << "Finding " << end << " from " << src << " using Bellman-Ford took "
		 << duration.count() << " seconds" << endl;
	cout << "Verticies visited " << d[end] << endl;
	cout << "Best path from " << src << " to " << end << ": ";
	int x = 999;
	int y = end;
	vector<int> v;
	while (x != src)
	{
		x = p[y];
		v.push_back(x);
		y = x;
	}
	for (int i = v.size() - 1; i >= 0; i--)
	{
		cout << v[i] << " -> ";
	}
	cout << end << endl;
	cout << endl;
}

// does dijkstra stuff
void dijkstra(vector<unordered_set<int>>& adjList, int src, int end)
{
	auto start = chrono::high_resolution_clock::now();
	unordered_set<int> computed;
	unordered_set<int> notComputed;
	computed.emplace(src);
	//filling notComputed with all verticies except src
	for (unsigned int i = 0; i < adjList.size(); i++)
	{
		if (computed.count(i) != 1)
		{
			notComputed.emplace(i);
		}
	}
	// distance to each verticy
	vector<int> d(adjList.size(), 99999);
	d[src] = 0;
	// keeps track of predecessor
	vector<int> p(adjList.size(), -1);

	for (auto it = adjList[src].begin(); it != adjList[src].end(); ++it)
	{
		d[*it] = 1;
		p[*it] = src;
	}

	while (!notComputed.empty())
	{
		//need the smallest index that hasnt been visited yet
		vector<int> temp = d;
		for (auto it = computed.begin(); it != computed.end(); ++it)
		{
			temp[*it] = 999999999;
		}

		int minIndex = std::min_element(temp.begin(), temp.end()) - temp.begin();
		computed.emplace(minIndex);
		notComputed.erase(minIndex);

		for (auto it = adjList[minIndex].begin(); it != adjList[minIndex].end(); ++it)
		{
			if (notComputed.count(*it) != 0)
			{
				if (d[minIndex] + 1 < d[*it])
				{
					d[*it] = d[minIndex] + 1;
					p[*it] = minIndex;
				}
			}
		}
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - start;
	cout << endl;
	cout << "Using DIJKSTRA" << endl;
	cout << "Finding " << end << " from " << src << " using DIJSKTRA took "
		 << duration.count() << " Im not sure of time units" << endl;
	cout << "The distance from " << src << " to " << end << " is " << d[end] << endl;
	cout << "Best path from " << src << " to " << end << ": ";
	int x = 999;
	int y = end;
	vector<int> v;
	while (x != src)
	{
		x = p[y];
		v.push_back(x);
		y = x;
	}
	for (int i = v.size() - 1; i >= 0; i--)
	{
		cout << v[i] << " -> ";
	}
	cout << end << endl;
	cout << endl;
}

//DFS stuff
void DFS(vector<unordered_set<int>>& adjList, int src, int end)
{
	auto start = chrono::high_resolution_clock::now();
	unordered_set<int> visited;
	stack<int> stack;
	visited.emplace(src);
	stack.push(src);
	vector<int> path;

	while (!stack.empty())
	{
		int x = stack.top();
		if (x == end)
		{
			break;
		}
		//cout << x << " ";
		path.push_back(x);
		stack.pop();
		vector<int> neighbors;
		for (auto it = adjList[x].begin(); it != adjList[x].end(); ++it)
		{
			neighbors.push_back(*it);
		}
		for (int v : neighbors)
		{
			if (visited.count(v) == 0)
			{
				visited.insert(v);
				stack.push(v);
			}
		}
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - start;
	cout << "Using Depth First Search" << endl;
	cout << "Finding " << end << " from " << src << " using DFS took "
		 << duration.count() << " Im not sure time unit" << endl;
	cout << "The distance from " << src << " to " << end << " is " << path.size() << endl;
	cout << "Best path from " << src << " to " << end << ": ";
	for (unsigned int i = 0; i < path.size(); i++)
	{
		cout << path[i] << "-> ";
	}
	cout << end;
}

//BFS search
void BFS(vector<unordered_set<int>>& adjList, int src, int end)
{
	auto start = chrono::high_resolution_clock::now();
	unordered_set<int> visited;
	queue<int> queue;
	visited.emplace(src);
	queue.push(src);
	vector<int> path;

	while (!queue.empty())
	{
		int x = queue.front();
		if (x == end)
		{
			break;
		}
		path.push_back(x);
		queue.pop();
		vector<int> neighbors;
		for (auto it = adjList[x].begin(); it != adjList[x].end(); ++it)
		{
			neighbors.push_back(*it);
		}
		for (int v : neighbors)
		{
			if (visited.count(v) == 0)
			{
				visited.insert(v);
				queue.push(v);
			}
		}
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - start;
	cout << endl;
	cout << endl;
	cout << "Using Breadth First Search" << endl;
	cout << "Finding " << end << " from " << src << " using BFS took "
		 << duration.count() << " Im not sure time unit" << endl;
	cout << "The distance from " << src << " to " << end << " is " << path.size() << endl;
	cout << "Best path from " << src << " to " << end << ": ";
	for (unsigned int i = 0; i < path.size(); i++)
	{
		cout << path[i] << "-> ";
	}
	cout << end;
}