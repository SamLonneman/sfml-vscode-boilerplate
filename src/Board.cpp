#include "Board.h"
#include "TextureManager.h"
#include <chrono>
#include <queue>
#include <sstream>
#include <stack>
#include <limits.h>
using namespace std;

Board::Board()
{
	BFSbutton.setTexture(TextureManager::getTexture("BFS"));
	BFSbutton.setPosition(0, 601);
	DFSbutton.setTexture(TextureManager::getTexture("DFS"));
	DFSbutton.setPosition(217, 601);
	DijkstraButton.setTexture(TextureManager::getTexture("Djikstra"));
	DijkstraButton.setPosition(651, 601);
	resetButton.setTexture(TextureManager::getTexture("reset"));
	resetButton.setPosition(921, 0);
	newMazeButton.setTexture(TextureManager::getTexture("newMaze"));
	newMazeButton.setPosition(0, 0);
	AStarButton.setTexture(TextureManager::getTexture("AStar"));
	AStarButton.setPosition(868, 601);
	timeButton.setTexture(TextureManager::getTexture("timingComparison"));
	timeButton.setPosition(434, 601);
	tiles = vector<vector<Tile>>(51, vector<Tile>(71));
	setBoard();
}

void Board::Draw(sf::RenderWindow& window)
{
	window.draw(BFSbutton);
	window.draw(DijkstraButton);
	window.draw(resetButton);
	window.draw(newMazeButton);
	window.draw(timeButton);
	window.draw(DFSbutton);
	window.draw(AStarButton);
	for (unsigned int i = 0; i < tiles.size(); i++)
	{
		for (unsigned int j = 0; j < tiles[i].size(); j++)
			tiles[i][j].Draw(window);
	}
}

void Board::reDraw(sf::RenderWindow& window, int x, int y)
{
	tiles[y][x].Draw(window);
	tiles[y + 1][x].Draw(window);
	tiles[y - 1][x].Draw(window);
	tiles[y][x + 1].Draw(window);
	tiles[y][x - 1].Draw(window);
}

void Board::resetGame()
{
	for (int i = 0; i < 51; i++)
	{
		for (int j = 0; j < 71; j++)
		{
			tiles[i][j].makeCrossed(false);
			tiles[i][j].makeFinalPath(false);
		}
	}
}

void Board::leftClick(sf::Vector2i mousePos, sf::RenderWindow& window)
{
	int src = 0;
	int end = 874;

	auto BFSbuttonBounds = BFSbutton.getGlobalBounds();
	auto DFSbuttonBounds = DFSbutton.getGlobalBounds();
	auto DijkstraButtonBounds = DijkstraButton.getGlobalBounds();
	auto AStarButtonBounds = AStarButton.getGlobalBounds();
	auto resetButtonBounds = resetButton.getGlobalBounds();
	auto newMazeButtonBounds = newMazeButton.getGlobalBounds();
	auto timeButtonBounds = timeButton.getGlobalBounds();

	if (resetButtonBounds.contains(mousePos.x, mousePos.y))
	{
		resetGame();
		Draw(window);
		window.display();
	}
	if (newMazeButtonBounds.contains(mousePos.x, mousePos.y))
	{
		setBoard();
		Draw(window);
		window.display();
	}
	if (BFSbuttonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the BFS visualization
		runBFS(window, src, end);
	}
	if (DFSbuttonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the DFS visualization
		runDFS(window, src, end);
	}
	if (DijkstraButtonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the Dijkstra visualization
		runDijkstra(window, src, end);
	}
	if (AStarButtonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the Bellman-Ford visualization
		runAStar(window, src, end);
	}
	if (timeButtonBounds.contains(mousePos.x, mousePos.y))
    {
        //Start the big boy timing and show results screen
        int size = gatherSize(window);
        displayData(window, size);
    }
}

// Generates a random single-solution maze adjacency list using a version of DFS, then removes walls to introduce more solutions
vector<unordered_set<int>> Board::generateGraph(int H, int W)
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
vector<vector<bool>> Board::graphToMaze(int H, int W)
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

void Board::setBoard()
{
	Tile tile;
	for (unsigned int i = 0; i < 51; i++)
	{
		for (unsigned int j = 0; j < 71; j++)
		{
			tile.setPosition(j * 10 + 185, i * 10 + 50);
			addTile(tile, j, i);
		}
	}

	const int H = 25, W = 35;
	adjList = generateGraph(H, W);
	auto matrix = graphToMaze(H, W);
	// We need to iterate through every node and change what is is, wall v path v crossed
	for (unsigned int i = 0; i < matrix.size(); i++)
	{
		for (unsigned int j = 0; j < matrix[i].size(); j++)
		{
			if (matrix[i][j])
				tiles[i][j].makePath(true);
		}
	}
}

void Board::addTile(Tile& tile, int y, int x)
{
	tiles[x][y] = tile;
}

void Board::runBFS(sf::RenderWindow& window, int src, int end)
{
	resetGame();
	unordered_set<int> visited;
	queue<int> queue;
	vector<int> parents(end + 1, -1);
	visited.emplace(src);
	queue.push(src);
	vector<int> path;

	while (!queue.empty())
	{
		//Code for visualizing the popped tile
		int front = queue.front();
		queue.pop();
		int yVal = front / 35 * 2 + 1;
		int xVal = front % 35 * 2 + 1;

		// Edge includer
		if (front == 0)
			tiles[1][0].makeCrossed(true);
		if (front == 874)
			tiles[49][70].makeCrossed(true);
		vector<int> neighbors;
		for (auto neighbor : adjList[front])
			neighbors.push_back(neighbor);
		random_shuffle(neighbors.begin(), neighbors.end());
		for (int n : neighbors)
		{
			int yValN = n / 35 * 2 + 1;
			int xValN = n % 35 * 2 + 1;
			if (tiles[yValN][xValN].isCrossedTrue())
			{
				tiles[(yVal + yValN) / 2][(xVal + xValN) / 2].makeCrossed(true);
			}
			if (visited.count(n) == 0)
			{
				visited.insert(n);
				queue.push(n);
				parents[n] = front;
			}
		}

		tiles[yVal][xVal].makeCrossed(true);
		Draw(window);
		window.display();

		if (front == end)
		{
			break;
		}
	}
	//HIGHLIGHTING SELECTED PATH IN DARK GREEN
	int currNode = end;
	path.push_back(currNode);
	while (currNode != -1)
	{
		currNode = parents[currNode];
		path.push_back(currNode);
	}
	path.pop_back();
	tiles[49][70].makeFinalPath(true);
	tiles[49][70].makeCrossed(false);
	for (int i : path)
	{
		int yVal = i / 35 * 2 + 1;
		int xVal = i % 35 * 2 + 1;
		tiles[yVal][xVal].makeFinalPath(true);
		tiles[yVal][xVal].makeCrossed(false);
		int yValP = parents[i] / 35 * 2 + 1;
		int xValP = parents[i] % 35 * 2 + 1;
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeFinalPath(true);
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeCrossed(false);
		Draw(window);
		window.display();
	}
	cout << "Distance for BFS Path: " << path.size() << endl;
}

void Board::runDFS(sf::RenderWindow& window, int src, int end)
{
	resetGame();
	unordered_set<int> visited;
	stack<int> stack;
	vector<int> parents(end + 1, -1);
	visited.emplace(src);
	stack.push(src);
	vector<int> path;

	while (!stack.empty())
	{
		//Code for visualizing the popped tile
		int front = stack.top();
		stack.pop();
		int yVal = front / 35 * 2 + 1;
		int xVal = front % 35 * 2 + 1;

		// Edge includer
		if (front == 0)
			tiles[1][0].makeCrossed(true);
		if (front == 874)
			tiles[49][70].makeCrossed(true);
		vector<int> neighbors;
		for (auto neighbor : adjList[front])
			neighbors.push_back(neighbor);
		random_shuffle(neighbors.begin(), neighbors.end());
		for (int n : neighbors)
		{
			int yValN = n / 35 * 2 + 1;
			int xValN = n % 35 * 2 + 1;
			if (tiles[yValN][xValN].isCrossedTrue())
			{
				tiles[(yVal + yValN) / 2][(xVal + xValN) / 2].makeCrossed(true);
			}
			if (visited.count(n) == 0)
			{
				visited.insert(n);
				stack.push(n);
				parents[n] = front;
			}
		}

		tiles[yVal][xVal].makeCrossed(true);
		Draw(window);
		window.display();

		if (front == end)
		{
			break;
		}
	}
	//HIGHLIGHTING SELECTED PATH IN DARK GREEN
	int currNode = end;
	path.push_back(currNode);
	while (currNode != -1)
	{
		currNode = parents[currNode];
		path.push_back(currNode);
	}
	path.pop_back();
	tiles[49][70].makeFinalPath(true);
	tiles[49][70].makeCrossed(false);
	for (int i : path)
	{
		int yVal = i / 35 * 2 + 1;
		int xVal = i % 35 * 2 + 1;
		tiles[yVal][xVal].makeFinalPath(true);
		tiles[yVal][xVal].makeCrossed(false);
		int yValP = parents[i] / 35 * 2 + 1;
		int xValP = parents[i] % 35 * 2 + 1;
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeFinalPath(true);
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeCrossed(false);
		Draw(window);
		window.display();
	}
	cout << "Distance for DFS Path: " << path.size() << endl;
}

void Board::runDijkstra(sf::RenderWindow& window, int src, int end)
{
	resetGame();
	vector<int> path;
	unordered_set<int> computed;
	computed.emplace(src);
	vector<int> d(adjList.size(), INT_MAX);
	d[src] = 0;
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> fringe;
	fringe.emplace(make_pair(d[src], src));
	vector<int> p(adjList.size(), -1);

	tiles[1][0].makeCrossed(true);
	tiles[1][1].makeCrossed(true);
	Draw(window);
	window.display();

	while (computed.size() < 875)
	{
		//need the highest priority index that hasnt been visited yet
		int minIndex = fringe.top().second;
		fringe.pop();
		computed.emplace(minIndex);

		// Edge includer
		int yVal = minIndex / 35 * 2 + 1;
		int xVal = minIndex % 35 * 2 + 1;
		if (minIndex == 874)
			tiles[49][70].makeCrossed(true);
		for (int n : adjList[minIndex])
		{
			int yValN = n / 35 * 2 + 1;
			int xValN = n % 35 * 2 + 1;
			if (tiles[yValN][xValN].isCrossedTrue())
				tiles[(yVal + yValN) / 2][(xVal + xValN) / 2].makeCrossed(true);
		}

		//Trying to highlight what stuff has been done already
		tiles[yVal][xVal].makeCrossed(true);
		Draw(window);
		window.display();

		// Relax all uncomputed neighbors
		for (auto i : adjList[minIndex])
		{
			if (!computed.count(i))
			{
				if (d[minIndex] + 1 < d[i])
				{
					d[i] = d[minIndex] + 1;
					p[i] = minIndex;
					fringe.emplace(make_pair(d[i], i));
				}
			}
		}

		// Stop if reached end
		if (minIndex == end)
		{
			break;
		}
	}

	//HIGHLIGHTING SELECTED PATH IN DARK GREEN
	int currNode = end;
	path.push_back(currNode);
	while (currNode != 0)
	{
		currNode = p[currNode];
		path.push_back(currNode);
	}
	tiles[49][70].makeFinalPath(true);
	tiles[49][70].makeCrossed(false);
	for (int i : path)
	{
		int yVal = i / 35 * 2 + 1;
		int xVal = i % 35 * 2 + 1;
		tiles[yVal][xVal].makeFinalPath(true);
		tiles[yVal][xVal].makeCrossed(false);
		int yValP = p[i] / 35 * 2 + 1;
		int xValP = p[i] % 35 * 2 + 1;
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeFinalPath(true);
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeCrossed(false);
		Draw(window);
		window.display();
	}
	cout << "Distance for Dijkstra's Path: " << path.size() << endl;
}

void Board::runAStar(sf::RenderWindow& window, int src, int end)
{
	resetGame();
	vector<int> path;
	unordered_set<int> computed;
	computed.emplace(src);
	vector<int> d(adjList.size(), INT_MAX);
	d[src] = 0;
	priority_queue<pair<unsigned int, int>, vector<pair<unsigned int, int>>, greater<pair<unsigned int, int>>> fringe;
	fringe.emplace(make_pair(F(d, src), src));
	vector<int> p(adjList.size(), -1);

	tiles[1][0].makeCrossed(true);
	tiles[1][1].makeCrossed(true);
	Draw(window);
	window.display();

	while (computed.size() < 875)
	{
		//need the highest priority index that hasnt been visited yet
		int minIndex = fringe.top().second;
		fringe.pop();
		computed.emplace(minIndex);

		// Edge includer
		int yVal = minIndex / 35 * 2 + 1;
		int xVal = minIndex % 35 * 2 + 1;
		if (minIndex == 874)
			tiles[49][70].makeCrossed(true);
		for (int n : adjList[minIndex])
		{
			int yValN = n / 35 * 2 + 1;
			int xValN = n % 35 * 2 + 1;
			if (tiles[yValN][xValN].isCrossedTrue())
				tiles[(yVal + yValN) / 2][(xVal + xValN) / 2].makeCrossed(true);
		}

		//Trying to highlight what stuff has been done already
		tiles[yVal][xVal].makeCrossed(true);
		Draw(window);
		window.display();

		// Relax all uncomputed neighbors
		for (auto i : adjList[minIndex])
		{
			if (!computed.count(i))
			{
				if (d[minIndex] + 1 < d[i])
				{
					d[i] = d[minIndex] + 1;
					p[i] = minIndex;
					fringe.emplace(make_pair(F(d, i), i));
				}
			}
		}

		// Stop if reached end
		if (minIndex == end)
			break;
	}

	//HIGHLIGHTING SELECTED PATH IN DARK GREEN
	int currNode = end;
	path.push_back(currNode);
	while (currNode != 0)
	{
		currNode = p[currNode];
		path.push_back(currNode);
	}
	tiles[49][70].makeFinalPath(true);
	tiles[49][70].makeCrossed(false);
	for (int i : path)
	{
		int yVal = i / 35 * 2 + 1;
		int xVal = i % 35 * 2 + 1;
		tiles[yVal][xVal].makeFinalPath(true);
		tiles[yVal][xVal].makeCrossed(false);
		int yValP = p[i] / 35 * 2 + 1;
		int xValP = p[i] % 35 * 2 + 1;
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeFinalPath(true);
		tiles[(yVal + yValP) / 2][(xVal + xValP) / 2].makeCrossed(false);
		Draw(window);
		window.display();
	}
	cout << "Distance for A* Path: " << path.size() << endl;
}

// Heuristic Function: Returns path distance from start + Manhattan distance to end
unsigned int Board::F(vector<int>& d, int i)
{
	return d[i] + (25 - i / 35 - 1) + (35 - i % 35 - 1);
}

void Board::setBigBoard(int h, int w)
{
	bigAdjList = generateGraph(h, w);
	bigH = h;
	bigW = w;
}

vector<float> Board::runBigBFS()
{
	unordered_set<int> visited;
	queue<int> queue;
	vector<int> parents(bigH * bigW, -1);
	visited.emplace(0);
	queue.push(0);

	auto startTime = chrono::high_resolution_clock::now();
	while (!queue.empty())
	{
		int front = queue.front();
		queue.pop();

		vector<int> neighbors;
		for (int neighbor : bigAdjList[front])
			neighbors.push_back(neighbor);
		random_shuffle(neighbors.begin(), neighbors.end());
		for (int n : neighbors)
		{
			if (!visited.count(n))
			{
				visited.insert(n);
				queue.push(n);
				parents[n] = front;
			}
		}

		if (front == bigH * bigW - 1)
			break;
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - startTime;

	int currNode = bigH * bigW - 1;
	int pathLength = 0;
	while (currNode != -1)
	{
		currNode = parents[currNode];
		pathLength++;
	}
	return { duration.count(), (float)visited.size(), (float)pathLength };
}

vector<float> Board::runBigDFS()
{
	unordered_set<int> visited;
	stack<int> stack;
	vector<int> parents(bigH * bigW, -1);
	visited.emplace(0);
	stack.push(0);

	auto startTime = chrono::high_resolution_clock::now();
	while (!stack.empty())
	{
		int top = stack.top();
		stack.pop();

		vector<int> neighbors;
		for (int neighbor : bigAdjList[top])
			neighbors.push_back(neighbor);
		random_shuffle(neighbors.begin(), neighbors.end());
		for (int n : neighbors)
		{
			if (!visited.count(n))
			{
				visited.insert(n);
				stack.push(n);
				parents[n] = top;
			}
		}

		if (top == bigH * bigW - 1)
			break;
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - startTime;

	int currNode = bigH * bigW - 1;
	int pathLength = 0;
	while (currNode != -1)
	{
		currNode = parents[currNode];
		pathLength++;
	}
	return { duration.count(), (float)visited.size(), (float)pathLength };
}

vector<float> Board::runBigDijkstra()
{
	unordered_set<int> computed;
	computed.emplace(0);
	vector<int> d(bigAdjList.size(), INT_MAX);
	d[0] = 0;
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> fringe;
	fringe.emplace(make_pair(d[0], 0));
	vector<int> p(bigAdjList.size(), -1);

	auto startTime = chrono::high_resolution_clock::now();
	while (computed.size() < (unsigned int)(bigH * bigW))
	{
		// W need the highest priority index that hasnt been visited yet
		int minIndex = fringe.top().second;
		fringe.pop();
		computed.emplace(minIndex);

		// Relax all uncomputed neighbors
		for (auto i : bigAdjList[minIndex])
		{
			if (!computed.count(i))
			{
				if (d[minIndex] + 1 < d[i])
				{
					d[i] = d[minIndex] + 1;
					p[i] = minIndex;
					fringe.emplace(make_pair(d[i], i));
				}
			}
		}

		// Stop if reached end
		if (minIndex == bigH * bigW - 1)
			break;
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - startTime;

	int currNode = bigH * bigW - 1;
	int pathLength = 0;
	while (currNode != -1)
	{
		currNode = p[currNode];
		pathLength++;
	}
	return { duration.count(), (float)computed.size(), (float)pathLength };
}

vector<float> Board::runBigAStar()
{
	unordered_set<int> computed;
	computed.emplace(0);
	vector<int> d(bigAdjList.size(), INT_MAX);
	d[0] = 0;
	priority_queue<pair<unsigned int, int>, vector<pair<unsigned int, int>>, greater<pair<unsigned int, int>>> fringe;
	fringe.emplace(make_pair(bigF(d, 0), 0));
	vector<int> p(bigAdjList.size(), -1);

	auto startTime = chrono::high_resolution_clock::now();
	while (computed.size() < (unsigned int)(bigH * bigW))
	{
		// W need the highest priority index that hasnt been visited yet
		int minIndex = fringe.top().second;
		fringe.pop();
		computed.emplace(minIndex);

		// Relax all uncomputed neighbors
		for (auto i : bigAdjList[minIndex])
		{
			if (!computed.count(i))
			{
				if (d[minIndex] + 1 < d[i])
				{
					d[i] = d[minIndex] + 1;
					p[i] = minIndex;
					fringe.emplace(make_pair(bigF(d, i), i));
				}
			}
		}

		// Stop if reached end
		if (minIndex == bigH * bigW - 1)
			break;
	}
	auto stopTime = chrono::high_resolution_clock::now();
	chrono::duration<float> duration = stopTime - startTime;

	int currNode = bigH * bigW - 1;
	int pathLength = 0;
	while (currNode != -1)
	{
		currNode = p[currNode];
		pathLength++;
	}
	return { duration.count(), (float)computed.size(), (float)pathLength };
}

// Heuristic Function: Returns path distance from start + Manhattan distance to end
unsigned int Board::bigF(vector<int>& d, int i)
{
	return d[i] + (bigH - i / bigW - 1) + (bigW - i % bigW - 1);
}

void Board::displayData(sf::RenderWindow& window, int size)
{

    sf::RectangleShape screen(sf::Vector2f(window.getSize().x, window.getSize().y));
    screen.setFillColor(sf::Color(0, 0, 0));
    screen.setPosition(0, 0);
    returnImage.setTexture(TextureManager::getTexture("return"));
    returnImage.setPosition(window.getSize().x - 234, window.getSize().y - 240);

    // Displayes instructions to hover over return button to exit
    sf::Text text;
    sf::Font font;
    font.loadFromFile("src/text/mono.ttf");
    text.setFont(font);
    text.setCharacterSize(50);
    text.setPosition(405, 30);
    text.setFillColor(sf::Color::White);
    std::stringstream stream;
    stream << "Maze Runners";
    text.setString(stream.str());

    // Displays instructions to hover over return button to exit
    sf::Text information;
    information.setFont(font);
    information.setCharacterSize(35);
    information.setPosition(20, 100);
    information.setFillColor(sf::Color::White);
    std::stringstream stream1;
    int vertices = size * size;
    stream1 << "Timing comparison for randomly generated maze of " << vertices << " vertices";
    information.setString(stream1.str());

    window.draw(screen);
    window.draw(returnImage);
    window.draw(text);
    window.draw(information);
    window.display();
    setBigBoard(size, size);

    // Displays DFS information
    vector<float> data = runBigDFS();
    sf::Text DFS;
    DFS.setFont(font);
    DFS.setCharacterSize(23);
    DFS.setPosition(20, 210);
    DFS.setFillColor(sf::Color::White);
    std::stringstream DFSstream;
    DFSstream << "DFS took " << data[0] << " seconds, visited " << data[1] << " vertices, and found a valid path of " << data[2] << " vertices";
    DFS.setString(DFSstream.str());

    window.draw(screen);
    window.draw(returnImage);
    window.draw(text);
    window.draw(information);
    window.draw(DFS);
    window.display();

    // Displays BFS information
    data = runBigBFS();
    sf::Text BFS;
    BFS.setFont(font);
    BFS.setCharacterSize(23);
    BFS.setPosition(20, 270);
    BFS.setFillColor(sf::Color::White);
    std::stringstream BFSstream;
    BFSstream << "BFS took " << data[0] << " seconds, visited " << data[1] << " vertices, and found shortest path of " << data[2] << " vertices";
    BFS.setString(BFSstream.str());

    window.draw(screen);
    window.draw(returnImage);
    window.draw(text);
    window.draw(information);
    window.draw(DFS);
    window.draw(BFS);
    window.display();

    // Displays Dijkstras information
    data = runBigDijkstra();
    sf::Text DIJ;
    DIJ.setFont(font);
    DIJ.setCharacterSize(23);
    DIJ.setPosition(20, 330);
    DIJ.setFillColor(sf::Color::White);
    std::stringstream DIJstream;
    DIJstream << "Dijkstra's took " << data[0] << " seconds, visited " << data[1] << " vertices, and found shortest path of " << data[2] << " vertices";
    DIJ.setString(DIJstream.str());

    window.draw(screen);
    window.draw(returnImage);
    window.draw(text);
    window.draw(information);
    window.draw(DFS);
    window.draw(BFS);
    window.draw(DIJ);
    window.display();

    // Displays A* information
    data = runBigAStar();
    sf::Text A;
    A.setFont(font);
    A.setCharacterSize(23);
    A.setPosition(20, 390);
    A.setFillColor(sf::Color::White);
    std::stringstream Astream;
    Astream << "A* took " << data[0] << " seconds, visited " << data[1] << " vertices, and found shortest path of " << data[2] << " vertices";
    A.setString(Astream.str());

    window.draw(screen);
    window.draw(returnImage);
    window.draw(text);
    window.draw(information);
    window.draw(DFS);
    window.draw(BFS);
    window.draw(DIJ);
    window.draw(A);
    window.display();

    while (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
        continue;
}

int Board::gatherSize(sf::RenderWindow& window) {
    sf::RectangleShape screen1(sf::Vector2f(window.getSize().x, window.getSize().y));
    screen1.setFillColor(sf::Color(0, 0, 0));
    screen1.setPosition(0, 0);

    sf::Text instruction;
    sf::Font font;
    instruction.setFont(font);
    font.loadFromFile("src/text/mono.ttf");
    instruction.setCharacterSize(30);
    instruction.setPosition(175, 50);
    instruction.setFillColor(sf::Color::White);
    std::stringstream instructionstream;
    instructionstream << "Select the side length for a square maze in vertices";
    instruction.setString(instructionstream.str());

    sf::Text instruction2;
    instruction2.setFont(font);
    instruction2.setCharacterSize(30);
    instruction2.setPosition(325, 100);
    instruction2.setFillColor(sf::Color::White);
    std::stringstream instructionstream2;
    instructionstream2 << "Use up and down arrows to adjust";
    instruction2.setString(instructionstream2.str());

    sf::Text instruction3;
    instruction3.setFont(font);
    instruction3.setCharacterSize(30);
    instruction3.setPosition(375, 150);
    instruction3.setFillColor(sf::Color::White);
    std::stringstream instructionstream3;
    instructionstream3 << "Press \"Enter\" to submit";
    instruction3.setString(instructionstream3.str());

    sf::Text size1;
    size1.setFont(font);
    size1.setCharacterSize(30);
    size1.setPosition(window.getSize().x / 2 - 120, window.getSize().y / 2 + 30);
    size1.setFillColor(sf::Color::White);
	sf::Text size2;
    size2.setFont(font);
    size2.setCharacterSize(30);
    size2.setPosition(window.getSize().x / 2 - 220, window.getSize().y / 2 + 100);
    size2.setFillColor(sf::Color::White);

    int size = 320;
    while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            size++;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && size > 2)
            size--;
        std::stringstream siz1;
        siz1 << size;
        size1.setString("Side length: " + siz1.str());
		std::stringstream siz2;
        siz2 << (size * size);
        size2.setString("Total number of vertices: " + siz2.str());

		window.draw(screen1);
		window.draw(instruction);
		window.draw(instruction2);
		window.draw(instruction3);
        window.draw(size1);
		window.draw(size2);
        window.display();
		this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return size;
}