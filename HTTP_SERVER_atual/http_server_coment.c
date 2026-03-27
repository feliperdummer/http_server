#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "funcoes_get.h"
#include "http_verbs.h"

#define PORTA 8080
#define ROOT "C:/server_http_dir/www/html" // caminho root para a pasta que contém os arquivos do servidor

#ifndef BUFSIZE_INCLUDE
#define BUFSIZE 8192 
#endif

void handle_request(SOCKET);

// ip que vai ser usado: 127.0.0.1 pra teste

// SOCKET = macro pra um valor unsigned int
// INVALID_SOCKET = macro pra um valor negativo 

int main(void) {

    int ops_result = 0; // armazena a flag de cada operação feita com sockets, valor inteiro;

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
    int addr_family = AF_INET; // familia do endereco ip usado pelo server, AF_INET significa ipv4;
    int socket_type = SOCK_STREAM; // tipo do socket, fornece conexao sequenciada e stream de bytes baseado em conexao;
    int protocol = IPPROTO_TCP; // protocolo que sera utilizado, IPPROTO_TCP so pode ser usad com ipv4 ou ipv6 com tipo de socket SOCK_STREAM;

    server_socket = socket(addr_family, socket_type, protocol); // funcao retorna um file descriptor que referencia o novo socket do server;
    if (server_socket == INVALID_SOCKET) {
        int server_socket_error = WSAGetLastError();

        printf("erro: nao foi possivel inicializar o socket\ncodigo do erro: %d\n", server_socket_error);

        WSACleanup();

        return 1;
    }
    
    /* // ligar o socket criado a uma porta e um IP

        Da documentacao da winsock2:

            "Quando um socket e criado usando a funcao socket, ele existe em um namespace, mas nao contem nenhum name atribuido.
            A funcao bind serve para estabelecer a associacao local do socket atribuindo um local name a um unnamed socket"

        Quando a Internet address family for usada, o name contem tres partes:

            - A familia do endereco
            - Um endereco IP do host
            - Uma porta que indentifica a aplicacao

        Podemos usar getsockname() DEPOIS de chamar bind para saber o endereco e a porta que foram atribuidos
        ao socket;
    
    */
    struct sockaddr_in service; 
    service.sin_family = AF_INET; // define a familia do ip como IpV4;
    service.sin_addr.s_addr = inet_addr("127.0.0.1"); // converte a string contendo o ip pra um ip valido da struct sin_addr;
    service.sin_port = htons(PORTA); // como o host (cpu) usa little-endian pra armazenar valor em bytes na memoria,
                                     // é necessário converter o numero da porta para big-endian, porque protocolos
                                     // de internet usam esse modelo;
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

        closesocket(client);
    }

    printf("\nconexao fechada...\n");

    closesocket(server_socket); // fecha o socket do servidor;
    WSACleanup();

    return 0;
}

void handle_request(SOCKET client) {
    // buffer que vai receber a requisicao HTTP
    char request_buffer[BUFSIZE] = {'\0'};

    // grava a mensagem HTTP no array request_buffer;
    if (recv(client, request_buffer, BUFSIZE, 0) == SOCKET_ERROR) {
        int receive_error = WSAGetLastError();

        printf("erro: nao foi possivel receber os dados da requisicao\ncodigo do erro: %d\n", receive_error);

        return;
    }

    // obtem o metodo http da requisicao
    char* http_method = get_http_method(request_buffer);

    char* response = NULL;

    // testa qual foi o tipo de requisicao feita pelo cliene
    if (strcmp(http_method, "GET")==0) {
        // obtem a url do arquivo requisitado
        char* requested_file_url = get_file_url(request_buffer);

        char full_file_path[128] = {'\0'}; // array que vai armazenar o caminho completo para o arquivo requisitado
        int intended_copied_char = snprintf(full_file_path, 128, ROOT); // copia o root pro full_file_path

        // copia a url do arquivo para o array full_file_path
        strcat(&full_file_path[intended_copied_char], requested_file_url);

        // atribui uma resposta HTTP a response
        response = http_GET(full_file_path);

        free(requested_file_url);
    }

    // envia a resposta HTTP de volta
    send(client, response, strlen(response), 0);

    // libera o espaco que response apontava
    free(response);    
}
