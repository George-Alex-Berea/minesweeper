#ifndef STUFF
#define STUFF
#define BOMB 255
#define HIDE 0
#define SHOW 1
#define FLAG 2
#define UNSURE 3
#define VISITED 4
#define VALID 0
#define INVALID 1
#define LOST 2
typedef struct board {
    unsigned char** field;
    unsigned char** mask;
    unsigned int height;
    unsigned int width;
    unsigned int no_mines;
    unsigned int no_mines_shown;
} board;
void dificulty_select(board* b);
void generate_field(board* b, int xsafe, int ysafe);
void chord0(board* b, int x, int y);
void mine(board* b, int x, int y);
char action(board* b);
void print_board(board* b);
char check_win(board* b);
void mine(board* b, int x, int y);
#endif