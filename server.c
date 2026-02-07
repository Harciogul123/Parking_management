#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#define PORT 2728
#define MAX_LOCURI 100
#define MAX_FDS 1024

time_t timp_intrare[MAX_LOCURI + 1];
int locuri_ocupate[MAX_LOCURI + 1]; 
char nr_inmatriculare[MAX_LOCURI + 1][15];
int nr_locuri_libere = MAX_LOCURI;

int datorie_client[MAX_FDS];
int loc_in_curs_de_plata[MAX_FDS];

void initializare() {
    srand(time(NULL));
    for (int i = 1; i <= MAX_LOCURI; i++) {
        locuri_ocupate[i] = 0;
        bzero(nr_inmatriculare[i], 15);
    }
    for (int i = 0; i < MAX_FDS; i++) {
        datorie_client[i] = 0;
        loc_in_curs_de_plata[i] = -1;
    }
}

int handle_client(int fd) {
    char buffer[100];
    char raspuns[150];
    int bytes;

    bzero(buffer, sizeof(buffer));
    if ((bytes = read(fd, buffer, sizeof(buffer) - 1)) <= 0) return 0;
    buffer[strcspn(buffer, "\n\r")] = 0;

    if (datorie_client[fd] > 0) {
        if (strncmp(buffer, "/plata ", 7) == 0) {
            int suma_data = atoi(buffer + 7);
            int loc = loc_in_curs_de_plata[fd];

            if (suma_data >= datorie_client[fd]) {
                int rest = suma_data - datorie_client[fd];
                if (rest > 0) 
                    sprintf(raspuns, "Multumim! Rest: %d lei. Locul %d s-a eliberat. Drum bun!", rest, loc);
                else 
                    sprintf(raspuns, "Multumim! Plata efectuata. Locul %d s-a eliberat. Drum bun!", loc);
                
                locuri_ocupate[loc] = 0;
                bzero(nr_inmatriculare[loc], 15);
                nr_locuri_libere++;
                
                write(fd, raspuns, strlen(raspuns) + 1);
                return 0;
            } else {
                datorie_client[fd] -= suma_data;
                sprintf(raspuns, "Suma insuficienta. Mai aveti de achitat %d lei.", datorie_client[fd]);
                write(fd, raspuns, strlen(raspuns) + 1);
                return 1; 
            }
        } else {
            sprintf(raspuns, "Eroare: Trebuie sa achitati %d lei. Comanda: /plata <suma>", datorie_client[fd]);
            write(fd, raspuns, strlen(raspuns) + 1);
            return 1;
        }
    }

    int loc_gasit = -1;
    for (int i = 1; i <= MAX_LOCURI; i++) {
        if (locuri_ocupate[i] == 1 && strcmp(buffer, nr_inmatriculare[i]) == 0) {
            loc_gasit = i;
            break;
        }
    }

    if (loc_gasit != -1) {
        time_t acum = time(NULL);
        int pret = (int)(acum - timp_intrare[loc_gasit]) / 2; 
        if (pret < 1) pret = 1;

        datorie_client[fd] = pret;
        loc_in_curs_de_plata[fd] = loc_gasit;

        sprintf(raspuns, "De plata: %d lei. Introduceti /plata <suma>", pret);
        write(fd, raspuns, strlen(raspuns) + 1);
        return 1; 
    } else {
        if (nr_locuri_libere <= 0) {
            sprintf(raspuns, "Parcarea este plina!");
            write(fd, raspuns, strlen(raspuns) + 1);
            return 0;
        }

        int loc_ales;
        do {
            loc_ales = (rand() % MAX_LOCURI) + 1;
        } while (locuri_ocupate[loc_ales] == 1);

        locuri_ocupate[loc_ales] = 1;
        strcpy(nr_inmatriculare[loc_ales], buffer);
        timp_intrare[loc_ales] = time(NULL);
        nr_locuri_libere--;

        sprintf(raspuns, "Ati parcat la locul %d. Locuri libere: %d", loc_ales, nr_locuri_libere);
        write(fd, raspuns, strlen(raspuns) + 1);
        return 0;
    }
}

int main() {
    struct sockaddr_in server, from;
    fd_set readfds, actfds;
    int sd, optval = 1;

    initializare();
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return errno;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) return errno;
    if (listen(sd, 5) == -1) return errno;

    FD_ZERO(&actfds);
    FD_SET(sd, &actfds);
    int nfds = sd;

    printf("Server Parcare activ pe portul %d...\n", PORT);

    while (1) {
        readfds = actfds;
        if (select(nfds + 1, &readfds, NULL, NULL, NULL) < 0) return errno;

        if (FD_ISSET(sd, &readfds)) {
            socklen_t len = sizeof(from);
            int client = accept(sd, (struct sockaddr *)&from, &len);
            FD_SET(client, &actfds);
            if (client > nfds) nfds = client;
        }

        for (int fd = 0; fd <= nfds; fd++) {
            if (fd != sd && FD_ISSET(fd, &readfds)) {
                if (handle_client(fd) == 0) {
                    close(fd);
                    FD_CLR(fd, &actfds);
                    datorie_client[fd] = 0;
                    loc_in_curs_de_plata[fd] = -1;
                }
            }
        }
    }
}