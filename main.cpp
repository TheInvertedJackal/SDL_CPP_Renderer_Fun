#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

using namespace std;

const int WINDOW_SIZE_X = 800, WINDOW_SIZE_Y = 600;

constexpr int DEFAULT_OFFSET_X = WINDOW_SIZE_X / 2, DEFAULT_OFFSET_Y = WINDOW_SIZE_Y / 2;

const int MAX_SPOTLIGHTS = 100;

const double PI = 3.141592;

struct Color {
    int a = 255;
    int r = 255;
    int g = 255;
    int b = 255;
};

struct SpotLight {
    double radius;
    double og_x_offset;
    double og_y_offset;
    double offset_x;
    double offset_y = 0;
    double upper_bounds = -1;
    double lower_bounds = -1;
    double (*x_move)(double og, double speed, double range, int frame_count);
    double (*y_move)(double og, double speed, double range, int frame_count);
    Color sl_color;
};

Uint32 get_32bit_ARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
    int value = a;
    value = value << 8 | r;
    value = value << 8 | g;
    value = value << 8 | b;
    return value;
}

void cap_value_255(int *value){
    if(*value > 255){
        *value = 255;
    }
}

Uint32 add_32Bit_ARGB(Uint32 og_color, unsigned char a_a, unsigned char a_r, unsigned char a_g, unsigned char a_b){
    int a = 0x000000ff & (og_color >> 24);
    int r = 0x000000ff & (og_color >> 16);
    int g = 0x000000ff & (og_color >> 8);
    int b = 0x000000ff & og_color;
    a += a_a;
    r += a_r;
    g += a_g;
    b += a_b;
    cap_value_255(&a);
    cap_value_255(&r);
    cap_value_255(&g);
    cap_value_255(&b);
    return get_32bit_ARGB(a, r, g, b);
}

Uint32 add_32Bit_ARGB(Uint32 og_color, Color other){
    return add_32Bit_ARGB(og_color, other.a, other.r, other.g, other.b);
}

/*
    Movement funcs
*/

double basic_move(double og, double speed, double range, int frame_count){
    return og + range * WINDOW_SIZE_X * sin((frame_count / 60.0) * speed);
}

double move_one(double og, double speed, double range, int frame_count){
    return og + range * WINDOW_SIZE_X * cos((frame_count / 60.0) * speed);
}

int read_in_spotlights(SpotLight spotlights[]) {
    int count = 0;

    double radius;
    double x_offset;
    double y_offset;
    Color this_color;
    string x_move;
    string y_move;
    cin >> radius;
    while (cin && count < MAX_SPOTLIGHTS)
    {
        cin >> x_offset >> y_offset >> this_color.a >> this_color.r >> this_color.g >> this_color.b;
        cin >> x_move >> y_move;
        SpotLight temp;
        temp.radius = radius;
        if(x_offset == -1.0){
            x_offset = DEFAULT_OFFSET_X;
        }
        if(y_offset == -1.0){
            y_offset = DEFAULT_OFFSET_Y;
        }
        temp.og_x_offset = x_offset;
        temp.offset_x = x_offset;
        temp.og_y_offset = y_offset;
        temp.offset_y = y_offset;
        temp.sl_color.a = this_color.a;
        temp.sl_color.r = this_color.r;
        temp.sl_color.g = this_color.g;
        temp.sl_color.b = this_color.b;

        if(x_move == "O"){
            temp.x_move = move_one;
        } else {
            temp.x_move = basic_move;
        }

        if(y_move == "O"){
            temp.y_move = move_one;
        } else {
            temp.y_move = basic_move;
        }

        spotlights[count] = temp;
        ++count;
        cin >> radius;
    }
    return count;
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

    SpotLight spotlights[MAX_SPOTLIGHTS];

    int spotlight_count = read_in_spotlights(spotlights);

    Uint64 frame_count = 0;

    cout << "Spotlight count:" << spotlight_count << endl;

    //cout << "Test:" << add_32Bit_ARGB(0xffff0000, 255, 0, 0x0f, 0) << endl;

    while (in_use){	
        SDL_UpdateTexture(texture, NULL, pixels, WINDOW_SIZE_X * sizeof(Uint32));
        
        SDL_Event event;

        for (int x = 0; x < WINDOW_SIZE_X; x++)
        {
            for (int i = 0; i < spotlight_count; i++)
            {
                spotlights[i].upper_bounds = -1;
                spotlights[i].lower_bounds = -1;
                double x_size = spotlights[i].offset_x - x;
                if(abs(x_size) <= spotlights[i].radius){
                    double offset = sqrt(spotlights[i].radius * spotlights[i].radius - x_size * x_size);
                    spotlights[i].upper_bounds = spotlights[i].offset_y + offset;
                    spotlights[i].lower_bounds = spotlights[i].offset_y - offset;
                } 
            }
            for (int y = 0; y < WINDOW_SIZE_Y; y++)
            {
                Uint32 hold_color = 0;
                for (int i = 0; i < spotlight_count; i++)
                {
                    if(y >= spotlights[i].lower_bounds && y <= spotlights[i].upper_bounds){
                        hold_color = add_32Bit_ARGB(hold_color, spotlights[i].sl_color);
                    }
                }
                pixels[WINDOW_SIZE_X * y + x] = hold_color;
            }
        }

        for (int i = 0; i < spotlight_count; i++)
        {
            spotlights[i].offset_x = spotlights[i].x_move(spotlights[i].og_x_offset, 1, .25, frame_count);
            spotlights[i].offset_y = spotlights[i].y_move(spotlights[i].og_y_offset, 1, .25, frame_count);
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

    //delete[] spotlights;
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}