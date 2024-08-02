/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;

const string STANDARD_CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25] = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();

void fillCubesVector(Vector<string>& cubes, int n);
void shakeCubes(Vector<string>& cubes, int n);

void fillTheBoard(Vector<string>& cubes, Grid<char>& grid, int n);
void humansTurn(Lexicon& lex, Grid<char>& board);
void computersTurn(Lexicon& lex, Grid<char>& board);
void mixCubes(Vector<string>& cubes);


/* Main program */
int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    Lexicon lex("EnglishWords.dat");
    initGBoggle(gw);
    welcome();
    giveInstructions();

    while (true) {
        int n = getInteger("enter board size ( 4 or 5) ");
        drawBoard(n, n);
        Vector<string> cubes;
        string ans = getLine("wanna choose your own cubes? (enter <yes> for yes, else hit return) ");
        if (ans == "yes") {
            fillCubesVector(cubes, n * n);
        }
        else {
            shakeCubes(cubes, n * n);
        }
        Grid<char> board(n, n);
        fillTheBoard(cubes, board, n);
        humansTurn(lex, board);
        computersTurn(lex, board);
        string continuing = getLine("enter <yes> if you want to contunue ");
        if (continuing != "yes") {
            break;
        }
    }
    return 0;
}
struct charPoint
{
    int row;
    int column;
};
bool contains(Vector<charPoint>& route, charPoint point) {
    for (charPoint p : route) {
        if (p.row == point.row && p.column == point.column) {
            return true;
        }
    }
    return false;
}
char neighbour(Grid<char>& board, charPoint lastPoint, int i, Vector<charPoint>& route) {
    int changeRow[8] = { 1, 1, 1, -1, -1, -1, 0, 0 };
    int changeColumn[8] = { 1, 0, -1, 1, 0, -1, 1, -1 };
    int newRow = lastPoint.row + changeRow[i];
    int newColumn = lastPoint.column + changeColumn[i];
    if (newRow >= 0 && newRow <= board.numRows() - 1
        && newColumn >= 0 && newColumn <= board.numCols() - 1) {
        charPoint newPoint;
        newPoint.row = newRow;
        newPoint.column = newColumn;
        if (!contains(route, newPoint)) {
            route.add(newPoint);
            return board[newRow][newColumn];
        }
       
    }
    return 0;
}

void printWords(Lexicon& lex, Grid<char>& board, string s, Set<string>& found, Vector<charPoint>& route) {
    if (s.length() >= 4 && lex.contains(s) && !found.contains(s)) {
        found.add(s);
        recordWordForPlayer(s, COMPUTER);
    }
    if (!lex.containsPrefix(s)) {
        return;
    }
    if (s.length() == 0) {
        for (int r = 0; r < board.numRows(); r++) {
            for (int c = 0; c < board.numCols(); c++) {
                s += board[r][c];
                charPoint p;
                p.row = r;
                p.column = c;
                route.add(p);
                printWords(lex, board, s, found, route);
                s = s.substr(0, s.size() - 1);
                route.remove(route.size() - 1);
            }
        }
    }
    else {
        for (int i = 0; i < 8; i++) {
            char c = neighbour(board, route[route.size() - 1], i, route);
            if (c != 0) {
                s += c;
            }
            else {
                continue;
            }
            printWords(lex, board, s, found, route);
            s = s.substr(0, s.size() - 1);
            route.remove(route.size() - 1);
        }
    }

}
void computersTurn(Lexicon& lex, Grid<char>& board) {
    string s = "";
    Set<string> found = {};
    Vector<charPoint> route = {};
    printWords(lex, board, s, found, route);
}

bool continuesRoute(Grid<char>& board, charPoint lastCharPoint, charPoint newPoint) {
    int r = lastCharPoint.row;
    int c = lastCharPoint.column;
    int changeRow[8] = { 1, 1, 1, -1, -1, -1, 0, 0 };
    int changeColumn[8] = { 1, 0, -1, 1, 0, -1, 1, -1 };
    for (int i = 0; i < 8; i++) {
        int newRow = r + changeRow[i];
        int newColumn = c + changeColumn[i];
        if ((newRow == newPoint.row) && (newColumn == newPoint.column)) {
            return true;
        }
    }
    return false;
}
bool boardContains(Grid<char>& board, string word, Vector<charPoint>& route) {
    if (route.size() == word.size()) {
        return true;
    }
    for (int r = 0; r < board.numRows(); r++) {
        for (int c = 0; c < board.numCols(); c++) {
            char nextChar = word[route.size()];
            charPoint newPoint;
            newPoint.row = r;
            newPoint.column = c;
            if (board[r][c] == nextChar) {
                
                if (route.isEmpty() || (continuesRoute(board, route[route.size() - 1], newPoint) && !contains(route, newPoint))) {

                    route += newPoint;
                    
                    if (boardContains(board, word, route)) {
                        return true;
                    }
                    route.remove(route.size() - 1);
                }
            } 
        }
    }
    return false;
}
void highlightBoard(Grid<char>& board, Vector<charPoint> route, bool change) {
    for (charPoint point : route) {
        highlightCube(point.row, point.column, change);
    }
}
void humansTurn(Lexicon& lex, Grid<char>& board) {
    Set<string> foundWords;
    while (true) {
        string word = getLine("enter the word you found (if there are not any return)");
        if (word == "") break;
        word = toUpperCase(word);

        Vector<charPoint> route;


        if (word.length() >= 4 && lex.contains(word) && !foundWords.contains(word) && boardContains(board, word, route)) {
            foundWords.add(word);
            recordWordForPlayer(word, HUMAN);
         
            highlightBoard(board, route, true);
            pause(800);
            highlightBoard(board, route, false);

        }
        else {
            cout << "this word is not eccapted, try again" << endl;
        }
    }
}
void fillTheBoard(Vector<string>& cubes, Grid<char>& board, int n) {
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            int cubeNumber = r * n + c;
            int randomCharNum = randomInteger(0, 5);
            board[r][c] = cubes[cubeNumber][randomCharNum];
            labelCube(r, c, board[r][c]);
        }
    }
}
void fillCubesVector(Vector<string>& cubes, int n) {
    while (cubes.size() < n) {
        string s = getLine("enter string for cube: ");
        if (s.length() == 6) {
            cubes += s;
        }
    }
}
void shakeCubes(Vector<string>& cubes, int n) {
    for (int i = 0; i < n; i++) {
        if (n == 16) {
            cubes += STANDARD_CUBES[i];
        }
        else if (n == 25) {
            cubes += BIG_BOGGLE_CUBES[i];
        }
        else {
            cout << "wrong num" << endl;
        }
    }
    mixCubes(cubes);
}
void swap(Vector<string>& cubes, int i, int j) {
    string s = cubes[i];
    cubes[i] = cubes[j];
    cubes[j] = s;
}
void mixCubes(Vector <string>& cubes) {
    for (int i = 0; i < cubes.size(); i++) {
        int x = randomInteger(i, cubes.size() - 1);
        swap(cubes, x, i);
    }
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */
void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}

// [TODO: Fill in the rest of the code]