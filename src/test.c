#include <stdio.h>
#include <math.h>

#define PI 3.14159265f

int main() {
    int room[8][8] = {1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1};
    float cosine = cos(90.0 * PI / 180.0);
    float sine = sin(90.0 * PI / 180.0);

    int old_tiles[8][8];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", room[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            old_tiles[i][j] = room[i][j];
            room[i][j] = 0;
        }
    }

    for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("%d ", old_tiles[i][j]);
            }
            printf("\n");
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", room[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int new_row = floor(((float)j - 4) * sine + ((float)i - 4) * cosine + 4);
            int new_col = floor(((float)j - 4) * cosine - ((float)i - 4) * sine + 4);
            room[new_row][new_col] = old_tiles[i][j];
        }
    }
    for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("%d ", room[i][j]);
            }
            printf("\n");
    }
}
