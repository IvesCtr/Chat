#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT 5000 // Porta desejada

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha ao inicializar o Winsock.\n");
        return 1;
    }

    // Criar um socket (familia de protocolos, tipo, protocolo)
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        printf("Erro ao criar o socket.\n");
        WSACleanup();
        return 1;
    }

    // Configurar a estrutura de endereço (cria o struct para usar no bind)
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; //define qualquer endereço que seja especificado pela rede
    serverAddress.sin_port = htons(PORT);

    // Vincular o socket a um endereço IP e porta (socket a ser vinculado, ponteiro para o struct, tamanho em bytes do segundo argumento)
    if (bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("\nFalha ao vincular o socket.\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Escutar por conexões -> o listen permite que o socket receba conexões (servidor, limite max de conexões simultâneas)
    if (listen(listenSocket, 3) == SOCKET_ERROR) {
        printf("\nErro ao escutar por conexoes.\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("\n>> Servidor iniciado.\nAguardando que um ou mais clientes se conectem.");
    printf("\nPara encerrar o servidor, digite '/encerrar'.\n\n");

    int enc = 0;

    while (1) {
        // Aceitar conexões (accept)
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("\nFalha ao aceitar conexao.\n");
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf(">> Um cliente foi conectado ao servidor.\n\n");

        // Loop para enviar/receber mensagens
        while (1) {

            // Receber mensagem do cliente (recv)
            char buffer[1024]; //o buffer serve para guardar as informações
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);  // o 0 diz respeito à flags que irão alterar como o recv recebe os dados
            if (bytesRead == SOCKET_ERROR) {
                printf("\nErro ao receber dados.\n");
                closesocket(clientSocket);
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }
            buffer[bytesRead] = '\0';
            printf("> Cliente: %s\n", buffer);

            // Verificar se o cliente deseja encerrar a conexão
            if (strcmp(buffer, "/sair") == 0) {
                printf("\nO cliente saiu do bate-papo.\n");
                break;
            }

            // Ler mensagem para ser enviada
            char message[1024];
            printf("> Servidor: ");
            fgets(message, sizeof(message), stdin);
            printf("\n");

            // Remover o caractere de nova linha da string
            size_t len = strlen(message);
            if (len > 0 && message[len - 1] == '\n') {
                message[len - 1] = '\0';
            }

            // Enviar resposta para o cliente -> send envia dados pelo socket
            if (send(clientSocket, message, strlen(message), 0) == SOCKET_ERROR) {
                printf("\nHouve um erro no envio de dados.\n");
                closesocket(clientSocket);
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }

            if(strcmp(message, "/encerrar") == 0){
                printf("\n>> Servidor fechado.\n");
                enc = 1;
                break;
            }
        }

        // Encerrar conexão com o cliente atual
        closesocket(clientSocket);
        if(enc == 1) break;
        printf(">> Conexao com o cliente encerrada.\n\n");     
    }

    // Encerrar conexão principal
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
