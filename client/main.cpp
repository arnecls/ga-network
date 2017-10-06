#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <cstdio>

union ReadableIp {
    uint32_t n;
    uint8_t p[4];
};

bool DoTCP() {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, "httpstat.us", 80) < 0) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }

    auto ipr = *reinterpret_cast<ReadableIp*>(&ip.host);

    printf("httpstat.us resolved to %d.%d.%d.%d\n", 
    ipr.p[3],
    ipr.p[2],  
    ipr.p[1],
    ipr.p[0]);

    auto sock = SDLNet_TCP_Open(&ip);
    if(!sock) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return false;
    }

    printf("sending request ...\n");
    
    auto req = "GET /200 HTTP/1.1\r\nHost: httpstat.us\r\n\r\n";
    auto len = strlen(req)+1;
    if (SDLNet_TCP_Send(sock, req, len) < len) {
        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return false;
    }

    printf("waiting for response ...\n");
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    SDLNet_TCP_Recv(sock, buffer, sizeof(buffer));
    printf("\n%s\n", buffer);

    printf("done ...\n");
    SDLNet_TCP_Close(sock);

    return true;
}

int main(int, char**) {   	
    if(SDL_Init(0) < 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    if(SDLNet_Init() < 0) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);
    }

    if (!DoTCP()) {
        exit(3);
    }

    SDLNet_Quit();
    SDL_Quit();
}