#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>

#define WIDTH 800
#define HEIGHT 600
#define SEEDS_COUNT 10
#define COLOR_RED 0xFF0000FF
#define Color32 uint32_t
#define OUTPUT_FILE_PATH "output.ppm"
#define BACKGROUND_COLOR 0xFF181818
#define SEED_MARKER_RADIUS 5
#define BLACK 0xFF000000

#define SEED_MARKER_COLOR BLACK   
static Color32 image[HEIGHT][WIDTH];


#define BRIGHT_RED 0xFF3449FB
#define BRIGHT_GREEN 0xFF34FB49
#define BRIGHT_BLUE 0xFFFB3449
#define BRIGHT_YELLOW 0xFF49FB34
#define BRIGHT_MAGENTA 0xFFFB34FB
#define BRIGHT_CYAN 0xFF34FBFB
#define BRIGHT_WHITE 0xFFFFFFFF
#define BRIGHT_ORANGE 0xFFFB7A34
#define BRIGHT_PURPLE 0xFF7A34FB
#define BRIGHT_LIME 0xFF7AFB34


static Color32 pallate[] = {
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_BLUE,
    BRIGHT_YELLOW,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE,
    BRIGHT_ORANGE,
    BRIGHT_PURPLE,
    BRIGHT_LIME
};

#define Pallet_Count (sizeof(pallate)/sizeof(pallate[0]))

struct Points
{
    int x;
    int y;
};

static Points seeds[SEEDS_COUNT];


void fill_image(Color32 color){
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){
            image[i][j] = color;
        }
    }
}


void fill_seeds(){
    for(size_t i = 0; i < SEEDS_COUNT; i++){
        seeds[i].x = rand() % WIDTH;
        seeds[i].y = rand() % HEIGHT;
    }
}


void fill_circle(int x, int y, int radius, uint32_t color){
    int x0 = x - radius;
    int y0 = y - radius;
    int x1 = x + radius;
    int y1 = y + radius;
    for (int i = x0; i <= x1; ++i){
        if (0 <= i && i < WIDTH) {
            for (int j = y0; j <= y1; ++j){
                if (0 <= j && j < HEIGHT) {
                    int dx = x - i;
                    int dy = y - j;
                    if (dx*dx + dy*dy <= radius*radius){
                        image[j][i] = color;
                    }
                }
            }
        }
    }
}   

void save_image_as_ppm(std::string filePath){
    FILE *fp = fopen(filePath.c_str(), "wb");
    fprintf(fp, "P6\n%d %d 255\n", WIDTH, HEIGHT);
  
    for (int i = 0; i < HEIGHT; i++){
        for (int j = 0; j < WIDTH; j++){
            uint32_t pixel = image[i][j];
            uint8_t bytes[3] = {
                (pixel&0x0000FF) >> 8 * 0,
                (pixel&0x00FF00) >> 8 * 1,
                (pixel&0xFF0000) >> 8 * 2
            };
            fwrite(bytes, sizeof(bytes), 1, fp);
        }
    }
    fclose(fp);
}


int sqr_distance(int x1, int y1, int x2, int y2){
    int d1 = x2 - x1;
    int d2 = y2 - y1;
    return d1*d1 + d2*d2;
}

int manhattan_distance(int x1, int y1, int x2, int y2){
    return abs(x2 - x1) + abs(y2 - y1);
}


void render_voranoi_manhattan(){
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int closest_seed_index = 0;
            int min_distance = manhattan_distance(seeds[0].x, seeds[0].y, j, i);

            for (size_t k = 1; k < SEEDS_COUNT; k++) {
                int current_distance = manhattan_distance(seeds[k].x, seeds[k].y, j, i);
                if (current_distance < min_distance) {
                    closest_seed_index = k;
                    min_distance = current_distance;
                }
            }

            image[i][j] = pallate[closest_seed_index % Pallet_Count];
        }
    }
}

void render_voranoi(){
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int closest_seed_index = 0;
            int min_distance = sqr_distance(seeds[0].x, seeds[0].y, j, i);

            for (size_t k = 1; k < SEEDS_COUNT; k++) {
                int current_distance = sqr_distance(seeds[k].x, seeds[k].y, j, i);
                if (current_distance < min_distance) {
                    closest_seed_index = k;
                    min_distance = current_distance;
                }
            }

            image[i][j] = pallate[closest_seed_index % Pallet_Count];
        }
    }
}

void ppm_to_png(){
    system("ffmpeg -i output.ppm output.png");

}

int main(){
    srand(time(NULL));
    fill_image(BACKGROUND_COLOR);
    fill_seeds();
    render_voranoi();
    for(size_t i = 0; i < SEEDS_COUNT; i++){
        fill_circle(seeds[i].x, seeds[i].y, SEED_MARKER_RADIUS, SEED_MARKER_COLOR);
    }
    save_image_as_ppm(OUTPUT_FILE_PATH); 
    ppm_to_png();
    return 0;
}