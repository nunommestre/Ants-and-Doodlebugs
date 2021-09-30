// Ants and Doodle Bugs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

const int X_SIZE = 20;
const int Y_SIZE = 20;
const float SQUARE_PIXELS = 20.0;
enum Tile_type { EMPTY, ANT, DOODLE_BUG, NUMBER_OF_TILE_TYPES };
enum Direction { UP, DOWN, LEFT, RIGHT, NUMBER_OF_DIRECTIONS };
class World;

class Index_out_of_bounds {};

class Tile
{
public:
	//Tile() : x(0), y(0), shape() { }
	Tile(int x = 0, int y = 0, float radius = SQUARE_PIXELS, size_t point_count = 30) : x(x), y(y), shape(radius, point_count) { }
	virtual void display(sf::RenderWindow& window) = 0;
	virtual Tile_type who() = 0; //this makes the class into an abstract class.
	virtual void turn(World& w) {};
	static void tile_swap(Tile*& pTile1, Tile*& pTile2);
protected:
	int x;
	int y;
	sf::CircleShape shape;
	void fix_shape_position();
};

class Organism : public Tile
{
public:
	Organism(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 30)
	{
		shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
	}
private:
	virtual void breeding(World& w, int& turn_number) = 0;
	virtual void move(World& w, int& turn_number) = 0;
};

class Ant : public Organism
{
public:
	Ant(int x = 0, int y = 0) : Organism(x, y)
	{
		shape.setFillColor(sf::Color::Green);
	}
	virtual void display(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual Tile_type who() { return ANT; }
	virtual void turn(World& w);
	void breeding(World& w, int& turn_number);
	void move(World& w, int& turn_number);
private:
	int breed = 0;
};

class Doodle_Bug : public Organism
{
public:
	Doodle_Bug(int x = 0, int y = 0) : Organism(x, y)
	{
		shape.setFillColor(sf::Color::Red);
	}
	virtual void display(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual void turn(World& w);
	void breeding(World& w, int& turn_number);
	void eat(World& w, int& turn_number);
	void move(World& w, int& turn_number);
	virtual Tile_type who() { return DOODLE_BUG; }
private:
	int breed = 0;
	int eating = 8;
};

class Empty : public Tile
{
public:
	Empty(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 3)
	{
		shape.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256, 255));
		shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
	}
	void display(sf::RenderWindow& window)
	{
		//window.draw(shape);
	}
	virtual Tile_type who() { return EMPTY; }
private:
};

class World
{
public:
	World(int x = X_SIZE, int y = Y_SIZE, int ants = 100, int doodle_bugs = 5);
	//Fantastic Five
	void display(sf::RenderWindow& window);
	void run_simulation();
	void turn();
	~World();
	vector<Tile*>& operator[](int index);
	const vector<Tile*>& operator[](int index) const;
	int get_width() { return grid.size(); }
	//START HERE.
private:
	vector<vector<Tile*>> grid;
};


int main()
{
	srand(time(0));

	World w;

	w.run_simulation();

	return 0;

}

World::World(int x, int y, int ants, int doodle_bugs)
{
	vector<Tile*> column(y);

	for (int i = 0; i < x; i++)
	{
		grid.push_back(column);
	}

	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			if (ants > 0)
			{
				grid[i][j] = new Ant(i, j);
				ants--;
			}
			else if (doodle_bugs > 0)
			{
				grid[i][j] = new Doodle_Bug(i, j);
				doodle_bugs--;
			}
			else
			{
				grid[i][j] = new Empty(i, j);
			}
		}
	}

	int x1, x2;
	int y1, y2;

	//[E][G][R][E]
	//[G][G][G][E]
	//[G][R][E][E]
	for (int i = 0; i < 100000; i++)
	{
		x1 = rand() % x;
		x2 = rand() % x;
		y1 = rand() % y;
		y2 = rand() % y;
		Tile::tile_swap(grid[x1][y1], grid[x2][y2]);
	}



}

void World::display(sf::RenderWindow& window)
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			grid[i][j]->display(window);
		}
	}

}

void World::run_simulation()
{
	sf::RenderWindow window(sf::VideoMode(400, 400), "Ants vs Doodlebugs (WHO WILL WIN?)");

	int count = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			//cout << event.type << endl;
		}
		window.clear();
		count++;
		if (count == 100)
		{
			turn();
			count = 0;
		}
		display(window);
		window.display();
	}
}

void World::turn()
{
	vector<Tile*> doodle_bugs;
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j]->who() == DOODLE_BUG)
			{
				doodle_bugs.push_back(grid[i][j]);
			}
		}
	}

	for (int i = 0; i < doodle_bugs.size(); i++)
	{
		doodle_bugs[i]->turn(*this);
	}

	vector<Tile*> ants;
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j]->who() == ANT)
			{
				ants.push_back(grid[i][j]);
			}
		}
	}

	for (int i = 0; i < ants.size(); i++)
	{
		ants[i]->turn(*this);
	}

}

World::~World()
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			delete grid[i][j];
		}
	}
}

void Tile::tile_swap(Tile*& pTile1, Tile*& pTile2)
{
	swap(pTile1->x, pTile2->x);
	swap(pTile1->y, pTile2->y);
	swap(pTile1, pTile2);
	pTile1->fix_shape_position();
	pTile2->fix_shape_position();
}

void Tile::fix_shape_position()
{
	shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
}

void Doodle_Bug::turn(World& w)
{
	//look for something to eat.
	//if eating fails then...lets just move.
	//allow it to change eating in the function...
	eat(w, Doodle_Bug::breed);
	breeding(w, Doodle_Bug::breed);
	//if they can reproduce then do it.
}

void Ant::turn(World& w)
{
	move(w, Ant::breed);
	breeding(w, Ant::breed);
}



vector<Tile*>& World::operator[](int index)
{
	if (index >= grid.size())
	{
		throw Index_out_of_bounds();
	}
	return grid[index];
}
const vector<Tile*>& World::operator[](int index) const
{
	if (index >= grid.size())
	{
		throw Index_out_of_bounds();
	}
	return grid[index];
}

void Doodle_Bug::move(World& w, int& turn_number)
{
	switch (rand() % NUMBER_OF_DIRECTIONS)
	{
	case UP:
		if (y > 0)
		{
			if (w[x][y - 1]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x][y - 1]);
				turn_number++;
				eating--;
			}
		}
		break;
	case DOWN:
		if (y < w[x].size() - 1)
		{
			if (w[x][y + 1]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x][y + 1]);
				turn_number++;
				eating--;
			}
		}
		break;
	case LEFT:
		if (x > 0)
		{
			if (w[x - 1][y]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x - 1][y]);
				turn_number++;
				eating--;
			}
		}
		break;
	case RIGHT:
		if (x < w.get_width() - 1)
		{
			if (w[x + 1][y]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x + 1][y]);
				turn_number++;
				eating--;
			}
		}
	}
}
void Ant::move(World& w, int& turn_number)
{
	switch (rand() % NUMBER_OF_DIRECTIONS)
	{
	case UP:
		if (y > 0)
		{
			if (w[x][y - 1]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x][y - 1]);
				turn_number++;
			}
		}
		break;
	case DOWN:
		if (y < w[x].size() - 1)
		{
			if (w[x][y + 1]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x][y + 1]);
				turn_number++;
			}
		}
		break;
	case LEFT:
		if (x > 0)
		{
			if (w[x - 1][y]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x - 1][y]);
				turn_number++;
			}
		}
		break;
	case RIGHT:
		if (x < w.get_width() - 1)
		{
			if (w[x + 1][y]->who() == EMPTY)
			{
				Tile::tile_swap(w[x][y], w[x + 1][y]);
				turn_number++;
			}
		}
	}
}
void Ant::breeding(World& w, int& turn_number) {
	if (turn_number == 3) {
			if (y > 0 && w[x][y - 1]->who() == EMPTY)
			{
					//delet empty add new blob
					delete w[x][y - 1];
					w[x][y - 1] = new Ant(x, y - 1);
					turn_number = 0;
			}
			else if (y < w[x].size() - 1 && w[x][y + 1]->who() == EMPTY)
			{
					delete w[x][y + 1];
					w[x][y + 1] = new Ant(x, y + 1);
					turn_number = 0;
			}
			else if (x > 0 && w[x - 1][y]->who() == EMPTY)
			{
					delete w[x - 1][y];
					w[x - 1][y] = new Ant(x - 1, y);
					turn_number = 0;
			}
			else if (x < w.get_width() - 1 && w[x + 1][y]->who() == EMPTY)
			{
					delete w[x + 1][y];
					w[x + 1][y] = new Ant(x + 1, y);
					turn_number = 0;
			}
			turn_number = 0;
		}
}
void Doodle_Bug::breeding(World& w, int& turn_number) {
	if (eating == 0 && turn_number == 8) {
		int temp1 = x;
		int temp2 = y;
		delete w[x][y];
		w[temp1][temp2] = new Empty(temp1, temp2);
	}
	else if (turn_number == 8 && eating != 0) {
		if (y > 0 && w[x][y - 1]->who() == EMPTY)
		{
			//delet empty add new blob
			turn_number = 0;
			eating = 8;
			delete w[x][y - 1];
			w[x][y - 1] = new Doodle_Bug(x, y - 1);
			Tile::tile_swap(w[x][y], w[x][y - 1]);

		}
		else if (y < w[x].size() - 1 && w[x][y + 1]->who() == EMPTY)
		{
			turn_number = 0;
			eating = 8;
			delete w[x][y + 1];
			w[x][y + 1] = new Doodle_Bug(x, y + 1);
			Tile::tile_swap(w[x][y], w[x][y + 1]);
		}
		else if (x > 0 && w[x - 1][y]->who() == EMPTY)
		{
			turn_number = 0;
			eating = 8;
			delete w[x - 1][y];
			w[x - 1][y] = new Doodle_Bug(x - 1, y);
			Tile::tile_swap(w[x][y], w[x - 1][y]);
		}
		else if (x < w.get_width() - 1 && w[x + 1][y]->who() == EMPTY)
		{
			turn_number = 0;
			eating = 8;
			delete w[x + 1][y];
			w[x + 1][y] = new Doodle_Bug(x + 1, y);
			Tile::tile_swap(w[x][y], w[x + 1][y]);
		}


	}
}
void Doodle_Bug::eat(World& w, int& turn_number) {
	if (y > 0 && w[x][y - 1]->who() == ANT)
	{
			delete w[x][y - 1];
			w[x][y - 1] = new Empty (x, y-1);
			Tile::tile_swap(w[x][y], w[x][y - 1]);
			eating = 8;
	}
	else if (y < w[x].size() - 1 && w[x][y + 1]->who() == ANT)
	{
			delete w[x][y + 1];
			w[x][y + 1] = new Empty(x, y + 1);
			Tile::tile_swap(w[x][y], w[x][y + 1]);
			eating = 8;
	}
	else if (x > 0 && w[x - 1][y]->who() == ANT)
	{
			delete w[x - 1][y];
			w[x - 1][y] = new Empty(x - 1, y);
			Tile::tile_swap(w[x][y], w[x - 1][y]);
			eating = 8;
	}
	else if (x < w.get_width() - 1 && w[x + 1][y]->who() == ANT)
	{
			delete w[x + 1][y];
			w[x + 1][y] = new Empty(x + 1, y);
			Tile::tile_swap(w[x][y], w[x + 1][y]);
			eating = 8;
	}
	else {
		move(w, turn_number);
	}
}// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
