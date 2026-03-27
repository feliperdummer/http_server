# http_server

Fontes que me ajudaram no projeto:  
  
Documentação do protocolo HTTP: https://datatracker.ietf.org/doc/html/rfc9112  
Guia HTTP Mozilla: https://developer.mozilla.org/en-US/docs/Web/HTTP  
Github do JeffreyTheCoder: https://github.com/JeffreytheCoder  
Documentação da lib Winsock2, para uso de sockets no windows: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/?utm_source=chatgpt.com  

Servidor HTTP simples que serve como objeto de estudo do protocolo HTTP e como as requisições são feitas e respondidas pelo servidor  

----------------------------------------------------------  

Para rodar:    
    
  2 - Compile o arquivo http_server.c com:  
        gcc http_server_coment.c http_verbs.c funcoes_get.c -lws2_32 -o http_server_comment.exe 
          
  3 - Altere a macro ROOT para o caminho que preferir  
      e a macro PORTA pra que preferir também;  
  
----------------------------------------------------------  
  
