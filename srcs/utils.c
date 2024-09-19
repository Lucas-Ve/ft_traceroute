#include <ft_traceroute.h>

// Fonction pour afficher l'aide
void print_help() {
    printf("Usage: ft_traceroute [options] <destination>\n");
    printf("Options:\n");
    printf("  --help       Display this help message and exit\n");
    printf("\n");
    printf("Destination:\n");
    printf("  <destination>  IP address or hostname to trace route to\n");
}

// Fonction pour initialiser la structure s_traceroute
void initialize_traceroute(t_traceroute *tr, const char *target) {
    memset(tr, 0, sizeof(t_traceroute));  // Initialise la structure à zéro
    tr->target = strdup(target);          // Copie le nom d'hôte ou l'adresse IP cible
    tr->max_ttl = 30;                     // TTL maximum par défaut, ajustable si nécessaire

    // Vérifie si l'argument est une adresse IPv4 ou un nom d'hôte
    if (is_valid_ipv4(target)) {
        strncpy(tr->ip_address, target, INET_ADDRSTRLEN);
    } else if (resolve_hostname(target, tr->ip_address)) {
    } else {
        free(tr->target);
        exit(EXIT_FAILURE);
    }

    // Prépare l'adresse de destination pour l'envoi de paquets
    tr->dest_addr.sin_family = AF_INET;
    tr->dest_addr.sin_port = htons(33434);  // Port utilisé par défaut pour traceroute
    inet_pton(AF_INET, tr->ip_address, &(tr->dest_addr.sin_addr));
}

// Fonction pour calculer le temps écoulé en millisecondes entre deux timeval
double calculate_time_diff(struct timeval *start, struct timeval *end) {
    return (double)(end->tv_sec - start->tv_sec) * 1000.0 + 
           (double)(end->tv_usec - start->tv_usec) / 1000.0;
}

// Fonction pour initialiser et remplir la structure t_hop
void initialize_hop(t_hop *hop, int ttl, struct timeval *start, struct timeval *end, struct sockaddr_in *recv_addr, int received) {
    hop->ttl = ttl;
    hop->response_time_ms = calculate_time_diff(start, end);
    hop->ip_address[0] = '\0'; // Initialise à vide pour les cas sans réponse

    if (received > 0) {
        inet_ntop(AF_INET, &recv_addr->sin_addr, hop->ip_address, sizeof(hop->ip_address));
    }
}