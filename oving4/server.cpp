
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PORT 8080

void* handleClient(void* socket_desc) {
    int sock = *(int*)socket_desc;
    free(socket_desc);
    char buffer[1024] = {0};
    char result[1024];
    int valread;

    while(true) {
        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, sizeof(buffer));
        if (strcmp(buffer, "q") == 0 || valread == 0) {
            printf("Forbindelse avsluttet av klienten.\n");
            break;
        }

        int tall1, tall2;
        char operasjon;
        sscanf(buffer, "%d %c %d", &tall1, &operasjon, &tall2);

        int resultat = (operasjon == '+') ? (tall1 + tall2) : (tall1 - tall2);
        snprintf(result, sizeof(result), "%d", resultat);
        send(sock, result, strlen(result), 0);
    }

    close(sock);
    return nullptr;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    while(true) {
        printf("Venter på ny forbindelse...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }
        printf("Forbindelse etablert.\n");

        int* new_sock = new int(new_socket);

        pthread_t thread;
        if (pthread_create(&thread, nullptr, handleClient, (void*)new_sock) != 0) {
            perror("could not create thread");
        }
        pthread_detach(thread); // Detach tråden slik at ressurser frigjøres når den avsluttes
    }

    close(server_fd);
    return 0;
}