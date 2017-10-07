#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <cstdio>
#include <string>

std::string IPToString(const IPaddress& aIp) {
    char out[22];
    auto a = reinterpret_cast<const uint8_t*>(&aIp.host);
    auto p = reinterpret_cast<const uint8_t*>(&aIp.port);
    auto port = (p[0] << 8) | p[1];
    sprintf(out, "%d.%d.%d.%d:%d", a[3], a[2], a[1], a[0], port);
    return out;
}

bool DoListen() {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, 9000) < 0) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }
    printf("listening on %s\n", IPToString(ip).c_str());
    
    // Open connection
    auto sock = SDLNet_TCP_Open(&ip);
    if(!sock) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return false;
    }
    printf("Waiting for connections...\n");

    for (;;) {
        auto client = SDLNet_TCP_Accept(sock);
        if (client == nullptr) {
            continue;
        }

        printf("client connected\n");
        
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        auto len = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
        printf("got %s\n", buffer);

        if (strcmp(buffer,"quit") == 0) {
            SDLNet_TCP_Close(client);
            break;
        }

        SDLNet_TCP_Send(client, buffer, len);
        SDLNet_TCP_Close(client);
    }

    // Close connection
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

    if (!DoListen()) {
        exit(3);
    }

    SDLNet_Quit();
    SDL_Quit();

	return 0;
}