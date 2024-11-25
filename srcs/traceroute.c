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

void set_socket_timeout(int socket_fd, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

void send_udp_packet(t_traceroute *tr, int ttl) {
    if (setsockopt(tr->udp_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("Error: Cannot set TTL");
        close(tr->socket_fd);
        close(tr->udp_socket);
        exit(EXIT_FAILURE);
    }

    tr->dest_addr.sin_port = htons(33434 + ttl);
    if (sendto(tr->udp_socket, "", 0, 0, (struct sockaddr *)&tr->dest_addr, sizeof(tr->dest_addr)) < 0) {
        perror("Error: Cannot send UDP packet");
    }
}

int receive_icmp_response(t_traceroute *tr, struct sockaddr_in *recv_addr, char *recv_buffer, socklen_t *addr_len) {
    return recvfrom(tr->socket_fd, recv_buffer, 512, 0, (struct sockaddr *)recv_addr, addr_len);
}

void process_icmp_response(t_hop *hop, char *recv_buffer, int received) {
    if (received > 0) {
        struct iphdr *ip_header = (struct iphdr *)recv_buffer;
        struct icmphdr *icmp_header = (struct icmphdr *)(recv_buffer + ip_header->ihl * 4);

        // Récupérer l'adresse IP source
        inet_ntop(AF_INET, &ip_header->saddr, hop->ip_address, sizeof(hop->ip_address));

        // Vérifier les types ICMP
        if (icmp_header->type == 11 && icmp_header->code == 0) {
            hop->status = TIME_EXCEEDED;
        } else if (icmp_header->type == 3) {
            if (icmp_header->code == 3) {
                hop->status = DESTINATION_REACHED;
            } else {
                hop->status = DESTINATION_UNREACHABLE;
            }
        } else {
            hop->status = UNKNOWN;
        }
    } else {
        hop->status = NO_REPLY;
    }
}

void perform_traceroute(t_traceroute *tr) {
    char recv_buffer[512];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    struct timeval start, end;
    t_hop hops[3];
    int destination_reached = 0;

    create_sockets(tr);
    set_socket_timeout(tr->socket_fd, 1);
    printf("traceroute to %s (%s), %d hops max, 60 byte packets\n", tr->target, tr->ip_address, tr->max_ttl);
    for (int ttl = 1; ttl <= tr->max_ttl && !destination_reached; ttl++) {
        printf("%d ", ttl);

        for (int i = 0; i < 3; i++) {
            gettimeofday(&start, NULL);
            send_udp_packet(tr, ttl);

            int received = receive_icmp_response(tr, &recv_addr, recv_buffer, &addr_len);
            gettimeofday(&end, NULL);

            initialize_hop(&hops[i], ttl, &start, &end, &recv_addr, received);
            process_icmp_response(&hops[i], recv_buffer, received);
        }

        print_hop(hops);

        for (int i = 0; i < 3; i++) {
            if (hops[i].status == DESTINATION_REACHED) {
                destination_reached = 1;
                break;
            }
        }
        printf("\n");
    }
    close(tr->socket_fd);
    close(tr->udp_socket);
}
