#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define PORTA 8080
#define ROOT "C:/server_http_dir/www"

#define BUFSIZE 2048

void handle_request(SOCKET);
char* get_file_ext(const char*);
char* get_mime_type(char*);
char* get_http_response(FILE*, char*);

int main(void) {

    int ops_result = 0;

    // inicializacao da windows dll
    WSADATA wsa_data = {0};
    ops_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (ops_result != 0) {
        int wsa_startup_error = WSAGetLastError();

        printf("erro: nao foi possivel inicializar a dll\ncodigo do erro: %d\n", wsa_startup_error);

        return 1;
    }

    // criacao o socket do lado do server
    SOCKET server_socket = INVALID_SOCKET;
    int addr_family = AF_INET; // familia do endereco ip usado pelo server
    int socket_type = SOCK_STREAM; // tipo do socket
    int protocol = IPPROTO_TCP; // protocolo

    server_socket = socket(addr_family, socket_type, protocol);
    if (server_socket == INVALID_SOCKET) {
        int server_socket_error = WSAGetLastError();

        printf("erro: nao foi possivel inicializar o socket\ncodigo do erro: %d\n", server_socket_error);

        WSACleanup();

        return 1;
    }
    
    // ligar o socket criado a uma porta e um IP
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(PORTA);

    ops_result = bind(server_socket, (struct sockaddr*)&service, sizeof(service));
    if (ops_result == SOCKET_ERROR) {
        int socket_bind_error = WSAGetLastError();

        printf("erro: falha na funcao bind\ncodigo do erro: %d\n", socket_bind_error);

        closesocket(server_socket);
        WSACleanup();

        return 1;
    }

    // coloca o socket em modo que escute conexoes 
    ops_result = listen(server_socket, SOMAXCONN);
    if (ops_result == SOCKET_ERROR) {
        int listen_error = WSAGetLastError();

        printf("erro: falha na funcao listen\ncodigo do erro: %d\n", listen_error);

        closesocket(server_socket);
        WSACleanup();

        return 1;
    }

    // aceita requisicoes de clientes, um por vez
    while (1) {
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);

        int client = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client == INVALID_SOCKET) {
            int client_accept_error = WSAGetLastError();

            printf("erro: nao foi possivel aceitar a conexao\ncodigo do erro: %d\n", client_accept_error);

            continue;
        }

        // analisa a requisicao detalhadamente
        handle_request(client);
    }

    printf("\nconexao fechada...\n");

    // fecha o socket do servidor;
    closesocket(server_socket);
    WSACleanup();

    return 0;
}

void handle_request(SOCKET client) {
    char buffer[BUFSIZE] = {'\0'};
    char* response = NULL;

    if (recv(client, buffer, BUFSIZE, 0) == SOCKET_ERROR) {
        int receive_error = WSAGetLastError();

        printf("erro: nao foi possivel receber os dados da requisicao\ncodigo do erro: %d\n", receive_error);

        return;
    }

    printf("%s\n", buffer);

    if (strstr(buffer, "GET") != NULL) {
        char file_path[200] = {'\0'};
        int iFile = snprintf(file_path, 200, ROOT); // copia o root pro file_path

        // copia o nome do arquivo pro array file_path
        for (char* s = &buffer[4]; *s != ' ' && *s != '\n' && *s != '\0'; s++) {
            file_path[iFile++] = *s;
        }

        char* f_ext = get_file_ext(file_path); // obtem a extensao do arquivo
        char* mime_type = get_mime_type(f_ext); // obtem o mime_type pra resposta;
        FILE* fresponse = fopen(file_path, "rb"); // ponteiro para o arquivo

        response = get_http_response(fresponse, mime_type);
        
        send(client, response, strlen(response), 0);

        free(response);
    }
    closesocket(client);
}

char* get_http_response(FILE* fresponse, char* mime_type) {
    if (fresponse==NULL) {
        return "HTTP/1.1 404 NOT FOUND\r\n"
               "Content-type: text/plain\r\n"
               "Content-length: 0\r\n"
               "\r\n";
    }

    char* response = (char*)malloc(BUFSIZE * sizeof(char));

    // calculo do tamanho do arquivo
    fseek(fresponse, 0, SEEK_END);
    long fsize = ftell(fresponse);
    fseek(fresponse, 0, SEEK_SET);

    snprintf(response, BUFSIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-type: %s\r\n"
             "Content-length: %ld\r\n"
             "\r\n",
             mime_type, fsize);

    char body[256] = {'\0'};
    int n = fread(body, sizeof(body[0]), 256, fresponse);
    body[n] = '\0';
    strcat(response, body);

    printf("%s\n\n", response);

    return response;

}

char* get_mime_type(char* file_ext) {
    if (strcmp(file_ext, "html") == 0) {
        return "text/html";
    }
    if (strcmp(file_ext, "txt") == 0) {
        return "text/plain";
    }
    return "application/octet-stream";
}

char* get_file_ext(const char* file_name) {
    char* dot = strchr(file_name, '.');

    if (dot == NULL) {
        return NULL;
    }

    return ++dot;

}
