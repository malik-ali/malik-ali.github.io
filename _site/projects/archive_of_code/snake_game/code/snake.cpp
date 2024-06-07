#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <chrono>
#include <thread>

using namespace std;

const char EMPTY_TILE   =   ' ';
const char SNAKE_TILE   =   '*';
const char WALL_TILE    =   '#';
const char FOOD_TILE    =   '$';
const int WIN_THRESHOLD =   15; 
const int WAIT_MS       =   100;
const double TURN_RATE  =   0.1;
const string CLEAR_CMD  =   "clear";    // "cls" for windows

//using pointT = pair<int, int>;

struct pointT {
    int row, col;
};

struct gameT {
    vector<string> world;
    int numRows, numCols;
    deque<pointT> snake;
    int dx, dy;
    int numEaten = 0;
};

void init_game(gameT &game);
void open_file(ifstream &inp);
void load_world(gameT &game, ifstream &inp);
void print_board(gameT &game);
void next_move(gameT &game);
bool make_move(gameT &game);
bool run_simulation(gameT &game);
void pause(int ms);

int main() {
    gameT game;
    init_game(game);
    print_board(game);
    if(run_simulation(game)) {
        cout << "You win!" << endl;
    } else {
        cout << "You lose!" << endl;
    }
}

void init_game(gameT &game) {
    srand(static_cast<unsigned int>(time(NULL)));
    ifstream inp;
    open_file(inp);
    load_world(game, inp);
}

pointT make_point(int row, int col) {
    pointT ret;
    ret.row = row;
    ret.col = col;
    return ret;
}

void load_world(gameT &game, ifstream &inp) {
    inp >> game.numRows >> game.numCols;
    game.world.resize(game.numRows);
    inp >> game.dx >> game.dy;
    string dummy;
    getline(inp, dummy);    // clear stream of the newline character
    for(int row = 0; row < game.numRows; ++row) {
        getline(inp, game.world[row]);
        int col = game.world[row].find(SNAKE_TILE);
        if(col != string::npos) {
            game.snake.push_back(make_point(row, col));
        }
    }
    game.numEaten = 0;
}

void open_file(ifstream &inp) {
    while(true) {
        cout << "Enter filename: " << endl;
        string name;
        getline(cin, name);
        inp.open(name.c_str());
        if(inp.is_open()) return;
        cout << "Could not find file " << name << endl;
        inp.clear();
    }
}


bool run_simulation(gameT &game) {
    while(game.numEaten <= WIN_THRESHOLD) {
        print_board(game);
        next_move(game);
        if(!make_move(game)) return false;
        pause(WAIT_MS);
    }
    return true;

}

void print_board(gameT &game) {
    system(CLEAR_CMD.c_str());
    for(int row = 0; row < game.numRows; ++row) {
        cout << game.world[row] << endl;
    } 
    cout << "Food eaten: " << game.numEaten << endl;
}

pointT next_pos( pointT curr, int dx, int dy) {
    pointT ret = curr;
    ret.row += dx;
    ret.col += dy;
    return ret;
}

bool randomChance(double p) {
    return (rand() / (RAND_MAX + 1.0)) < p;
}

bool in_bounds(pointT pos, const gameT &game) {
    return pos.row >= 0 && pos.col >= 0 
            && pos.row < game.numRows && pos.col < game.numCols;
}

bool inline crash(pointT pos, const gameT &game) {
    return !in_bounds(pos, game) || game.world[pos.row][pos.col] == WALL_TILE 
                || game.world[pos.row][pos.col] == SNAKE_TILE; 
}

void next_move(gameT &game) {
    pointT next = next_pos(game.snake.front(), game.dx, game.dy);
    if(crash(next, game) || randomChance(TURN_RATE)) {
        int left_dx = -game.dy;
        int left_dy = game.dx;
        int right_dx = game.dy;
        int right_dy = -game.dx;
        bool left_crash = crash(next_pos(game.snake.front(), left_dx, left_dy), game);
        bool right_crash = crash(next_pos(game.snake.front(), right_dx, right_dy), game);
        bool turn_left = false;
        if(left_crash && right_crash) {
            return;
        } else if(left_crash) {
            turn_left = false;
        } else if(right_crash) {
            turn_left = true;
        } else {
            turn_left = randomChance(0.5);
        }
        game.dx = turn_left ? left_dx : right_dx;
        game.dy = turn_left ? left_dy : right_dy;
    }

}

void spawn_food(gameT &game) {
    while(true) {
        int row = rand() % game.numRows;
        int col = rand() % game.numCols;
        if(game.world[row][col] == EMPTY_TILE) {
            game.world[row][col] = FOOD_TILE;
            return;
        }
    }

}

bool make_move(gameT &game) {
    pointT next = next_pos(game.snake.front(), game.dx, game.dy);
    if(crash(next, game)) return false;
    bool is_food = game.world[next.row][next.col] == FOOD_TILE;
    game.snake.push_front(next);
    game.world[next.row][next.col] = SNAKE_TILE;
    if(!is_food) {
        pointT end = game.snake.back();
        game.snake.pop_back();
        game.world[end.row][end.col] = EMPTY_TILE;
    } else {
        ++game.numEaten;
        spawn_food(game);
    }
    return true;
}

void pause(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
