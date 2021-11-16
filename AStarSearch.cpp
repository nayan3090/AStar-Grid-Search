#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <array>

using std::cout;
using std::ifstream;
using std::istringstream;
using std::vector;
using std::string;
using std::abs;
using std::sort;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
const int len_delta = 4;

vector<State> ParseLine(string line)
{
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',')
    {
        if (n == 1)
        {
            row.push_back(State::kObstacle);
        }
        else
        {
            row.push_back(State::kEmpty);
        }
    }
    return row;
}

vector<vector<State>> ReadBoardFile(string file_path)
{
    ifstream FileToRead (file_path);
    vector<vector<State>> board;
    string line;
    while(getline(FileToRead, line))
    {
        vector<State> row = ParseLine(line);
        board.push_back(row);
    }
    return board;
}

/**
 * Compare the F values of two cells.
 */
bool Compare(const vector<int> node1, const vector<int> node2)
{
    int node1f = node1[2] + node1[3];
    int node2f = node2[2] + node2[3];
    return node1f>node2f;
}

/**
 * Sort the two-dimensional vector of ints in descending order.
 */
void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

// Calculate the manhattan distance
int Heuristic(int x1, int y1, int x2, int y2)
{
    return abs(x2-x1)+abs(y2-y1);
}

/*
 * Check valid neighboring nodes a
 */ 
bool CheckValidCell(int x, int y, vector<vector<State>> &grid)
{
    int m = grid.size();
    int n = grid[x].size();
    if (x >= 0 && x < m && y >= 0 && y < n)
    {
        return (grid[x][y] == State::kEmpty);
    }
    return false;
}

/** 
 * Add a node to the open list and mark it as open. 
 */
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist, vector<vector<State>> &grid)
{
    vector<int> node = {x, y, g, h};
    openlist.push_back(node);
    grid[x][y] = State::kClosed;
}

/** 
 * Expand current nodes's neighbors and add them to the open list.
 */
void ExpandNeighbors(vector<int> &current_node, vector<vector<int>> &open, vector<vector<State>> &grid, int goal[2])
{
    int x = current_node[0];
    int y = current_node[1];
    int g = current_node[2];

    for (int i = 0; i < len_delta; i++)
    {
        int x_new = x + delta[i][0];
        int y_new = y + delta[i][1];

        if (CheckValidCell(x_new, y_new, grid))
        {
            int g_new = g + 1;
            int h_new = Heuristic(x_new, y_new, goal[0], goal[1]);
            AddToOpen(x_new, y_new, g_new, h_new, open, grid);
        }
    }
}

/** 
 * Implementation of A* search algorithm
 */
vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2])
{
    vector<vector<int>> open = {};
    int x = init[0];
    int y = init[1];
    int g = 0;
    int h = Heuristic(x, y, goal[0], goal[1]);
    AddToOpen(x, y, g, h, open, grid);

    while (!open.empty())
    {
        CellSort(&open);
        auto current_node = open.back();
        open.pop_back();
        grid[current_node[0]][current_node[1]] = State::kPath;
        if (current_node[0] == goal[0] && current_node[1] == goal[1])
        {
            grid[init[0]][init[1]] = State::kStart;
            grid[goal[0]][goal[1]] = State::kFinish;
            return grid;
        }
        ExpandNeighbors(current_node, open, grid, goal);
    }
    cout << "No path found!" << "\n";
    return vector<vector<State>>{}; 
}

string CellString(State cell)
{
    switch(cell)
    {
    case State::kObstacle:
        return "⛰️";
    case State::kPath:
        return "🚗";
    case State::kStart:
        return "🚦";
    case State::kFinish:
        return "🏁";
    default:
        return "0";
    }
}

void PrintBoard(vector<vector<State>> board)
{
    for(int i = 0; i < board.size(); i++)
    {
        for(int j = 0; j < board[i].size(); j++)
        {
            cout << CellString(board[i][j]) << "\t|\t";
        }
        cout << "\n";
    }
}

int main() 
{
    int init[2] = {0,0};
    int goal[2] = {4,5};
    auto Board = ReadBoardFile("/home/t9882nd/Documents/Udacity_CPP/1.board");
    auto Board_with_path = Search(Board, init, goal);
    PrintBoard(Board_with_path);
}