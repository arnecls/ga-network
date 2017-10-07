#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <cstdio>
#include <string>

std::string IPToString(const IPaddress& aIp) {
    char out[22];
    auto p = reinterpret_cast<const uint8_t*>(&aIp.host);
    sprintf(out, "%d.%d.%d.%d:%d", p[3], p[2], p[1], p[0], aIp.port);
    return out;
}

bool DoSend(TCPsocket& sock) {
    printf("sending request ...\n");
    
    auto req = "GET /200 HTTP/1.1\r\nHost: httpstat.us\r\n\r\n";
    auto len = strlen(req)+1;

    if (SDLNet_TCP_Send(sock, req, len) < len) {
        printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

void DoReceive(TCPsocket& sock) {
    printf("waiting for response ...\n");
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    SDLNet_TCP_Recv(sock, buffer, sizeof(buffer));
    printf("\n%s\n", buffer);
}

bool DoTCP() {
    IPaddress ip;
    // DNS query
    if (SDLNet_ResolveHost(&ip, "httpstat.us", 80) < 0) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
    }
    printf("httpstat.us resolved to %s\n", IPToString(ip).c_str());

    // Open connection
    auto sock = SDLNet_TCP_Open(&ip);
    if(!sock) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return false;
    }

    // Send request
    if (!DoSend(sock)) {
        return false;
    }    

    // Get response
    DoReceive(sock);

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

    if (!DoTCP()) {
        exit(3);
    }

    SDLNet_Quit();
    SDL_Quit();
}