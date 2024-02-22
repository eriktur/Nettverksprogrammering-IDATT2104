#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        close(sock);
        return -1;
    }

    while (true) {
        char operation;
        int num1, num2;

        std::cout << "Enter your first number: ";
        std::cin >> num1;
        std::cout << "Enter your second number: ";
        std::cin >> num2;
        std::cout << "Enter the operation (+, -, *, /): ";
        std::cin >> operation;

        std::stringstream ss;
        ss << operation << " " << num1 << " " << num2;
        std::string message = ss.str();

        sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        char buffer[1024] = {0};
        socklen_t serv_addr_size = sizeof(serv_addr);
        int bytes_received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&serv_addr, &serv_addr_size);
        if (bytes_received == -1) {
            std::cerr << "recvfrom failed" << std::endl;
            continue;
        }

        std::cout << "Answer from server: " << buffer << std::endl;

        std::string decision;
        std::cout << "Do you want to continue? (y/n): ";
        std::cin >> decision;
        if (decision != "y" && decision != "Y") {
            break;
        }
    }

    close(sock);
    return 0;
}
