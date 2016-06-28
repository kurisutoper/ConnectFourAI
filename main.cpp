#include <iostream>
#include <cstdlib>
#include <curses.h>
#include <unistd.h>
using namespace std;

char board[7][6];
WINDOW *win;

void initBoard(void) {
    for(int i = 0; i < 7; ++i) {
        for(int j = 0; j < 6; ++j) {
            board[i][j] = ' ';
        }
    }
}

void printBoard(void) {
    //vertical bars
    for(int i = 0; i < 6; ++i) {
        for(int j = 0; j < 8; ++j) {
            wmove(win, 2*i+1, 4*j);
            waddch(win, '|');
        }
    }

    //horizontal bars
    for(int j = 1; j < 7; ++j) {
        for(int i = 0; i < 29; ++i) {
            wmove(win, 2*j, i);
            waddch(win, '-');
        }
    }

    //board pieces
    for(int i = 0; i < 7; ++i) {
        for(int j = 0; j < 6; ++j) {
            mvwaddch(win, 1+2*j, 2+4*i, board[i][j]);
        }
    }
}

//columns are numbered 0->5 (see board def)
int dropIntoCol(char player, int col) {
    if(col < 0 || col >= 7) return col;

    int deepest = -1;
    for(int i = 0; i < 6; ++i) {
        if(board[col][i] == ' ') {
            deepest = i;
        }
    }

    if(deepest < 0) {
        return -1;
    }
    else {
        board[col][deepest] = player;
    }

    return 0;
}

// return ' ' for no winner or the winning player symbol
char winner(void) {
    for(int i = 0; i < 7; ++i) {
        for(int j = 0; j < 6; ++j) {
            if(board[i][j] != ' ') {
                //row
                if(i <= 6-4) {
                    bool success = true;
                    for(int a = i; a-i < 4; ++a) {
                        if(board[a][j] != board[i][j]) {
                            success = false;
                        }
                    }
                    if(success) {
                        return board[i][j];
                    }
                }
                //column
                if(j <= 7-4) {
                    bool success = true;
                    for(int a = j; a-j < 4; ++a) {
                        if(board[i][a] != board[i][j]) {
                            success = false;
                        }
                    }
                    if(success) {
                        return board[i][j];
                    }
                }
                //diagonal (down-right)
                if(i <= 6-4 && j <= 7-4) {
                    bool success = true;
                    for(int a = i, b = j; a-i < 4 && b-j < 4; ++a, ++b) {
                        if(board[a][b] != board[i][j]) {
                            success = false;
                        }
                    }
                    if(success) {
                        return board[i][j];
                    }
                }
                //diagonal (down-left)
                if(i >= 3 && j <= 7-4) {
                    bool success = true;
                    for(int a = i, b = j; i-a < 4 && b-j < 4; --a, ++b) {
                        if(board[a][b] != board[i][j]) {
                            success = false;
                        }
                    }
                    if(success) {
                        return board[i][j];
                    }
                }
            }
        }
    }

    return ' ';
}

void init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);

    int h, w;
    getmaxyx(stdscr, h, w);
    win = newwin(h, w, 0, 0);
    
    nodelay(stdscr, TRUE);
}

void runGame(void) {
    char whose_turn = 'X'; //X turn first
 
    int xpos = 2;
    while(true) {
        //input stuff
        int ch = getch();
        switch(ch) {
            case KEY_LEFT:
                if(xpos > 2) {
                    xpos -= 4;
                }
                clear();
            break;
            case KEY_RIGHT:
                if(xpos < 25) {
                    xpos += 4;
                }
                clear();
            break;
            case ' ':
                int res = dropIntoCol(whose_turn, (xpos-2)/4);
                if(res == 0) {
                    whose_turn = (whose_turn == 'X')?'O':'X';
                }
                clear();
            break;
        }

        //output stuff

        printBoard();
        mvwaddch(win, 0, xpos, whose_turn);

        char victory = winner();
        if(victory != ' ') {
            wmove(win, 15, 0);
            wprintw(win, "Congratulations Player ");
            waddch(win, victory);

            wmove(win, 17, 0);
            wprintw(win, "Press space to exit...");
        }

        wrefresh(win);

        //wait a bit
        usleep(10000);
    }
}

int main(void) {
    init();
    initBoard();
    runGame();
    return 0;
}
