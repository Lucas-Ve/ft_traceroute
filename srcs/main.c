#include <ft_traceroute.h>

int main(int argc, char **argv)
{
    t_traceroute traceroute; 
    parse_arguments(argc, argv, &traceroute);

    perform_traceroute(&traceroute);
    
    free(traceroute.target);
    return 0;
}