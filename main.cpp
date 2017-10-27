#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;

#define RESET           0
#define BRIGHT          1
#define DIM             2
#define UNDERLINE       3
#define BLINK           4
#define REVERSE         7
#define HIDDEN          8
#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7

struct Board{
    int b[8][7];
    int sRow, sCol, fRow, fCol;
    bool attacking = false;
};

Board CopyBoard(Board myBoard){
    Board newBoard;
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 7; j++){
            newBoard.b[i][j] = myBoard.b[i][j];
            newBoard.sRow = myBoard.sRow;
            newBoard.sCol = myBoard.sCol;
            newBoard.fRow = myBoard.fRow;
            newBoard.fCol = myBoard.fCol;
            newBoard.attacking = myBoard.attacking;
        }
    return newBoard;
}

int maxdepth=6;
bool turnorder = false;

enum Piece{
    R_KING = 9999,
    R_SAM = 10,
    R_NIN = 8,
    R_MINI_SAM = 4,
    R_MINI_NIN = 3,
    B_KING = -9999,
    B_SAM = -10,
    B_NIN = -8,
    B_MINI_SAM = -4,
    B_MINI_NIN = -3,
    EMPTY = 0
};

enum Column{
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6
};

void makemove();
vector<Board> movegen(Board myBoard);
int min(int depth, Board myBoard, int alpha, int beta);
int max(int depth, Board myBoard, int alpha, int beta);
int evaluate(Board b);
int check4winner();
void checkGameOver();
void getamove();
void setup();
void printboard(Board b);
bool ask4turnorder();
bool checkWinner(Board b);

Board board;
vector<Board> turnMoves;
void textcolor(int attr, int fg, int bg)
{ char command[13];
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf("%s", command);
}


int main(){
    setup();
    turnorder = ask4turnorder();
    printboard(board);
    if(turnorder){
        while(1){
           getamove();
           checkGameOver();
           makemove();
           checkGameOver();
        }
    } else{
        while(1){
           makemove();
           checkGameOver();
           getamove();
           checkGameOver();
        }
    }
}

bool ask4turnorder(){
    string s;
    cout << "Would you like to go first? (y for yes n for no): ";
    cin >> s;
    if(s.compare("y") != 0 && s.compare("n") != 0) return ask4turnorder();
    else if(s.compare("y")) return false;
    else return true;
}

int attack(int piece){
    switch(piece){
        case R_SAM: return R_MINI_SAM;
        case R_NIN: return R_MINI_NIN;
        case B_SAM: return B_MINI_SAM;
        case B_NIN: return B_MINI_NIN;
        default: return EMPTY;
    }
}

vector<Board> movegen(Board myBoard, int team){
    vector<Board> legalMoves;
    Board proxyBoard = CopyBoard(myBoard);
    Board attackProxy = CopyBoard(proxyBoard);
    int myPiece, m, n;
    
    //Loop through board array
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 7; j++){
            //collect what piece we are working with
            myPiece = myBoard.b[i][j];
            if(myPiece == EMPTY) continue;
            
            //Switch on piece type for different move sets
            if(team == RED){
            switch(myPiece){
                case R_SAM:
                    //Check for forward moves
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    for(m = i+1; proxyBoard.b[m][j] == EMPTY && m < 8; m++){
                        proxyBoard.b[m-1][j] = EMPTY;
                        proxyBoard.b[m][j] = R_SAM;
                        proxyBoard.fCol = j;
                        proxyBoard.fRow = m;
                        if(proxyBoard.b[m+1][j] < 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m+1][j] = attack(attackProxy.b[m+1][j]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves to the right
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    for(n = j+1; n < 7 && proxyBoard.b[i][n] == EMPTY; n++){
                        proxyBoard.b[i][n-1] = EMPTY;
                        proxyBoard.b[i][n] = R_SAM;
                        if(proxyBoard.b[i+1][n] < 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = i;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i+1][n] = attack(attackProxy.b[i+1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves to the left
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    for(n = j-1; n >= 0 && proxyBoard.b[i][n] == EMPTY; n--){
                        proxyBoard.b[i][n+1] = EMPTY;
                        proxyBoard.b[i][n] = R_SAM;
                        if(proxyBoard.b[i+1][n] < 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = i;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i+1][n] = attack(attackProxy.b[i+1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    break;
                case R_NIN:
                    //Check for Diagonal Left
                    m = i+1;
                    n = j-1;
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    while(m < 8 && n >= 0 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m-1][n+1] = EMPTY;
                        proxyBoard.b[m][n] = R_NIN;
                        proxyBoard.fCol = n;
                        proxyBoard.fRow = m;
                        if(proxyBoard.b[m+1][n] < 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m+1][n] = attack(attackProxy.b[m+1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                        m++;
                        n--;
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for Diagonal Right
                    m = i+1;
                    n = j+1;
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    while(m < 8 && n < 7 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m-1][n-1] = EMPTY;
                        proxyBoard.b[m][n] = R_NIN;
                        proxyBoard.fCol = n;
                        proxyBoard.fRow = m;
                        if(proxyBoard.b[m+1][n] < 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m+1][n] = attack(attackProxy.b[m+1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                        m++;
                        n++;
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves backwards left
                    m = i - 1;
                    n = j - 1;
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    while(m >= 0 && n >= 0 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m+1][n+1] = EMPTY;
                        proxyBoard.b[m][n] = R_NIN;
                        if(proxyBoard.b[m+1][n] < 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = m;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m+1][n] = attack(attackProxy.b[m+1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        m--;
                        n--;
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves backwards right
                    m = i - 1;
                    n = j + 1;
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                   while(m >= 0 && n < 7 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m+1][n-1] = EMPTY;
                        proxyBoard.b[m][n] = R_NIN;
                        if(proxyBoard.b[m+1][n] < 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = m;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m+1][n] = attack(attackProxy.b[m+1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        m--;
                        n++;
                    }
                    proxyBoard = CopyBoard(myBoard);
                    break;
                case R_MINI_SAM:
                    //Check for forward move
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(i+1 < 8 && proxyBoard.b[i+1][j] == EMPTY){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i+1][j] = R_MINI_SAM;
                        proxyBoard.fCol = j;
                        proxyBoard.fRow = i+1;
                        if(proxyBoard.b[i+2][j] < 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i+2][j] = EMPTY;
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for left attacking move
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(j-1 >= 0 && proxyBoard.b[i][j-1] == EMPTY && proxyBoard.b[i+1][j-1] < 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i][j-1] = R_MINI_SAM;
                        proxyBoard.fCol = j-1;
                        proxyBoard.fRow = i;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i+1][j-1] = attack(attackProxy.b[i+1][j-1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for right attacking move
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(j+1 < 7 && proxyBoard.b[i][j+1] == EMPTY && proxyBoard.b[i+1][j+1] < 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i][j+1] = R_MINI_SAM;
                        proxyBoard.fCol = j+1;
                        proxyBoard.fRow = i;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i+1][j+1] = attack(attackProxy.b[i+1][j+1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
                    break;
                case R_MINI_NIN:
                    //Check for diagonal right
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i+1][j+1] == EMPTY && i+1 < 8 && j+1 < 7){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i+1][j+1] = R_MINI_NIN;
                        proxyBoard.fCol = j+1;
                        proxyBoard.fRow = i+1;
                        if(proxyBoard.b[i+2][j+1] < 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i+2][j+1] = attack(attackProxy.b[i+2][j+1]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for diagonal left
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i+1][j-1] == EMPTY && i+1 < 8 && j-1 >= 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i+1][j-1] = R_MINI_NIN;
                        proxyBoard.fCol = j-1;
                        proxyBoard.fRow = i+1;
                        if(proxyBoard.b[i+2][j-1] < 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i+2][j-1] = attack(attackProxy.b[i+2][j-1]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking back right
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i-1][j+1] == EMPTY && i+1 < 8 && j+1 < 7 && proxyBoard.b[i][j+1] < 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i-1][j+1] = R_MINI_NIN;
                        proxyBoard.fCol = j+1;
                        proxyBoard.fRow = i-1;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i][j+1] = attack(attackProxy.b[i][j+1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking back left
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i-1][j-1] == EMPTY && i+1 < 8 && j+1 < 7 && proxyBoard.b[i][j-1] < 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i-1][j-1] = R_MINI_NIN;
                        proxyBoard.fCol = j-1;
                        proxyBoard.fRow = i-1;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i][j-1] = attack(attackProxy.b[i][j-1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
                    break;
            }
            }
            else if(team == BLUE){
            switch(myPiece){
                case B_SAM:
                    //Check for forward moves
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    for(m = i-1; proxyBoard.b[m][j] == EMPTY && m >= 0; m--){
                        proxyBoard.b[m+1][j] = EMPTY;
                        proxyBoard.b[m][j] = B_SAM;
                        proxyBoard.fCol = j;
                        proxyBoard.fRow = m;
                        if(proxyBoard.b[m-1][j] > 0 && m-1 >= 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m-1][j] = EMPTY;
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves to the right
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    for(n = j+1; n < 7 && && i-1 >= 0 && proxyBoard.b[i][n] == EMPTY; n++){
                        proxyBoard.b[i][n-1] = EMPTY;
                        proxyBoard.b[i][n] = B_SAM;
                        if(proxyBoard.b[i-1][n] > 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = i;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i-1][n] = attack(attackProxy.b[i-1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves to the left
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    for(n = j-1; n >= 0 && && i-1 >= 0 && proxyBoard.b[i][n] == EMPTY; n--){
                        proxyBoard.b[i][n+1] = EMPTY;
                        proxyBoard.b[i][n] = B_SAM;
                        if(proxyBoard.b[i-1][n] > 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = i;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i-1][n] = attack(attackProxy.b[i-1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                    }
                    proxyBoard = CopyBoard(myBoard);
                    break;
                case B_NIN:
                    //Check for Diagonal Left
                    m = i-1;
                    n = j-1;
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    while(m >= 0 && n >= 0 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m+1][n+1] = EMPTY;
                        proxyBoard.b[m][n] = B_NIN;
                        proxyBoard.fCol = n;
                        proxyBoard.fRow = m;
                        if(proxyBoard.b[m-1][n] > 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m-1][n] = attack(attackProxy.b[m-1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                        m--;
                        n--;
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for Diagonal Right
                    m = i-1;
                    n = j+1;
                    proxyBoard.sCol = j;
                    proxyBoard.sRow = i;
                    while(m >= 0 && n < 7 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m+1][n-1] = EMPTY;
                        proxyBoard.b[m][n] = B_NIN;
                        proxyBoard.fCol = n;
                        proxyBoard.fRow = m;
                        if(proxyBoard.b[m-1][n] > 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m-1][n] = attack(attackProxy.b[m-1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                        m--;
                        n++;
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves backwards left
                    m = i + 1;
                    n = j - 1;
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    while(m < 8 && n >= 0 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m-1][n+1] = EMPTY;
                        proxyBoard.b[m][n] = B_NIN;
                        if(proxyBoard.b[m-1][n] > 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = m;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m-1][n] = attack(attackProxy.b[m-1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        m++;
                        n--;
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for attacking moves backwards right
                    m = i + 1;
                    n = j + 1;
                    proxyBoard.sCol = j;
                    proxyBoard.sRow = i;
                   while(m < 8 && n < 7 && proxyBoard.b[m][n] == EMPTY){
                        proxyBoard.b[m-1][n-1] = EMPTY;
                        proxyBoard.b[m][n] = B_NIN;
                        if(proxyBoard.b[m-1][n] > 0){
                            proxyBoard.fCol = n;
                            proxyBoard.fRow = m;
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[m-1][n] = attack(attackProxy.b[m-1][n]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        m++;
                        n++;
                    }
                     proxyBoard = CopyBoard(myBoard);
                    break;
                case B_MINI_SAM:
                    //Check for forward move
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i-1][j] == EMPTY){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i-1][j] = B_MINI_SAM;
                        proxyBoard.fCol = j;
                        proxyBoard.fRow = i-1;
                        if(proxyBoard.b[i-2][j] > 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i-2][j] = attack(attackProxy.b[i-2][j]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for left attacking move
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i][j-1] == EMPTY && proxyBoard.b[i-1][j-1] > 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i][j-1] = B_MINI_SAM;
                        proxyBoard.fCol = j-1;
                        proxyBoard.fRow = i;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i-1][j-1] = attack(attackProxy.b[i-1][j-1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for right attacking move
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i][j+1] == EMPTY && proxyBoard.b[i-1][j+1] > 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i][j+1] = B_MINI_SAM;
                        proxyBoard.fCol = j+1;
                        proxyBoard.fRow = i;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i-1][j+1] = attack(attackProxy.b[i-1][j+1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
                    break;
                case B_MINI_NIN:
                    //Check for diagonal right
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i-1][j+1] == EMPTY && i-1 >= 0 && j+1 < 7){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i-1][j+1] = B_MINI_NIN;
                        proxyBoard.fCol = j+1;
                        proxyBoard.fRow = i-1;
                        if(proxyBoard.b[i-2][j+1] > 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i-2][j+1] = attack(attackProxy.b[i-2][j+1]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for diagonal left
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i-1][j-1] == EMPTY && i-1 >= 0 && j-1 >= 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i-1][j-1] = B_MINI_NIN;
                        proxyBoard.fCol = j-1;
                        proxyBoard.fRow = i-1;
                        if(proxyBoard.b[i-2][j-1] > 0){
                            attackProxy = CopyBoard(proxyBoard);
                            attackProxy.attacking = true;
                            attackProxy.b[i-2][j-1] = attack(attackProxy.b[i-2][j-1]);
                            legalMoves.push_back(attackProxy);
                            attackProxy = CopyBoard(proxyBoard);
                        }
                        else {
                            proxyBoard.attacking = false;
                            legalMoves.push_back(proxyBoard);
                        }
                    }
                     proxyBoard = CopyBoard(myBoard);
                    //Check for attacking back right
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i+1][j+1] == EMPTY && i+1 < 8 && j+1 < 7 && proxyBoard.b[i][j+1] > 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i+1][j+1] = B_MINI_NIN;
                        proxyBoard.fCol = j+1;
                        proxyBoard.fRow = i+1;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i][j+1] = attack(attackProxy.b[i][j+1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
                    //Check for attacking back left
                    proxyBoard.sRow = i;
                    proxyBoard.sCol = j;
                    if(proxyBoard.b[i+1][j-1] == EMPTY && i+1 < 8 && j+1 < 7 && proxyBoard.b[i][j-1] > 0){
                        proxyBoard.b[i][j] = EMPTY;
                        proxyBoard.b[i+1][j-1] = B_MINI_NIN;
                        proxyBoard.fCol = j-1;
                        proxyBoard.fRow = i+1;
                        attackProxy = CopyBoard(proxyBoard);
                        attackProxy.attacking = true;
                        attackProxy.b[i][j-1] = attack(attackProxy.b[i][j-1]);
                        legalMoves.push_back(attackProxy);
                        attackProxy = CopyBoard(proxyBoard);
                    }
                    proxyBoard = CopyBoard(myBoard);
            }
            }
        }
    //cout << legalMoves.size();
    return legalMoves;
}

inline char numtoboardcol(int i){
    char c = i + '0';
    switch(c){
        case '0': return 'A';
        case '1': return 'B';
        case '2': return 'C';
        case '3': return 'D';
        case '4': return 'E';
        case '5': return 'F';
        case '6': return 'G';
    }
}

inline char numtoboardrow(int i){
    char c = i + '0';
    switch(c){
        case '0': return '8';
        case '1': return '7';
        case '2': return '6';
        case '3': return '5';
        case '4': return '4';
        case '5': return '3';
        case '6': return '2';
        case '7': return '1';
    }
}

inline char numtoinverserow(int i){
    char c = i + '0';
    switch(c){
        case '0': return '1';
        case '1': return '2';
        case '2': return '3';
        case '3': return '4';
        case '4': return '5';
        case '5': return '6';
        case '6': return '7';
        case '7': return '8';
    }
}

inline char numtoinversecol(int i){
    char c = i + '0';
    switch(c){
        case '0': return 'G';
        case '1': return 'F';
        case '2': return 'E';
        case '3': return 'D';
        case '4': return 'C';
        case '5': return 'B';
        case '6': return 'A';
    }
}

void makemove(){ 
    int best = -99999, depth = 0, score, myPiece, alpha = -99999, beta = 99999;
    char m1, m2, m3, m4;
    string s;
    vector<Board> myMoves = movegen(board, RED);
    Board proxyBoard;
    Board optimalBoard;
    
    //Loop through possible moves
    for(int i = 0; i < myMoves.size(); i++){
        proxyBoard = CopyBoard(myMoves.at(i));
        //printboard(proxyBoard);
        score = min(depth+1, proxyBoard, alpha, beta);
        if(score > best){
            optimalBoard = CopyBoard(proxyBoard);
            best = score;
        }
    }
    m1 = numtoboardcol(optimalBoard.sCol);
    m2 = numtoboardrow(optimalBoard.sRow);
    m3 = numtoboardcol(optimalBoard.fCol);
    m4 = numtoboardrow(optimalBoard.fRow);
    
    cout << "My move. " << m1 << m2 << m3 << m4 << endl;
    
    m1 = numtoinversecol(optimalBoard.sCol);
    m2 = numtoinverserow(optimalBoard.sRow);
    m3 = numtoinversecol(optimalBoard.fCol);
    m4 = numtoinverserow(optimalBoard.fRow);
    
    cout << "(" << m1 << m2 << m3 << m4 << ")" << endl;
    
    if(optimalBoard.attacking) cout << "I attack. HIYA!!\n";
    board = CopyBoard(optimalBoard);
    printboard(board);
}

int min(int depth, Board myBoard, int alpha, int beta){ 
    int best = 99999, score;
    Board proxyBoard;
    if(evaluate(myBoard) < -9999) return (-999999);
    if(depth == maxdepth) return (evaluate(myBoard));
    vector<Board> myMoves = movegen(myBoard, BLUE);
    for(int i = 0; i < myMoves.size(); i++){
        proxyBoard = CopyBoard(myMoves.at(i));
        score = max(depth+1, proxyBoard, alpha, beta);
        if(score < best) best = score;
        if(beta < best) beta = best;
        if( beta <= alpha) break;
    }
    return(best);
}

int max(int depth, Board myBoard, int alpha, int beta){
    int best = -99999, score;
    Board proxyBoard;
    if(evaluate(myBoard) > 9499) return (999999);
    if(depth == maxdepth) return (evaluate(myBoard));
    vector<Board> myMoves = movegen(myBoard, RED);
    for(int i = 0; i < myMoves.size(); i++){
        proxyBoard = CopyBoard(myMoves.at(i));
        score = min(depth+1, proxyBoard, alpha, beta);
        if(score > best) best = score;
        if(alpha > best) alpha = best;
        if(alpha >= beta) break;
    }
    return(best);
}

int evaluate(Board myBoard){
    int i, j, score = 0;
    for(i = 0; i < 8; i++)
        for(j = 0; j < 7; j++){
            score += myBoard.b[i][j];
        }
    return score;
}

char getPieceChar(int myPiece){
    int ptype = abs(myPiece);
    switch(ptype){
        case R_KING: return 'K';
        case R_SAM: return 'S';
        case R_NIN: return 'J';
        case R_MINI_SAM: return 's';
        case R_MINI_NIN: return 'j';
        default: return ' ';
    }
}

void printboard(Board b)
{ 
    cout << endl;
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 7; j++){
            //Initial variables for board colors
            int bgColor = BLACK;
            int pieceColor = BLUE;
            
            char myPiece = getPieceChar(b.b[i][j]);
            
            //Board checker colors
            if((i+j)%2 == 0)
                bgColor = BLACK;
            else
                bgColor = WHITE;
            
            if(j == 0){
                textcolor(RESET, YELLOW, BLACK);
                cout << (8 - i);
            }
            
            //Board piece colors
            if(b.b[i][j] > 0)
                pieceColor = RED;
            else
                pieceColor = BLUE;
            
            //Set cout colors
            textcolor(BRIGHT, pieceColor, bgColor);
            
            //Print contents of the board
            cout << " " << myPiece << " ";
            if(j == 6)
                cout << endl;
        }
    textcolor(RESET, YELLOW, BLACK);
    cout << "  " << "A" << "  " << "B" << "  " << "C" << "  " << "D" << "  " << "E" << "  " << "F" << "  " << "G" << " " << endl;
    textcolor(RESET, WHITE, BLACK);
}

void setup()
{ 
    textcolor(RESET, BLACK, WHITE);
    cout << "Here are the 2 Rules of Miyagi-Ryu Karate. Rule Number 1: ‘Karate for defense only.’ Rule Number 2: ‘First learn rule number 1.’" << endl;
    for (int i=0; i<8; i++)
        for (int j=0; j<7; j++) {
        
            //Place Red (AI) Pieces
            if(i == 0 && j == D) board.b[i][j] = R_KING;
            else if(i == 1 && j < D) board.b[i][j] = R_NIN;
            else if(i == 1 && j > D) board.b[i][j] = R_SAM;
            else if(i == 2 && j < D) board.b[i][j] = R_MINI_SAM;
            else if(i == 2 && j > D) board.b[i][j] = R_MINI_NIN;

            //Place Blue (Human) Pieces
            else if(i == 7 && j == D) board.b[i][j] = B_KING;
            else if(i == 6 && j < D) board.b[i][j] = B_SAM;
            else if(i == 6 && j > D) board.b[i][j] = B_NIN;
            else if(i == 5 && j < D) board.b[i][j] = B_MINI_NIN;
            else if(i == 5 && j > D) board.b[i][j] = B_MINI_SAM;

            // Fill empty spaces
            else board.b[i][j] = EMPTY;
        }
}

inline bool isInteger(const std::string & s){
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

inline bool Equivalent(Board b1, Board b2){
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 7; j++){
            if(b1.b[i][j] != b2.b[i][j] && b1.b[i][j] < 0) return false;
        }
    return true;
}

inline bool isLegal(Board myBoard, vector<Board> legalMoves){
    Board legalBoard;
    for(int i = 0; i < legalMoves.size(); i++){
        legalBoard = legalMoves.at(i);
        //printboard(legalBoard);
        if(Equivalent(legalBoard, myBoard)){
            return true;
        }
    }
    return false;
}

void getamove(){
    int i, j, m, n;
    string s = "", s1;
    char c;
    bool done = false;
    Board proxyBoard;
    vector<Board>myMoves = movegen(board, BLUE);
    
    //Loop until we receive valid input
    while(!done){
        cout << "Enter Move: ";
        cin >> s1;
        
        //Parse input by character, push input into new string
        for(int ctr = 0; ctr < s1.size(); ctr++){
            c = s1.at(ctr);
            switch(c){
                case 'A': s.push_back('0'); break;
                case 'B': s.push_back('1'); break;
                case 'C': s.push_back('2'); break;
                case 'D': s.push_back('3'); break;
                case 'E': s.push_back('4'); break;
                case 'F': s.push_back('5'); break;
                case 'G': s.push_back('6'); break;
                case '1': s.push_back('7'); break;
                case '2': s.push_back('6'); break;
                case '3': s.push_back('5'); break;
                case '4': s.push_back('4'); break;
                case '5': s.push_back('3'); break;
                case '6': s.push_back('2'); break;
                case '7': s.push_back('1'); break;
                case '8': s.push_back('0'); break;
            }
        }
        
        //Check if the argument is an integer of the proper size
        if(s.size() == 4 && isInteger(s)){
            j = s.at(0) - '0';
            i = s.at(1) - '0';
            n = s.at(2) - '0';
            m = s.at(3) - '0';
            
            //printf("Arguments: %d %d %d %d", i, j, m, n);
            
            //Make the move on a proxy board
            proxyBoard = CopyBoard(board);
            int myPiece = proxyBoard.b[i][j];
            proxyBoard.b[i][j] = EMPTY;
            proxyBoard.b[m][n] = myPiece;
            if(proxyBoard.b[m-1][n] > 0) proxyBoard.b[m-1][n] = attack(proxyBoard.b[m-1][n]);
            
            //Check if we are moving a player piece and that the move is legal
            if(board.b[i][j] < 0 && isLegal(proxyBoard, myMoves)){
                if(proxyBoard.attacking) cout << "HIYA!!\n";
                board = CopyBoard(proxyBoard);
                done = true;
            }
            else printf("Can't move there!\n");
        }
        else printf("Not valid input.\n");
        s = "";
    }
    printboard(board);
}

void checkGameOver(){
    int winner = 0, myPiece, val = 0;
    bool kingFound = false;
    vector<Board> aiMoves = movegen(board, RED);
    vector<Board> humanMoves = movegen(board, BLUE);
    
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 7; j++){
            myPiece = board.b[i][j];
            if(myPiece == B_KING || myPiece == R_KING){
                kingFound = true;
                val += myPiece;
            }
        }
    if(val > 0) winner = 1;
    if(val < 0) winner = -1;
    if(!kingFound) winner = -2;
    
    
    if(aiMoves.size() == 0 && humanMoves.size() == 0) winner = -2;
    if(aiMoves.size() == 0 && humanMoves.size() != 0) winner = -1;
    if(aiMoves.size() != 0 && humanMoves.size() == 0) winner = 1;
    if (winner == -1) { cout << "Never put passion in front of principle. Even if you win, you will lose." << endl; exit(0); }
    if (winner ==  1) { cout << "It is okay to lose to an opponent. It is never okay to lose to fear."   << endl; exit(0); }
    if (winner ==  -2)    { cout << "Either you karate do \"yes\" or karate do \"no.\" You karate do \"Guess so,\" you squish just like grape." << endl; exit(0); }
}