#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    if (argc != 3) {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    int sd = socket (AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (atoi(argv[2]));

    if (connect (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1) {
        perror("Eroare la connect");
        return -1;
    }

    printf("--- Sistem Gestiune Parcare ---\nIntroduceti nr. inmatriculare: ");
    char msg[100];
    bzero(msg, 100);
    fgets(msg, 100, stdin);
    msg[strcspn(msg, "\n")] = 0;

    write(sd, msg, strlen(msg) + 1);

    while (1) {
        bzero(msg, 100);
        int n = read(sd, msg, 100);
        
        if (n <= 0) {
            printf("\n[Bariera s-a ridicat / Conexiune finalizata]\n");
            break;
        }

        printf("\nServer: %s\n", msg);

        if (strstr(msg, "Ati parcat") || strstr(msg, "Multumim") || strstr(msg, "Drum bun")) {
            printf("Programul se inchide automat...\n");
            break; 
        }

        if (strstr(msg, "De plata") || strstr(msg, "Suma insuficienta") || strstr(msg, "Eroare")) {
            printf("\nComanda (ex: /plata 10): ");
            fflush(stdout);
            bzero(msg, 100);
            fgets(msg, 100, stdin);
            msg[strcspn(msg, "\n")] = 0;
            
            if (write(sd, msg, strlen(msg) + 1) <= 0) break;
        } else {
            break;
        }
    }

    close(sd);
    return 0;
}