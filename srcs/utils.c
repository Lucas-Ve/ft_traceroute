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

void print_hop(t_hop *hops)
{
    int unique_ips = 0;
    for (int i = 0; i < 3; i++)
    {
        if (hops[i].ip_address[0] != '\0')
        {
            int is_unique = 1;

            // Vérifie si l'adresse IP a déjà été affichée
            for (int j = 0; j < i; j++)
            {
                if (strcmp(hops[i].ip_address, hops[j].ip_address) == 0)
                {
                    is_unique = 0; // L'adresse IP n'est pas unique
                    break;
                }
            }

            // Affiche l'adresse IP unique avec le temps de réponse
            if (is_unique)
            {
                if (unique_ips > 0)
                    printf(" ");
                printf("(%s) %.3f ms", hops[i].ip_address, hops[i].response_time_ms);
                unique_ips++;
            }
            else
            {
                printf(" %.3f ms", hops[i].response_time_ms);
            }
        }
        else
        {
            if (unique_ips > 0)
                printf(" ");
            printf("*");
            unique_ips++;
        }
    }
}