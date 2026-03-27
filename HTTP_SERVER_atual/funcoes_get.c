#include "funcoes_get.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_mime_type(char* file_ext) {
    if (strcmp(file_ext, "html") == 0) {
        return "text/html";
    }
    if (strcmp(file_ext, "txt") == 0) {
        return "text/plain";
    }
    if (strcmp(file_ext, "csv") == 0) {
        return "text/csv";
    }
    if (strcmp(file_ext, "css") == 0) {
        return "text/css";
    }
    if (strcmp(file_ext, "js") == 0) {
        return "text/javascript";
    }
    if (strcmp(file_ext, "json") == 0) {
        return "application/json";
    }
    return "application/octet-stream";
}

// obtem a extensao do arquivo (tipo)
char* get_file_ext(char* file_path) {
    char* dot = strchr(file_path, '.');

    if (dot == NULL) {
        return "/root";
    }

    return ++dot;
}

// obtem o metodo http da requisicao
// retorna uma string indicando o metodo, read-only;
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

// obtem o caminho nao absoluto do arquivo requisitado por GET
// retorna uma string indicando a url, alocada dinamicamente;
// TEM QUE LEMBRAR DE DAR FREE NESSA STRING DE RETORNO EM ALGUM LUGAR
char* get_file_url(char* request) {

    // avanca ate passar o nome do  metodo http
    int i = 0;
    for (; request[i]!='\0' && request[i]!=' '; i++) {;}
    i++;

    // avanca ate obter o tamanho do file path
    int j = i;
    for (; request[j]!='\0' && request[j]!=' '; j++) {;}

    int path_length = j - i; // tamanho da string path
    char* path = (char*)malloc((path_length+1) * sizeof(char)); // aloca um pedaco de memoria para armazenar o path, de acordo com seu tamanho;
    strncpy(path, &request[i], path_length); // copia o pedaco que representa path para a string de retorno
    path[path_length] = '\0';

    return path;
}