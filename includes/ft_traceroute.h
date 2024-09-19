#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>

// Définition d'une structure pour stocker les informations d'un saut
typedef struct s_hop {
    int ttl;                    // Time To Live pour ce saut
    char ip_address[INET_ADDRSTRLEN]; // Adresse IP en format texte
    double response_time_ms;    // Temps de réponse en millisecondes
} t_hop;

// Définition d'une structure pour le contexte général de traceroute
typedef struct s_traceroute {
    int socket_fd;              // Descripteur du socket
    struct sockaddr_in dest_addr; // Adresse du destinataire
    char *target;               // Nom d'hôte ou adresse IP cible
    int max_ttl;                // TTL maximum pour traceroute
    char ip_address[INET_ADDRSTRLEN];
    int udp_socket;               // Descripteur du socket UDP
} t_traceroute;

// Fonctions prototypes
void initialize_traceroute(t_traceroute *tr, const char *target);
void perform_traceroute(t_traceroute *tr);
void create_sockets(t_traceroute *tr);
void set_socket_timeout(int socket_fd, int seconds);
void send_udp_packet(t_traceroute *tr, int ttl);
int receive_icmp_response(t_traceroute *tr, struct sockaddr_in *recv_addr, char *recv_buffer, socklen_t *addr_len);
void initialize_hop(t_hop *hop, int ttl, struct timeval *start, struct timeval *end, struct sockaddr_in *recv_addr, int received);

//parsing
void parse_arguments(int argc, char *argv[], t_traceroute *tr);
int resolve_hostname(const char *hostname, char *ip_buffer);
int is_valid_ipv4(const char *ip);
void print_help();
void print_hop(t_hop *hops);

double calculate_time_diff(struct timeval *start, struct timeval *end);

#endif
