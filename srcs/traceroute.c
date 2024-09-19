#include "ft_traceroute.h"

void create_sockets(t_traceroute *tr) {
    // Création du socket RAW pour recevoir des paquets ICMP
    tr->socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (tr->socket_fd < 0) {
        perror("Error: Cannot create raw socket");
        exit(EXIT_FAILURE);
    }

    // Création du socket UDP pour envoyer des paquets
    tr->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (tr->udp_socket < 0) {
        perror("Error: Cannot create UDP socket");
        close(tr->socket_fd);
        exit(EXIT_FAILURE);
    }
}

// Fonction pour configurer un timeout sur le socket RAW
void set_socket_timeout(int socket_fd, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

// Fonction pour envoyer un paquet UDP
void send_udp_packet(t_traceroute *tr, int ttl) {
    // Configuration du TTL pour le socket UDP
    if (setsockopt(tr->udp_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("Error: Cannot set TTL");
        close(tr->socket_fd);
        close(tr->udp_socket);
        exit(EXIT_FAILURE);
    }

    // Envoi du paquet UDP au destinataire sur un port non utilisé
    tr->dest_addr.sin_port = htons(33434 + ttl); // Port incrémental pour chaque TTL
    if (sendto(tr->udp_socket, "", 0, 0, (struct sockaddr *)&tr->dest_addr, sizeof(tr->dest_addr)) < 0) {
        perror("Error: Cannot send UDP packet");
    }
}

// Fonction pour recevoir une réponse ICMP
int receive_icmp_response(t_traceroute *tr, struct sockaddr_in *recv_addr, char *recv_buffer, socklen_t *addr_len) {
    return recvfrom(tr->socket_fd, recv_buffer, 512, 0, (struct sockaddr *)recv_addr, addr_len);
}

// Fonction principale pour exécuter le traceroute avec gestion des multiples adresses IP par saut
void perform_traceroute(t_traceroute *tr) {
    char recv_buffer[512];  // Buffer pour les paquets reçus
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    struct timeval start, end;
    t_hop hops[3];  // Tableau pour stocker les informations des trois paquets
    int destination_reached = 0;

    // Création et configuration des sockets
    create_sockets(tr);
    set_socket_timeout(tr->socket_fd, 2);

    for (int ttl = 1; ttl <= tr->max_ttl && !destination_reached; ttl++) {
        printf("%d ", ttl);

        // Envoi de trois paquets pour chaque TTL
        for (int i = 0; i < 3; i++) {
            gettimeofday(&start, NULL);
            send_udp_packet(tr, ttl);

            int received = receive_icmp_response(tr, &recv_addr, recv_buffer, &addr_len);
            gettimeofday(&end, NULL);

            initialize_hop(&hops[i], ttl, &start, &end, &recv_addr, received);
        }

        print_hop(hops);

        for (int i = 0; i < 3; i++)
        {
            if (!strcmp(hops[i].ip_address, tr->ip_address))
            {
                destination_reached = 1;
                break;
            }
        }
        printf("\n");
    }
    close(tr->socket_fd);
    close(tr->udp_socket);
}
