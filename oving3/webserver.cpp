#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>

#define PORT 3000

int main() {
    system("open http://localhost:3000");

    int server_fd, client_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1; // Definer en variabel for å holde verdien 1
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    client_socket = accept(server_fd, NULL, NULL);

    read(client_socket, buffer, 1024);

    std::string response = "HTTP/1.0 200 OK\nContent-Type: text/html; charset=utf-8\n\n";
    response += "<HTML><BODY>";
    response += "<H1>Hilsen. Du har koblet deg opp til min enkle web-tjener</H1>";
    response += "Header fra klient er:<UL>";

    std::istringstream requestHeaders(buffer);
    std::string line;
    while (std::getline(requestHeaders, line) && line != "\r") {
        response += "<LI>" + line + "</LI>";
    }
    response += "</UL></BODY></HTML>";

    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
    close(server_fd);

    return 0;
}