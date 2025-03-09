#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define BUFFER_SIZE 4096  // Розмір буфера для повідомлень Netlink

void listenUSB() {
    int sock_fd;
    struct sockaddr_nl addr;
    char buffer[BUFFER_SIZE];

    // 1. Створюємо Netlink-сокет
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (sock_fd < 0) {
        std::cerr << "Error: Cannot create Netlink socket!" << std::endl;
        return;
    }

    // 2. Налаштовуємо адресу для прослуховування подій
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = -1;  // Слухаємо всі події

    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: Cannot bind Netlink socket!" << std::endl;
        close(sock_fd);
        return;
    }

    std::cout << "Listening for USB events...\n";

    // 3. Читаємо події від ядра
    while (true) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (len < 0) {
            std::cerr << "Error receiving data!" << std::endl;
            continue;
        }

        buffer[len] = '\0'; // Завершуємо рядок
        std::cout << "USB Event: " << buffer << std::endl;

        // 4. Шукаємо Vendor ID та Product ID (якщо це USB-пристрій)
        if (strstr(buffer, "usb") && strstr(buffer, "add")) {
            std::cout << "New USB device detected!" << std::endl;
        }
    }

    close(sock_fd);
}

int main() {
    listenUSB();
    return 0;
}
