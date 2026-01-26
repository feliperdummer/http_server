# http_server

Fontes que me ajudaram no projeto:  
  
Documentação do protocolo HTTP: https://datatracker.ietf.org/doc/html/rfc9112  
Guia HTTP Mozilla: https://developer.mozilla.org/en-US/docs/Web/HTTP  
Github do JeffreyTheCoder: https://github.com/JeffreytheCoder  

Servidor HTTP simples que serve como objeto de estudo do protocolo HTTP e como as requisições são feitas e respondidas pelo servidor  

----------------------------------------------------------  

Para instalar e rodar:  
  
  1 - Instale os três arquivos fonte do repositório;  
    
  2 - Compile o arquivo http_server.c com:  
        gcc http_server.c -lws2_32 -o http-server.exe;  
          
  3 - Altere a macro ROOT para o caminho que preferir  
      e a macro PORTA pra que preferir também;  
  
----------------------------------------------------------  
  
