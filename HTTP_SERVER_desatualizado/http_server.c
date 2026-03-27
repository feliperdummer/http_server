#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define PORTA 8080
#define ROOT "C:/server_http_dir/www/html"
#define ROOT_LEN 28

#define BUFSIZE 8192 

void handle_request(SOCKET);

char* http_response_GET(char*);

char* get_mime_type(char*);
char* get_file_ext(char*);
char* get_http_method(char*);
char* get_file_url(char*);

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
    int addr_family = AF_INET;
    int socket_type = SOCK_STREAM;
    int protocol = IPPROTO_TCP;

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

    // coloca o socket em modo passivo para que escute por conexoes de clientes;
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

    char request_buffer[BUFSIZE] = {'\0'};

    if (recv(client, request_buffer, BUFSIZE, 0) == SOCKET_ERROR) {
        int receive_error = WSAGetLastError();

        printf("erro: nao foi possivel receber os dados da requisicao\ncodigo do erro: %d\n", receive_error);

        return;
    }

    char* http_method = get_http_method(request_buffer);

    char* response = NULL;

    if (strcmp(http_method, "GET")==0) {

        char* requested_file_url = get_file_url(request_buffer);

        char full_file_path[512] = {'\0'};
        int intended_copied_char = snprintf(full_file_path, 512, ROOT);

        strcat(&full_file_path[intended_copied_char], requested_file_url);

        response = http_response_GET(full_file_path);

        free(requested_file_url);
    }

    // envia a resposta HTTP de volta
    send(client, response, strlen(response), 0);

    free(response);

    closesocket(client);
}

char* http_response_GET(char* full_file_path) {
    char* response = (char*)malloc(BUFSIZE * sizeof(char));

    char* file_suffix = get_file_ext(full_file_path);

    // caso nao tenha um caminho especificado
    if (strcmp(file_suffix, "/root")==0) {
        FILE* fwelcome = fopen("C:/server_http_dir/www/html/welcome.html", "rb");

        fseek(fwelcome, 0, SEEK_END);
        long fwelcome_size = ftell(fwelcome);
        fseek(fwelcome, 0, SEEK_SET);

        snprintf(response, BUFSIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Content-type: %s\r\n"
                 "Content-length: %ld\r\n"
                 "\r\n",
                 "text/html", fwelcome_size);

        char body[fwelcome_size+1];
        int n = fread(body, sizeof(body[0]), fwelcome_size, fwelcome);
        body[n] = '\0';

        strcat(response, body);

        return response;
    }


    char* mime_type = get_mime_type(file_suffix); 

    FILE* fresponse = fopen(full_file_path, "rb");

    if (fresponse==NULL) {
        return "HTTP/1.1 404 NOT FOUND\r\n"
               "\r\n";
    }

    fseek(fresponse, 0, SEEK_END);
    long fsize = ftell(fresponse);
    fseek(fresponse, 0, SEEK_SET);

    snprintf(response, BUFSIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-type: %s\r\n"
             "Content-length: %ld\r\n"
             "\r\n",
             mime_type, fsize);

    char body[fsize+1];
    int n = fread(body, sizeof(body[0]), fsize, fresponse);
    body[n] = '\0';
    strcat(response, body);

    return response;
}

char* get_mime_type(char* file_suffix) {
    if (strcmp(file_suffix, "html") == 0) {
        return "text/html";
    }
    if (strcmp(file_suffix, "txt") == 0) {
        return "text/plain";
    }
    if (strcmp(file_suffix, "csv") == 0) {
        return "text/csv";
    }
    if (strcmp(file_suffix, "css") == 0) {
        return "text/css";
    }
    if (strcmp(file_suffix, "js") == 0) {
        return "text/javascript";
    }
    if (strcmp(file_suffix, "json") == 0) {
        return "application/json";
    }
    return "application/octet-stream";
}

char* get_file_ext(char* file_path) {
    char* dot = strchr(file_path, '.');

    if (dot == NULL) {
        return "/root";
    }

    return ++dot;
}

char* get_http_method(char* request) {
    if (strstr(request, "GET")!=NULL) {
        return "GET";
    }
    if (strstr(request, "POST")!=NULL) {
        return "POST";
    }
    if (strstr(request, "PUT")!=NULL) {
        return "PUT";
    }
    if (strstr(request, "PATCH")!=NULL) {
        return "PATCH";
    }
    if (strstr(request, "DELETE")!=NULL) {
        return "DELETE";
    }
}

char* get_file_url(char* request) {

    char request_copy[BUFSIZE];
    strcpy(request_copy, request);

    int i = 0;
    for (; request_copy[i]!='\0' && request_copy[i]!=' '; i++) {;}
    i++;

    int j = i;
    for (; request_copy[j]!='\0' && request_copy[j]!=' '; j++) {;}

    int path_length = j - i; 
    char* path = (char*)malloc((path_length+1) * sizeof(char)); 
    strncpy(path, &request_copy[i], path_length);
    path[path_length] = '\0';

    return path;
}
