#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 600;

Uint32 get_32bit_ARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    int value = a;
    value = value << 8 | r;
    value = value << 8 | g;
    value = value << 8 | b;
    return value;
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
    cout << get_32bit_ARGB(0, 0, 0, 255) << endl;
    //pixels[0] = get_32bit_ARGB(255, 255, 0, 0);

    for (size_t x = 0; x < WINDOW_SIZE_X; x++)
    {
        double x_value = ((double) x) / WINDOW_SIZE_X;
        for (size_t y = 0; y < WINDOW_SIZE_Y; y++)
        {
            double y_value = ((double) y) / WINDOW_SIZE_Y;
            Uint32 r = (Uint32) (x_value * 255);
            Uint32 g = (Uint32) (y_value * 255);
            pixels[WINDOW_SIZE_X * y + x] = get_32bit_ARGB(255, r, g, 0);
        }
        
    }
    
    cout << sizeof(Uint32);

    Uint64 frame_count = 0;
    
    while (in_use){
	
        SDL_UpdateTexture(texture, NULL, pixels, WINDOW_SIZE_X * sizeof(Uint32));
        
        SDL_Event event;
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

        SDL_Delay(1000 / 60);
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}