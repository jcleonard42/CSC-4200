/*
Name: Kevin Pathana and Jonathan Leonard
Class: CSC-4200
Assignment: PA3
Instructor: Susmit Shannigrahi
*/

#if defined(_WIN32) || defined(_WIN64)
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <time.h>

    #pragma comment(lib, "ws2_32.lib")

    #define PACKET_SIZE 12
    #define VERSION 17
    #define HELLO 1
    #define LIGHTON 2

    void send_success(SOCKET client_sock) {
        uint32_t version = htonl(VERSION);
        uint32_t message_type = htonl(HELLO);
        uint32_t message_length = htonl(7);
        char message[] = "SUCCESS";

        send(client_sock, (const char*)&version, sizeof(version), 0);
        send(client_sock, (const char*)&message_type, sizeof(message_type), 0);
        send(client_sock, (const char*)&message_length, sizeof(message_length), 0);
        send(client_sock, message, sizeof(message), 0);

        printf("Sent SUCCESS response\n");
    }

    char* get_timestamp() {
        time_t rawtime;
        struct tm* timeinfo;
        static char timestamp[25];

    #if defined(_WIN32) || defined(_WIN64)
        time(&rawtime);
        timeinfo = localtime(&rawtime);
    #else
        time(&rawtime);
        timeinfo = localtime(&rawtime);
    #endif

        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

        return timestamp;
    }

    void handle_client(SOCKET client_sock, struct sockaddr_in client_addr, FILE* log) {
        char client_ip[INET_ADDRSTRLEN];
    #if defined(_WIN32) || defined(_WIN64)
        const char* ip = inet_ntoa(client_addr.sin_addr);
        strcpy(client_ip, ip);
    #else
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    #endif
        printf("Received connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        fprintf(log, "%s Received connection from %s:%d\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));

        while (1) {
            uint32_t version, message_type, message_length;
            char message[8];

            if (recv(client_sock, (char*)&version, sizeof(version), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }
            if (recv(client_sock, (char*)&message_type, sizeof(message_type), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }
            if (recv(client_sock, (char*)&message_length, sizeof(message_length), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }
            if (recv(client_sock, message, sizeof(message), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }

            version = ntohl(version);
            message_type = ntohl(message_type);
            message_length = ntohl(message_length);

            printf("Received Data: version: %d type: %d length: %d\n", version, message_type, message_length);
            fprintf(log, "%s Received Data: version: %d type: %d length: %d\n", get_timestamp(), version, message_type, message_length);

            if (version != VERSION) {
                printf("VERSION MISMATCH\n");
                fprintf(log, "%s VERSION MISMATCH\n", get_timestamp());
            }

            if (message_type == HELLO) {
                send_success(client_sock);
                printf("VERSION ACCEPTED\n");
                fprintf(log, "%s VERSION ACCEPTED\n", get_timestamp());
            }
            else if (message_type == LIGHTON) {
                send_success(client_sock);
                printf("EXECUTING SUPPORTED COMMAND: LIGHTON\n");
                fprintf(log, "%s EXECUTING SUPPORTED COMMAND: LIGHTON\n", get_timestamp());
            }
            else {
                printf("IGNORING UNKNOWN COMMAND: %d\n", message_type);
                fprintf(log, "%s IGNORING UNKNOWN COMMAND: %d\n", get_timestamp(), message_type);
            }

            fflush(log);  // Flush the log file to ensure it's updated
        }

        closesocket(client_sock);
    }

    int main(int argc, char* argv[]) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed\n");
            return 1;
        }

        if (argc != 3) {
            fprintf(stderr, "Usage: %s <port> <log_file>\n", argv[0]);
            WSACleanup();
            return 1;
        }

        SOCKET server_sock;
        int port = atoi(argv[1]);
        char* log_file = argv[2];
        FILE* log = fopen(log_file, "a");

        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock == INVALID_SOCKET) {
            fprintf(stderr, "Socket creation failed\n");
            WSACleanup();
            return 1;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            fprintf(stderr, "Binding failed\n");
            closesocket(server_sock);
            WSACleanup();
            return 1;
        }

        if (listen(server_sock, 5) == SOCKET_ERROR) {
            fprintf(stderr, "Listening failed\n");
            closesocket(server_sock);
            WSACleanup();
            return 1;
        }

        if (!log) {
            fprintf(stderr, "Log file opening failed\n");
            closesocket(server_sock);
            WSACleanup();
            return 1;
        }

        fprintf(log, "%s Server started on port %d\n", get_timestamp(), port);

        while (1) {
            struct sockaddr_in client_addr;
            int client_addr_len = sizeof(client_addr);
            SOCKET client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_sock == INVALID_SOCKET) {
                fprintf(stderr, "Client connection failed\n");
                continue;
            }

            handle_client(client_sock, client_addr, log);
        }

        closesocket(server_sock);
        fclose(log);  // Close the log file
        WSACleanup();

        return 0;
    }
#else
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <time.h>
    #include <unistd.h>

    #define PACKET_SIZE 12
    #define VERSION 17
    #define HELLO 1
    #define LIGHTON 2

    void send_success(int client_sock) {
        uint32_t version = htonl(VERSION);
        uint32_t message_type = htonl(HELLO);
        uint32_t message_length = htonl(7);
        char message[] = "SUCCESS";

        send(client_sock, (const char*)&version, sizeof(version), 0);
        send(client_sock, (const char*)&message_type, sizeof(message_type), 0);
        send(client_sock, (const char*)&message_length, sizeof(message_length), 0);
        send(client_sock, message, sizeof(message), 0);

        printf("Sent SUCCESS response\n");
    }

    char* get_timestamp() {
        time_t rawtime;
        struct tm* timeinfo;
        static char timestamp[25];

    #if defined(_WIN32) || defined(_WIN64)
        time(&rawtime);
        timeinfo = localtime(&rawtime);
    #else
        time(&rawtime);
        timeinfo = localtime(&rawtime);
    #endif

        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

        return timestamp;
    }

    void handle_client(int client_sock, struct sockaddr_in client_addr, FILE* log) {
        char client_ip[INET_ADDRSTRLEN];
    #if defined(_WIN32) || defined(_WIN64)
        const char* ip = inet_ntoa(client_addr.sin_addr);
        strcpy(client_ip, ip);
    #else
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    #endif

        printf("Received connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        fprintf(log, "%s Received connection from %s:%d\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));

        while (1) {
            uint32_t version, message_type, message_length;
            char message[8];

            if (recv(client_sock, (char*)&version, sizeof(version), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }
            if (recv(client_sock, (char*)&message_type, sizeof(message_type), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }
            if (recv(client_sock, (char*)&message_length, sizeof(message_length), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }
            if (recv(client_sock, message, sizeof(message), 0) == 0) {
                printf("Connection from %s:%d is closed\n", client_ip, ntohs(client_addr.sin_port));
                fprintf(log, "%s Connection from %s:%d is closed\n", get_timestamp(), client_ip, ntohs(client_addr.sin_port));
                break;
            }

            version = ntohl(version);
            message_type = ntohl(message_type);
            message_length = ntohl(message_length);

            printf("Received Data: version: %d type: %d length: %d\n", version, message_type, message_length);
            fprintf(log, "%s Received Data: version: %d type: %d length: %d\n", get_timestamp(), version, message_type, message_length);

            if (version != VERSION) {
                printf("VERSION MISMATCH\n");
                fprintf(log, "%s VERSION MISMATCH\n", get_timestamp());
            }

            if (message_type == HELLO) {
                send_success(client_sock);
                printf("VERSION ACCEPTED\n");
                fprintf(log, "%s VERSION ACCEPTED\n", get_timestamp());
            }
            else if (message_type == LIGHTON) {
                send_success(client_sock);
                printf("EXECUTING SUPPORTED COMMAND: LIGHTON\n");
                fprintf(log, "%s EXECUTING SUPPORTED COMMAND: LIGHTON\n", get_timestamp());
            }
            else {
                printf("IGNORING UNKNOWN COMMAND: %d\n", message_type);
                fprintf(log, "%s IGNORING UNKNOWN COMMAND: %d\n", get_timestamp(), message_type);
            }

            fflush(log);  // Flush the log file to ensure it's updated
        }

        close(client_sock);
    }

    int main(int argc, char* argv[]) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <port> <log_file>\n", argv[0]);
            return 1;
        }

        int server_sock;
        int port = atoi(argv[1]);
        char* log_file = argv[2];
        FILE* log = fopen(log_file, "a");

        if (!log) {
            fprintf(stderr, "Log file opening failed\n");
            return 1;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock < 0) {
            perror("Socket creation failed");
            fclose(log);
            return 1;
        }

        if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Binding failed");
            fclose(log);
            return 1;
        }

        if (listen(server_sock, 5) < 0) {
            perror("Listening failed");
            fclose(log);
            return 1;
        }

        fprintf(log, "%s Server started on port %d\n", get_timestamp(), port);

        while (1) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_sock < 0) {
                perror("Client connection failed");
                continue;
            }

            handle_client(client_sock, client_addr, log);
        }

        close(server_sock);
        fclose(log);  // Close the log file

        return 0;
    }
#endif