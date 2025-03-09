#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "stuff.h"
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
} board;
void mine(board* b, int x, int y);
void chord(board* b, int x, int y);
// This functions allows the player to select out of easy, medium, hard or custom
// If custom the player can select the size and number of mines
// This function also allocates the memory on the heap
void dificulty_select(board* b) {
    printf("Choose the dificulty:\nType e for easy, m for medium, h for hard, c for custom\n");
    char dificulty;
    scanf("%c", &dificulty);
    if (dificulty == 'e') {
        b->width = 8;
        b->height = 8;
        b->no_mines = 10;
    } else if (dificulty == 'm') {
        b->width = 16;
        b->height = 16;
        b->no_mines = 40;
    } else if (dificulty == 'h') {
        b->width = 30;
        b->height = 16;
        b->no_mines = 99;
    } else if (dificulty == 'c') {
        while (1) {
            printf("Width = ");
            scanf("%d", &b->width);
            if (b->width <= 60) {
                break;
            } else {
                char choice = 0, redundancy = 0;
                scanf("%c", &redundancy);
                while (redundancy != '\n') {
                    scanf("%c", &redundancy);
                }
                printf("Warning! For large widths the rows may not appear on a single line.\nAre you sure you want to continue? (Y/n) ");
                scanf("%c", &choice);
                if (choice != 'n' && choice != 'N')
                    break;
            }
        }
        while (1) {
            printf("Height = ");
            scanf("%d", &b->height);
            if (b->height <= 40) {
                break;
            } else {
                char choice = 'n', redundancy = 0;
                scanf("%c", &redundancy);
                while (redundancy != '\n') {
                    scanf("%c", &redundancy);
                }
                printf("Warning! For large heights the columns may not be entirely displayed if CLI doesn't allow scrolling.\nAre you sure you want to continue? (Y/n)");
                scanf("%c", &choice);
                if (choice != 'n' && choice != 'N')
                    break;
            }
        }
        while (1) {
            printf("Number of mines = ");
            scanf("%d", &b->no_mines);
            if (b->no_mines <= b->width * b->height - 1)
                break;
            else 
                printf("Too many mines!!\n");
        }
    }
    b->field = malloc(b->height * sizeof(unsigned char*));
    for (unsigned int i = 0; i <= b->height; i++)
        b->field[i] = calloc(b->width, sizeof(unsigned char));
    b->mask = malloc(b->height * sizeof(unsigned char*));
    for (unsigned int i = 0; i <= b->height; i++)
        b->mask[i] = calloc(b->width, sizeof(unsigned char));
}
// This function generates a playable minesweeper board
void generate_field(board* b) {
    srand(time(NULL));
    unsigned int total_squares = b->height * b->width;
    for (unsigned int i = 0; i < b->no_mines; i++) {
        unsigned int random = rand() % total_squares;
        unsigned int y = random / b->width;
        unsigned int x = random % b->width;
        while (b->field[y][x] == BOMB) {
            random = rand() % total_squares;
            y = random / b->width;
            x = random % b->width;
        }
        b->field[y][x] = BOMB;
    }
    for (int i = 0; i < b->height; i++) {
        for (int j = 0; j < b->width; j++) {
            if (b->field[i][j] != BOMB) {
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        if (i + k >= 0 && i + k < b->height && j + l >= 0 && j + l < b->width) {
                            if (b->field[i + k][j + l] == BOMB)
                                b->field[i][j]++;
                        }
                    }
                }
            }
        }
    }
}
// This function is used to allow the automatic clearing of the squares which next to a 0
void chord0(board* b, int x, int y) {
    int xstart = -1, xfin = 1;
    int ystart = -1, yfin = 1;
    if (x < 1) xstart++;
    if (y < 1) ystart++;
    if (x >= b->height - 1) xfin--;
    if (y >= b->width - 1) yfin--;
    for (int i = xstart; i <= xfin; i ++)
        for (int j = ystart; j <= yfin; j ++)
            if (b->mask[x + i][y + j] == HIDE) {
                mine(b, x + i, y + j);
            }
}
void mine(board* b, int x, int y) {printf("mine %d %d\n", x, y);
    b->mask[x][y] = SHOW;
    if (b->field[x][y] == 0) {
        chord0(b, x, y);
    }
}
// This function records the player action inputs and the return value determines whether 
// the move waas succesfull (VALID), invalid or whether the player lost by playing that move
char action(board* b) {
    char action_type;
    int x, y;
    printf("m for mine, c for chord, f for flag, u for unsure\n");
    scanf("%c", &action_type);
    scanf("%d%d", &x, &y);
    switch (action_type) {
        case 'm':
            if (b->mask[x][y] != HIDE) {
                return INVALID;
            } else {
                mine(b, x, y);
                if (b->field[x][y] == BOMB) {
                    return LOST;
                } else {
                    return VALID;
                }
            }
        case 'c':
            if (b->mask[x][y] != SHOW) {
                return INVALID;
            } else {
                char no_flags = 0;
                int xstart = -1, xfin = 1;
                int ystart = -1, yfin = 1;
                if (x < 1) xstart++;
                if (y < 1) ystart++;
                if (x >= b->height - 1) xfin--;
                if (y >= b->width - 1) yfin--;
                for (int i = xstart; i <= xfin; i ++)
                    for (int j = ystart; j <= yfin; j ++)
                        if (b->mask[x + i][y + j] == FLAG)
                            no_flags++;
                if (no_flags == b->field[x][y]) {
                    for (int i = xstart; i <= xfin; i ++)
                        for (int j = ystart; j <= yfin; j ++)
                            if (b->mask[x + i][y + j] == HIDE || b->mask[x + i][y + j] == UNSURE) {
                                mine(b, x + i, y + j);
                            }
                } else {
                    return INVALID;
                }
                for (int i = xstart; i <= xfin; i ++)
                    for (int j = ystart; j <= yfin; j ++)
                        if (b->field[x + i][y + j] == BOMB && b->mask[x + i][y + j] == SHOW)
                            return LOST;
                return VALID;
            }
        default:
            printf("Invalid");
            return INVALID;
    }
}
void print_board(board* b) {
    for (unsigned int i = 0; i < b->width / 10; i++)
        for (int j = 0; j < 10; j++)
            printf("%d ", i);
    printf("\n");
    for (unsigned int i = 0; i < b->width / 10; i++)
        for (int j = 0; j < 10; j++)
            printf("%d ", j);
    printf("\n");
    for (unsigned int i = 0; i < b->width % 10; i++)
        printf("%d ", i);
    for (unsigned int i = 0; i < b->height; i++) {
        for (unsigned int j = 0; j < b->width; j++) {
            if (b->mask[i][j] == HIDE)
                printf("■ ");
            if (b->mask[i][j] == SHOW)
                if (b->field[i][j] != BOMB)
                    printf("%d ", b->field[i][j]);
                else
                    printf("* ");
            if (b->mask[i][j] == FLAG)
                printf("⚑ ");
            if (b->mask[i][j] == UNSURE)
                printf("? ");
        }
        printf("\n");
    }
}
int main() {
    board* b = malloc(sizeof(board));
    dificulty_select(b); // this is also where the memory is allocated
    generate_field(b);
    printf("%d\n", action(b));
    print_board(b);
    return 0;
}
