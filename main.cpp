#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 600;

const double PI = 3.141592;

Uint32 get_32bit_ARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    int value = a;
    value = value << 8 | r;
    value = value << 8 | g;
    value = value << 8 | b;
    return value;
}

double offset(double x, double mag, double period, double offset){
    double temp = mag * sin(period * x + offset);
    if(temp < 0){
        temp = 0;
    }
    return temp;
}


int main(int argc, char* argv[]){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "error initializing SDL: " << SDL_GetError() << endl;
    }
    SDL_Window* win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
        , WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);
    
    bool in_use = true;

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
 
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    SDL_Texture * texture = SDL_CreateTexture(rend,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    Uint32 * pixels = new Uint32[WINDOW_SIZE_X * WINDOW_SIZE_Y];
    memset(pixels, 255, WINDOW_SIZE_X * WINDOW_SIZE_Y * sizeof(Uint32));

    Uint64 frame_count = 0;
    
    while (in_use){
	
        SDL_UpdateTexture(texture, NULL, pixels, WINDOW_SIZE_X * sizeof(Uint32));
        
        SDL_Event event;

        for (size_t x = 0; x < WINDOW_SIZE_X; x++)
        {
            double x_value = ((double) x) / WINDOW_SIZE_X;
            double r1 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 2), frame_count / 100.0);
            double r2 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 4), frame_count / -50.0);
            double g1 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 1), frame_count / -30.0);
            double g2 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 3), frame_count / 75.0);
            double b1 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 2), frame_count / -50.0);
            double b2 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 4), frame_count / -200.0);
            double b3 = 255 * offset(x, 1, PI / (WINDOW_SIZE_X * 3), frame_count / 50.0);
            for (size_t y = 0; y < WINDOW_SIZE_Y; y++)
            {
                double y_value = ((double) y) / WINDOW_SIZE_Y;
                double tempR = r1 * y_value + r2 * y_value * y_value;
                double tempG = g1 * y_value + g2 + y_value * .2;
                double tempB = b1 * y_value + b2 * y_value * y_value + b3 * .9;
                if(tempR > 255){
                    tempR = 255;
                }
                if(tempG > 255){
                    tempG = 255;
                }
                if(tempB > 255){
                    tempB = 255;
                }
                Uint32 r = (Uint32) tempR;
                Uint32 g = (Uint32) tempG;
                Uint32 b = (Uint32) tempB;
                pixels[WINDOW_SIZE_X * y + x] = get_32bit_ARGB(255, r , g, b);
            }
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                in_use = false;
                break;
            }
        }

        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, texture, NULL, NULL);
        SDL_RenderPresent(rend);
        ++frame_count;
        SDL_Delay(1000 / 60);
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}