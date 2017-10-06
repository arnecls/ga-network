#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <cstdio>

int main(int, char**) {   	
    if(SDL_Init(0) < 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    if(SDLNet_Init() < 0) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);
    }

    exit(0);
}