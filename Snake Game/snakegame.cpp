#include <iostream>
#include <fstream>

using namespace std;

const int MAX_ROWS = 20;
const int MAX_COLS = 20;
const char WALL = '#';
const char EMPTY = ' ';
const char PLAYER = '^';
const char PLAYERUP = '^';
const char PLAYERDOWN = 'v';
const char PLAYERRIGHT = '>';
const char PLAYERLEFT = '<';
const char SNAKE = '~';
const char GOAL = 'X';
const char OBSTACLE = 'O';
int moves = 0;

struct Location {
    int row;
    int col;
};

template <typename T>
struct Node {
    T data;
    Node* next;
};

template <typename T>
class LinkedList {
public:
    LinkedList() : head(nullptr) {}

    void push(const T& value) {
        Node<T>* newNode = new Node<T>;
        newNode->data = value;
        newNode->next = head;
        head = newNode;
    }

    T pop() {
        if (head) {
            T value = head->data;
            Node<T>* temp = head;
            head = head->next;
            delete temp;
            return value;
        } else {
            
            return T(); 
        }
    }

    bool isEmpty() {
        return head == nullptr;
    }

    ~LinkedList() {
        while (!isEmpty()) {
            pop();
        }
    }

private:
    Node<T>* head;
};

class MazeGame {
public:
    MazeGame(const string& mapFileName);
    void play();

private:
    char gameMap[MAX_ROWS][MAX_COLS];
    int numRows;
    int numCols;
    int numObj;

    Location player;
    Location snake;
    Location goal;
    LinkedList<Location> moveHistory;
    LinkedList<Location> moveSnakeHistory; 

    void loadMap(const string& mapFileName);
    void printMap();
    bool isValidMove(int newRow, int newCol);
    bool isGameOver();
    bool movePlayer(char move);
    bool moveSnake();
    void undo();
    bool isWin();
};

MazeGame::MazeGame(const string& mapFileName) {
    loadMap(mapFileName);
   
}

void MazeGame::loadMap(const string& mapFileName) {
    ifstream mapFile(mapFileName);
    if (!mapFile) {
        cerr << "Error: File not found." << endl;
        exit(1);
    }
   
    mapFile >> numObj >> numRows >> numCols;

   
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            gameMap[i][j] = EMPTY;
        }
    }
    for (int i = 0; i < numObj; i++) {
        int r, c, code;
        mapFile >> r >> c >> code;
        if (code == 0) { // player location
            player.row = r;
            player.col = c;
            gameMap[r][c] = PLAYER;
        } else if (code == 1) { // snake
            snake.row = r;
            snake.col = c;
            gameMap[r][c] = SNAKE;
        } else if (code == 2) { // goal
            goal.row = r;
            goal.col = c;
            gameMap[r][c] = GOAL;
        } else if (code == 3) {
            gameMap[r][c] = OBSTACLE;
            // wall
        }
    }
    for (int i = 0; i < numRows; i++) {
        gameMap[i][0] = WALL;
        gameMap[i][numCols - 1] = WALL;
    }
    for (int i = 0; i < numCols; i++) {
        gameMap[0][i] = WALL;
        gameMap[numRows - 1][i] = WALL;
    }
    // Close the map file.
    mapFile.close();
}
void MazeGame::printMap() {
    system("cls");  
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            cout << gameMap[row][col] << ' ';
        }
        cout << endl;
    }
}
bool MazeGame::isValidMove(int newRow, int newCol) {
    return (newRow >= 0 && newRow < numRows && newCol >= 0 && newCol < numCols &&
            gameMap[newRow][newCol] != OBSTACLE && gameMap[newRow][newCol] != WALL);
}
bool MazeGame::movePlayer(char move) {
    int newRow = player.row;
    int newCol = player.col;
    if (move == 'w') newRow--;
    else if (move == 's') newRow++;
    else if (move == 'a') newCol--;
    else if (move == 'd') newCol++;
    if (isValidMove(newRow, newCol)) {
        moveHistory.push(player);  // Use the push method of the linked list
        gameMap[player.row][player.col] = EMPTY;
        player.row = newRow;
        player.col = newCol;
        if(move=='w')gameMap[player.row][player.col] = PLAYERUP;
       else if(move=='s')gameMap[player.row][player.col] = PLAYERDOWN;
       else if(move=='a')gameMap[player.row][player.col] = PLAYERLEFT;
       else if(move=='d')gameMap[player.row][player.col] = PLAYERRIGHT;
        return true;
    }
    return false;
}
bool MazeGame::moveSnake() {
    // Calculate the new snake position
    int newRow = snake.row;
    int newCol = snake.col;
    // If the snake is moving towards the player, update its position
    if (player.row < newRow&&gameMap[newRow-1][newCol]!=OBSTACLE) newRow--;
    else if (player.row > newRow&&gameMap[newRow+1][newCol]!=OBSTACLE) newRow++;
    else if (player.col < newCol&&gameMap[newRow][newCol-1]!=OBSTACLE) newCol--;
    else if (player.col > newCol&&gameMap[newRow][newCol+1]!=OBSTACLE) newCol++;
    // Update the snake's position
    if (isValidMove(newRow,newCol)) {
        moveSnakeHistory.push(snake);
        gameMap[snake.row][snake.col] = EMPTY;
        snake.row = newRow;
        snake.col = newCol;
        gameMap[snake.row][snake.col] = SNAKE;
    }
    // Check if the snake has bitten the player
    if (player.row == snake.row && player.col == snake.col) {
        return true;
    } else {
        return false;
    }
}
void MazeGame::undo() {
    if (!moveHistory.isEmpty()) {
        cout<<1<<endl;
        Location previous = moveHistory.pop();  // Use the pop method of the linked list
        gameMap[player.row][player.col] = EMPTY;
        player = previous;
        gameMap[player.row][player.col] = PLAYER;
    }
    if (!moveSnakeHistory.isEmpty()) {
        Location prev = moveSnakeHistory.pop(); 
        cout<<1<<endl; // Use the pop method of the linked list
        gameMap[snake.row][snake.col] = EMPTY;
        snake = prev;
        gameMap[snake.row][snake.col] = SNAKE;
    }
}
bool MazeGame::isWin() {
    return (player.row == goal.row && player.col == goal.col);
}
bool MazeGame::isGameOver() {
    return (player.row == snake.row && player.col == snake.col);
}
void MazeGame::play(){
    while (true) {
        printMap();

        if (isWin()) {
            cout << "Congratulations! You won!" << endl;
            break;
        }    
        char move;
        cout << "Enter move (w(up)/a(left)/s(down)/d(right)/u(undo)): ";
        cin >> move;

        if (move == 'u') {
            undo();
        } else if (move == 'w' || move == 'a' || move == 's' || move == 'd') {
            movePlayer(move);
             if (moveSnake()) {
            cout << "Game Over! Snake bit you." << endl;
            break;
        }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        const string mapFileName = argv[1];
        MazeGame game(mapFileName);
        game.play();
    } else {
        cout << "Please provide a file name as an argument." << endl;
    }
    return 0;
}