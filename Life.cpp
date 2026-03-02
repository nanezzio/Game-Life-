#include <iostream>
#include <fstream>
#include <Windows.h>   
#include <cstdlib>    

bool** createField(int rows, int cols)
{
    bool** a = new bool* [rows];
    for (int i = 0; i < rows; ++i)
    {
        a[i] = new bool[cols];
        for (int j = 0; j < cols; ++j)
            a[i][j] = false;
    }
    return a;
}

void deleteField(bool** a, int rows)
{
    for (int i = 0; i < rows; ++i)
        delete[] a[i];
    delete[] a;
}

void copyField(bool** from, bool** to, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            to[i][j] = from[i][j];
}

bool fieldsEqual(bool** a, bool** b, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (a[i][j] != b[i][j])
                return false;
    return true;
}

bool loadFromFile(const std::string& filename,
    bool**& field, int& rows, int& cols)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Cannot open file: " << filename << "\n";
        return false;
    }

    fin >> rows >> cols;
    if (!fin || rows <= 0 || cols <= 0)
    {
        std::cerr << "Incorrect field size in file\n";
        return false;
    }

    field = createField(rows, cols);

    int r, c;
    while (fin >> r >> c)
    {
        if (r >= 0 && r < rows && c >= 0 && c < cols)
            field[r][c] = true;
    }
    return true;
}

int countAliveNeighbours(bool** field, int rows, int cols, int r, int c)
{
    static const int dr[8] = { -1,-1,-1, 0, 0, 1, 1, 1 };
    static const int dc[8] = { -1, 0, 1,-1, 1,-1, 0, 1 };
    int cnt = 0;

    for (int k = 0; k < 8; ++k)
    {
        int nr = r + dr[k];
        int nc = c + dc[k];
        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
            if (field[nr][nc])
                ++cnt;
    }
    return cnt;
}

int countAliveCells(bool** field, int rows, int cols)
{
    int alive = 0;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (field[i][j])
                ++alive;
    return alive;
}

void printField(bool** field, int rows, int cols, int generation, int alive)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            std::cout << (field[i][j] ? "*" : "-");
            if (j + 1 < cols) std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Generation: " << generation
        << ". Alive cells: " << alive << "\n";
}

bool makeNextGeneration(bool** current, bool** next, int rows, int cols)
{
    bool changed = false;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            int neighbours = countAliveNeighbours(current, rows, cols, i, j);
            bool nowAlive = current[i][j];
            bool willAlive = false;

            if (!nowAlive && neighbours == 3)
            {
                willAlive = true;
            }
            else if (nowAlive && (neighbours == 2 || neighbours == 3))
            {
                willAlive = true;
            }
            else
            {
                willAlive = false;
            }

            next[i][j] = willAlive;
            if (willAlive != nowAlive)
                changed = true;
        }
    }
    return changed;
}

int main()
{
    std::string filename;
    std::cout << "Enter input file path: ";
    std::cin >> filename;

    int rows = 0, cols = 0;
    bool** current = nullptr;

    if (!loadFromFile(filename, current, rows, cols))
        return 1;

    bool** previous = createField(rows, cols);
    bool** next = createField(rows, cols);

    int generation = 1;

    while (true)
    {
        std::system("cls");
        int alive = countAliveCells(current, rows, cols);
        printField(current, rows, cols, generation, alive);

        if (alive == 0)
        {
            std::cout << "All cells are dead. Game over.\n";
            break;
        }

        if (generation > 1 && fieldsEqual(current, previous, rows, cols))
        {
            std::cout << "The world has stagnated. Game over.\n";
            break;
        }

        Sleep(1000);

        copyField(current, previous, rows, cols);
        bool anyChanged = makeNextGeneration(current, next, rows, cols);

        if (!anyChanged)
        {
            std::cout << "The world has stagnated. Game over.\n";
            break;
        }

        std::swap(current, next);
        ++generation;
    }

    deleteField(current, rows);
    deleteField(previous, rows);
    deleteField(next, rows);
}