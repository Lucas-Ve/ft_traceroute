#include <ft_traceroute.h>

// Fonction principale pour le parsing des arguments et initialisation
void parse_arguments(int argc, char *argv[], t_traceroute *tr) {
    int destination_set = 0; // Flag pour vérifier si une destination a été définie

    // Première passe : vérifier si '--help' est présent dans les arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            exit(EXIT_SUCCESS);
        }
    }

    // Deuxième passe : traiter les autres arguments
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        // Ignorer '--help' car il a déjà été géré dans la première passe
        if (strcmp(arg, "--help") == 0) {
            continue;
        }

        // Vérification des options invalides commençant par '--'
        if (strncmp(arg, "--", 2) == 0) {
            fprintf(stderr, "Error: Bad option `%s' (argc %d)\n", arg, argc);
            exit(EXIT_FAILURE);
        }

        // Gestion de la destination (adresse IP ou nom d'hôte)
        if (!destination_set) {
            initialize_traceroute(tr, arg);
            destination_set = 1; // Marque la destination comme définie
        } else {
            // Si une destination est déjà définie, affiche une erreur
            fprintf(stderr, "Error: Only one destination can be specified.\n");
            print_help();
            exit(EXIT_FAILURE);
        }
    }

    // Vérifie si aucune destination n'a été fournie
    if (!destination_set) {
        fprintf(stderr, "Error: No destination specified.\n");
        print_help();
        exit(EXIT_FAILURE);
    }
}


// Fonction pour résoudre un nom d'hôte en IPv4
int resolve_hostname(const char *hostname, char *ip_buffer) {
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // On ne gère que l'IPv4
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(status));
        return 0;
    }

    // Convertit la première adresse trouvée en chaîne de caractères IPv4
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ip_buffer, INET_ADDRSTRLEN);

    freeaddrinfo(res);
    return 1;
}

// Fonction pour vérifier si une adresse est IPv4
int is_valid_ipv4(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}