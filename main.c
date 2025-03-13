#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "stuff.h"
// This functions allows the player to select out of easy, medium, hard or custom
// If custom the player can select the size and number of mines
// This function also allocates the memory on the heap
void dificulty_select(board* b) {
    printf("Choose the dificulty:\nType e for easy, m for medium, h for hard, c for custom\n");
    char dificulty = ' ';
    while (dificulty == ' ' || dificulty == '\n')
        scanf("%c", &dificulty);
    switch (dificulty) {
    case 'e': 
        b->width = 8;
        b->height = 8;
        b->no_mines = 10;
        break;
   case 'm': 
        b->width = 16;
        b->height = 16;
        b->no_mines = 40;
        break;
    case 'h': 
        b->width = 30;
        b->height = 16;
        b->no_mines = 99;
        break;
    case 'c': {
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
        break;
    }
    default:
        printf("Invalid input\n");
    }
    b->no_mines_shown = b->no_mines;
    b->field = malloc(b->height * sizeof(unsigned char*));
    for (unsigned int i = 0; i <= b->height; i++)
        b->field[i] = calloc(b->width, sizeof(unsigned char));
    b->mask = malloc(b->height * sizeof(unsigned char*));
    for (unsigned int i = 0; i <= b->height; i++)
        b->mask[i] = calloc(b->width, sizeof(unsigned char));
}
// This function generates a playable minesweeper board
void generate_field(board* b, int xsafe, int ysafe) {
    srand(time(NULL));
    b->field[xsafe][ysafe] = BOMB;
    unsigned int total_squares = b->height * b->width;
    for (unsigned int i = 0; i < b->no_mines; i++) {
        unsigned int random = rand() % total_squares;
        unsigned int x = random / b->width;
        unsigned int y = random % b->width;
        while (b->field[x][y] == BOMB) {
            random = rand() % total_squares;
            x = random / b->width;
            y = random % b->width;
        }
        b->field[x][y] = BOMB;
    }
    b->field[xsafe][ysafe] = 0;
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
void mine(board* b, int x, int y) {
    b->mask[x][y] = SHOW;
    if (b->field[x][y] == 0) {
        chord0(b, x, y);
    }
}
// This function records the player action inputs and the return value determines whether 
// the move waas succesfull (VALID), invalid or whether the player lost by playing that move
char action(board* b) {
    char action_type = ' ';
    int x = 0, y = 0;
    printf("m for mine, c for chord, f for flag, u for unsure\n");
    while (action_type == ' ' || action_type == '\n')
        scanf("%c", &action_type);
    scanf("%d%d", &x, &y);
    if (x < 0 || x >= b->height || y < 0 || y >= b->width) {
        return INVALID;
    }
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
        case 'f':
            if (b->mask[x][y] == HIDE || b->mask[x][y] == UNSURE) {
                b->mask[x][y] = FLAG;
                b->no_mines_shown--;
                return VALID;
            } else if (b->mask[x][y] == FLAG) {
                b->mask[x][y] = HIDE;
                b->no_mines_shown++;
                return VALID;
            } else {
                return INVALID;
            }
        case 'u':
            if (b->mask[x][y] == HIDE || b->mask[x][y] == FLAG) {
                b->mask[x][y] = UNSURE;
                return VALID;
            } else if (b->mask[x][y] == UNSURE) {
                b->mask[x][y] = HIDE;
                return VALID;
            } else {
                return INVALID;
            }
        default:
            printf("Invalid");
            return INVALID;
    }
}
void print_board(board* b) {
    printf("\n   Mines left: %d\n", b->no_mines_shown);
    printf("     ");
    unsigned int i = 0;
    for (; i < b->width / 10; i++) {
        for (int j = 0; j < 10; j++)
            if (i) { 
                printf("%d ", i);
            } else {
                printf("  ");
            }
    }
    for (int j = 0; j < b->width % 10; j++) {
        if (i) {
            printf("%d ", j);
        } else {
            printf("  ");
        }
    }
    printf("\n     ");
    i = 0;
    for (; i < b->width / 10; i++) {
        for (int j = 0; j < 10; j++)
            printf("%d ", j);
    }
    for (int j = 0; j < b->width % 10; j++)
        printf("%d ", j);
    printf("\n   ┌");
    for (int j = 0; j < b->width; j++)
        printf("──");
    printf("─┐\n");
    for (unsigned int i = 0; i < b->height; i++) {
        printf("%2d │ ", i);
        for (unsigned int j = 0; j < b->width; j++) {
            if (b->mask[i][j] == HIDE)
                printf("□ ");
            if (b->mask[i][j] == SHOW)
                if (b->field[i][j] != BOMB)
                    printf("%d ", b->field[i][j]);
                else
                    printf("* ");
            if (b->mask[i][j] == FLAG)
                printf("■ ");//# % @ $ + ? ! & * ~
            if (b->mask[i][j] == UNSURE)
                printf("? ");
        }
        printf("│ %2d\n", i);
    }
    printf("   └");
    for (int j = 0; j < b->width; j++)
        printf("──");
    printf("─┘\n     ");
    i = 0;
    for (; i < b->width / 10; i++) {
        for (int j = 0; j < 10; j++)
            printf("%d ", j);
    }
    for (int j = 0; j < b->width % 10; j++)
        printf("%d ", j);
    printf("\n     ");
    i = 0;
    for (; i < b->width / 10; i++) {
        for (int j = 0; j < 10; j++)
            if (i) { 
                printf("%d ", i);
            } else {
                printf("  ");
            }
    }
    for (int j = 0; j < b->width % 10; j++) {
        if (i) {
            printf("%d ", j);
        } else {
            printf("  ");
        }
    }
    printf("\n");
}
char check_win(board* b) {
    for (unsigned int i = 0; i < b->height; i++) {
        for (unsigned int j = 0; j < b->width; j++) {
            if (b->mask[i][j] == HIDE && b->field[i][j] != BOMB)
                return 0;
        }
    }
    for (unsigned int i = 0; i < b->height; i++) {
        for (unsigned int j = 0; j < b->width; j++) {
            if (b->field[i][j] == BOMB)
                b->mask[i][j] = FLAG;
        }
    }
    b->no_mines_shown = 0;
    printf("\n");
    print_board(b);
    printf(" ^_^  You won!\n");
    return 1;
}
int main() {
    board* b = malloc(sizeof(board));
    dificulty_select(b); // this is also where the memory is allocated
    print_board(b);
    // ensuring that the first tap is not a bomb
    {
    printf("Select first square to mine\n");
    int x = 0, y = 0;
    scanf("%d%d", &x, &y);
    generate_field(b, x, y);
    mine(b, x, y);
    print_board(b);
    if (check_win(b)) {
        return 0;
    }
    }
    while (1) {
        switch (action(b)) {
            case VALID:
                print_board(b);
                if (check_win(b)) {
                    return 0;
                }
                break;
            case INVALID:
                print_board(b);
                printf("Invalid move\n");
                break;
            case LOST:
                for (unsigned int i = 0; i < b->height; i++) {
                    for (unsigned int j = 0; j < b->width; j++) {
                        if (b->field[i][j] == BOMB)
                            b->mask[i][j] = SHOW;
                    }
                }
                print_board(b);
                printf(" O_O  You lost\n");
                return 0;
        }
    }
}
