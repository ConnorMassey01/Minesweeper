#include <iostream>
#include <stdlib.h>
#include <queue>
#include "time.h"

#define NUM_ROWS 10
#define NUM_COLS 10
#define NUM_MINES 12

using namespace std;

void initGrid(char grid[NUM_ROWS][NUM_COLS], char ch){
    for(int row = 0; row < NUM_ROWS; row++){
        for (int col = 0; col < NUM_COLS; col++){
            grid[row][col] = ch;
        }
    }
}

bool isValid(int row, int col){
    if(row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS){
        return true;
    }
    return false;
}

void createGameGrid(char grid[NUM_ROWS][NUM_COLS]){
    srand(time(NULL));
    int addedMines = 0;
    int row, col, adjMines;
    const int patternSize = 8;
    int rowPattern[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    int colPattern[8] = {1, -1, 0, 0, 1, -1, -1, 1};
    while (addedMines < NUM_MINES){
        row = rand() % 10;
        col = rand() % 10;
        if(grid[row][col] != 'M'){
            grid[row][col] = 'M';
            addedMines++;
        }
    }
    for(int row = 0; row < NUM_ROWS; row++){
        for(int col = 0; col < NUM_COLS; col++){
            adjMines = 0;
            if(grid[row][col] != 'M'){
                for(int i = 0; i < patternSize; i++){
                    if(isValid(row + rowPattern[i], col + colPattern[i])){
                        if(grid[row + rowPattern[i]][col + colPattern[i]] == 'M'){
                            adjMines++;
                        }
                    }
                }
                grid[row][col] = '0' + adjMines;
            }
        }
    }
}

void printGrid(char grid[NUM_ROWS][NUM_COLS]){
    cout << "   ";
    for(int i = 0; i < NUM_COLS; i++){
        cout << i << ' ';
    }
    cout << endl;
    for(int row = 0; row < NUM_ROWS; row++){
        if(row < 10){
            cout << row << "  ";
        }
        else{
            cout << row << " ";
        }
        for (int col = 0; col < NUM_COLS; col++){
            cout << grid[row][col] << " ";
        }
        cout << endl;
    }
}

void printInstructions(){
    cout << "--------Controls--------\n";
    cout << "Command c: uncover a position\n";
    cout << "Command f: flag a mine\n";
    cout << "Command ?: mark a position unknown\n";
    cout << "Enter: <command> <row> <column> to invoke a command at the position on the gird\n";
}

void uncoverGrid(char gameGrid[NUM_ROWS][NUM_COLS], char hiddenGrid[NUM_ROWS][NUM_COLS], int row, int col){
    if(hiddenGrid[row][col] == '0'){
        //set up needed variables
        pair<int, int> cell;
        queue<pair<int, int>> q;
        bool visited[NUM_ROWS][NUM_COLS];
        for(int i = 0; i < NUM_ROWS; i++){
            for(int j = 0; j < NUM_COLS; j++){
                visited[i][j] = false;
            }
        }
        int rowPattern[8] = {0, 0, 1, -1, 1, -1, 1, -1};
        int colPattern[8] = {1, -1, 0, 0, 1, -1, -1, 1};
        const int patternSize = 8;

        q.push({row, col});
        visited[row][col] = true;
        //breadth first search to change all connected 0's to spaces
        while(!q.empty()){
            //get element from front of queue
            cell = q.front();
            q.pop();
            row = cell.first;
            col = cell.second;
            //make the game grid at that location a space character
            gameGrid[row][col] = hiddenGrid[row][col];
    
            //check all directions
            for(int i = 0; i < patternSize; i++){
                if(isValid(row + rowPattern[i], col + colPattern[i])){
                    if(!visited[row + rowPattern[i]][col + colPattern[i]] && hiddenGrid[row][col] == '0'){
                        q.push({row + rowPattern[i], col + colPattern[i]});
                        visited[row + rowPattern[i]][col + colPattern[i]] = true;
                    }
                }
            }
        }
    }
    else{
        gameGrid[row][col] = hiddenGrid[row][col];
    }
}

void gameLoop(char gameGrid[NUM_ROWS][NUM_COLS], char hiddenGrid[NUM_ROWS][NUM_COLS]){
    char inputCommand;
    int inputRow, inputCol;
    int minesFound = 0;
    int incorrectFlag = 0;
    int gameOver = 0;
    unsigned long int startTime, finishTime;
    printInstructions();
    startTime = clock();
    while(!gameOver){
        cout << "Flags placed: " << minesFound + incorrectFlag << endl;
        printGrid(gameGrid);
        cin >> inputCommand >> inputRow >> inputCol;
        if(inputCommand == 'c'){
            if(inputRow > -1 && inputRow < NUM_ROWS && inputCol > -1 && inputCol < NUM_COLS){
                if(hiddenGrid[inputRow][inputCol] == 'M'){
                    gameGrid[inputRow][inputCol] = hiddenGrid[inputRow][inputCol];
                    gameOver = 1;
                }
                else if(gameGrid[inputRow][inputCol] == 'F'){
                    incorrectFlag--;
                    uncoverGrid(gameGrid, hiddenGrid, inputRow, inputCol);
                }
                else{
                    uncoverGrid(gameGrid, hiddenGrid, inputRow, inputCol);
                }
            }
            else{
                cout << "Invalid position given\n";
            }
        }
        else if(inputCommand == 'f'){
            if(hiddenGrid[inputRow][inputCol] == 'M'){
                gameGrid[inputRow][inputCol] = 'F';
                minesFound++;
            }
            else{
                //mark the flag but do not increase number of mines found
                gameGrid[inputRow][inputCol] = 'F';
                incorrectFlag++;
            }
        }
        else if(inputCommand == '?' || inputCommand == '/'){
            gameGrid[inputRow][inputCol] = '?';
        }
        else{
            //incorrect command given
            cout << "Invalid input command\n";
        }
        if(minesFound == NUM_MINES && incorrectFlag == 0){
            finishTime = clock();
            cout << "Flags placed: " << minesFound + incorrectFlag << endl;
            printGrid(gameGrid);
            cout << "YOU WIN!\n";
            cout << "Game completed in " << (finishTime - startTime)/CLOCKS_PER_SEC << " seconds!\n";
            return;
        }
    }
    printGrid(gameGrid);
    cout << "MINE HIT!\nGAME OVER";
    cout << "\nYou found " << minesFound << " of the " << NUM_MINES << " mines\n";
    printGrid(hiddenGrid);
}

int main(){
    char hiddenGrid[NUM_ROWS][NUM_COLS];
    char gameGrid[NUM_ROWS][NUM_COLS];

    initGrid(gameGrid, '*');
    //printGrid(gameGrid);
    initGrid(hiddenGrid, ' ');
    createGameGrid(hiddenGrid);
    //printGrid(hiddenGrid);
    gameLoop(gameGrid, hiddenGrid);

}