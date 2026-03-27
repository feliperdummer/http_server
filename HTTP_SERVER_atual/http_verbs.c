#include "http_verbs.h"
#include "funcoes_get.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* http_GET(char* full_file_path) {
    char* response = (char*)malloc(BUFSIZE * sizeof(char));

    char* file_ext = get_file_ext(full_file_path); // obtem a extensao do arquivo a partir da url do mesmo

    // caso esteja buscando pela pagina inicial
    if (strcmp(file_ext, "/root")==0) {
        FILE* f_index = fopen("C:/server_http_dir/www/html/index.html", "r");

        fseek(f_index, 0, SEEK_END);
        long f_index_len = ftell(f_index);
        fseek(f_index, 0, SEEK_SET);

        snprintf(response, BUFSIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Content-type: %s\r\n"
                 "Content-length: %ld\r\n"
                 "\r\n",
                 "text/html", f_index_len);

        char body[f_index_len+1];
        int n = fread(body, sizeof(body[0]), f_index_len, f_index);
        body[n] = '\0';

        strcat(response, body);

        return response;
    }

    // obtem o mime_type pra resposta;
    char* mime_type = get_mime_type(file_ext); 

    // ponteiro para o arquivo, vai ser null caso o arquivo nao exista (i.e: a requisicao quis um arquivo inexistente)
    FILE* fresponse = fopen(full_file_path, "r");

    if (fresponse==NULL) {
        return "HTTP/1.1 404 NOT FOUND\r\n"
               "\r\n";
    }

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

    char body[fsize+1];
    int n = fread(body, sizeof(body[0]), fsize, fresponse);
    body[n] = '\0';
    strcat(response, body);

    return response;
}