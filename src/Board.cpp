#include "Board.h"
#include "TextureManager.h"
#include <chrono>
#include <queue>
#include <stack>
#include <thread>
using namespace std;

Board::Board()
{
	BFSbutton.setTexture(TextureManager::getTexture("BFS"));
	BFSbutton.setPosition(0, 601);
	DFSbutton.setTexture(TextureManager::getTexture("DFS"));
	DFSbutton.setPosition(217, 601);
	DjikstraButton.setTexture(TextureManager::getTexture("Djikstra"));
	DjikstraButton.setPosition(651, 601);
	resetButton.setTexture(TextureManager::getTexture("reset"));
	resetButton.setPosition(921, 0);
	newMazeButton.setTexture(TextureManager::getTexture("newMaze"));
	newMazeButton.setPosition(0, 0);
	BellmanFordButton.setTexture(TextureManager::getTexture("BellmanFord"));
	BellmanFordButton.setPosition(868, 601);
	timeButton.setTexture(TextureManager::getTexture("timingComparison"));
	timeButton.setPosition(434, 601);
	tiles = vector<vector<Tile>>(51, vector<Tile>(71));
	setBoard();
}

void Board::Draw(sf::RenderWindow& window)
{
	window.draw(BFSbutton);
	window.draw(DjikstraButton);
	window.draw(resetButton);
	window.draw(newMazeButton);
	window.draw(timeButton);
	window.draw(DFSbutton);
	window.draw(BellmanFordButton);
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

void Board::leftClick(sf::Vector2i mousePos, sf::RenderWindow& window)
{
	int src = 0;
	int end = 874;

	auto BFSbuttonBounds = BFSbutton.getGlobalBounds();
	auto DFSbuttonBounds = DFSbutton.getGlobalBounds();
	auto DjikstraButtonBounds = DjikstraButton.getGlobalBounds();
	auto BellmanFordButtonBounds = BellmanFordButton.getGlobalBounds();
	auto resetButtonBounds = resetButton.getGlobalBounds();
	auto newMazeButtonBounds = newMazeButton.getGlobalBounds();
	auto timeButtonBounds = timeButton.getGlobalBounds();

	if (resetButtonBounds.contains(mousePos.x, mousePos.y))
	{
		resetGame();
	}
	if (newMazeButtonBounds.contains(mousePos.x, mousePos.y))
	{
		setBoard();
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
	if (DjikstraButtonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the Djikstra visualization
		runDjikstra(window, src, end);
	}
	if (BellmanFordButtonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the Bellman-Ford visualization
		runBellmanFord(window, src, end);
	}
	if (timeButtonBounds.contains(mousePos.x, mousePos.y))
	{
		//Start the big boy timing and show results screen
	}
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
		//this_thread::sleep_for(std::chrono::milliseconds(1));

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
		//this_thread::sleep_for(std::chrono::milliseconds(1));

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

void Board::runDjikstra(sf::RenderWindow& window, int src, int end)
{
	resetGame();
	vector<int> path;
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
	// distance to each vertex
	vector<int> d(adjList.size(), 999999);
	d[src] = 0;
	// keeps track of predecessor
	vector<int> p(adjList.size(), -1);

	for (auto it = adjList[src].begin(); it != adjList[src].end(); ++it)
	{
		d[*it] = 1;
		p[*it] = src;
	}
	tiles[1][0].makeCrossed(true);
	tiles[1][1].makeCrossed(true);
	Draw(window);
	window.display();

	while (!notComputed.empty())
	{
		//need the smallest index that hasnt been visited yet
		vector<int> temp = d;
		for (auto it = computed.begin(); it != computed.end(); ++it)
		{
			temp[*it] = INT_MAX;
		}

		int minIndex = std::min_element(temp.begin(), temp.end()) - temp.begin();
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
	cout << "Distance for Djikstra Path: " << path.size() << endl;
}

void Board::runBellmanFord(sf::RenderWindow& window, int src, int end)
{
	resetGame();
	vector<int> path;
	vector<int> d(adjList.size(), 999999);
	vector<int> p(adjList.size(), -1);
	d[src] = 0;
	p[src] = 0;
	for (unsigned int j = 0; j < adjList.size() - 1; j++)
	{

		//Visualizing the nodes???? not sure micheal plz help
		//int yVal = j / 35 * 2 + 1;
		//int xVal = j % 35 * 2 + 1;
		//tiles[yVal][xVal].makeCrossed(true);
		//Draw(window);
		//window.display();

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
	tiles[1][0].makeFinalPath(true);
	tiles[1][0].makeCrossed(false);
	Draw(window);
	window.display();
	cout << "Distance for BellmanFord Path: " << path.size() << endl;
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