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
	Board board;
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
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					board.leftClick(sf::Mouse::getPosition(window), window);
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