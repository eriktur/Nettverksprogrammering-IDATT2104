#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed \n";
        return -1;
    }

    while (true) {
        std::string input;
        std::cout << "Enter operation or 'q' to exit: ";
        std::getline(std::cin, input);

        if (input == "q") {
            break;
        }

        send(sock, input.c_str(), strlen(input.c_str()), 0);
        valread = read(sock, buffer, 1024);
        std::cout << "Result: " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer)); // Rens bufferen for neste melding
    }

    close(sock);
    return 0;
}
