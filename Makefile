# Nom de l'exécutable
NAME = ft_traceroute

# Options du compilateur
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I includes

# Dossiers
SRCDIR = srcs
INCDIR = includes

# Fichiers sources et objets
SRCS = $(SRCDIR)/main.c $(SRCDIR)/parsing.c $(SRCDIR)/utils.c $(SRCDIR)/traceroute.c
OBJS = $(SRCS:.c=.o)

# Commande pour créer l'exécutable
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lm

# Commande pour recompiler uniquement les fichiers modifiés
$(SRCDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/ft_traceroute.h
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets
clean:
	rm -f $(OBJS)

# Nettoyage complet (fichiers objets + exécutable)
fclean: clean
	rm -f $(NAME)

# Recompiler tout depuis zéro
re: fclean all

.PHONY: all clean fclean re test-v test-help
