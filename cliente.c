#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT 5000 // Porta desejada
#define SERVER_IP "26.51.80.23" // IP do servidor

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha ao inicializar o Winsock.\n");
        return 1;
    }

    // Criar um socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("\nErro ao criar o socket.\n");
        WSACleanup();
        return 1;
    }

    // Configurar o endereço do servidor
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Conectar ao servidor
    if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("\nHouve uma falha na conexao com o servidor.\nVerifique o IP do Servidor e tente novamente.\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("\n>> Voce se conectou ao Servidor.\nPara sair do bate-papo, digite '/sair'.\n\n");

    while (1) {
        // Ler mensagem do usuário
        char message[1024];
        printf("> Cliente: ");
        fgets(message, sizeof(message), stdin);

        // Remover o caractere de nova linha da string
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }

        // Enviar mensagem para o servidor
        if (send(sockfd, message, strlen(message), 0) == SOCKET_ERROR) {
            printf("\nHouve um erro no envio de dados.\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        // Verificar se o cliente deseja encerrar a conexão
        if (strcmp(message, "/sair") == 0) {
            printf("\nVoce saiu do bate-papo.\n");
            break;
        }

        // Receber resposta do servidor
        char buffer[1024];
        int bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == SOCKET_ERROR) {
            printf("\nErro ao receber dados.\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }
        buffer[bytesRead] = '\0';
        printf("> Servidor: %s\n\n", buffer);

        if(strcmp(buffer, "/encerrar") == 0){
            printf("\n>> O servidor foi fechado.\n");
            break;
        }
    }

    // Encerrar a conexão e limpar a biblioteca Winsock
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
