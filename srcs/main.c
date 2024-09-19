#include <ft_traceroute.h>

int main(int argc, char **argv)
{
    t_traceroute traceroute; 
    parse_arguments(argc, argv, &traceroute);
    printf("traceroute to %s (%s), %d hops max, 60 byte packets\n", traceroute.target, traceroute.ip_address, traceroute.max_ttl);

    perform_traceroute(&traceroute);
    
    free(traceroute.target);
    return 0;
}