// Inkluder nødvendige biblioteker
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 8080

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char result[1024];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while(true) {
        printf("Venter på ny forbindelse...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Forbindelse vellykket etablert.\n");

        while(true) {
            memset(buffer, 0, 1024); // Nullstill bufferen for hver nye forespørsel
            valread = read(new_socket, buffer, 1024); // Les data sendt av klienten

            if (strcmp(buffer, "q") == 0 || valread == 0) {
                printf("Forbindelse avsluttet av klienten.\n");
                break; // Avslutt indre løkke hvis klienten sender "quit" eller forbindelsen lukkes
            }

            // Parse input og utfør operasjonen som tidligere
            int tall1, tall2;
            char operasjon;
            sscanf(buffer, "%d %c %d", &tall1, &operasjon, &tall2);
            int resultat;
            if (operasjon == '+') {
                resultat = tall1 + tall2;
            } else if (operasjon == '-') {
                resultat = tall1 - tall2;
            } else {
                snprintf(result, sizeof(result), "Ugyldig operasjon.");
                send(new_socket, result, strlen(result), 0);
                continue; // Fortsett å lytte etter nye forespørsler
            }

            // Send resultatet tilbake til klienten
            snprintf(result, sizeof(result), "Resultat: %d", resultat);
            send(new_socket, result, strlen(result), 0);
        }

        close(new_socket); // Lukk klientforbindelsen etter at løkken er avsluttet
    }
}