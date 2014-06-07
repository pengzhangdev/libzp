#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <string.h>

int get_ip_addr(const char *ifname, char *ip)
{
    int err = 0;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq req;

    if (sockfd == -1) {
        err = -1;
        goto exit;
    }

    strncpy(req.ifr_ifrn.ifrn_name, ifname, sizeof(req.ifr_ifrn.ifrn_name));
    err = ioctl(sockfd, SIOCGIFADDR, &req);

    if (err == 0) {
        memcpy(ip, inet_ntoa(((struct sockaddr_in*)(&req.ifr_addr))->sin_addr), 36);
    }

exit:
    if (sockfd) close(sockfd);
    return err;
}

#ifdef __TEST__
int main(int argc, char *argv[])
{
    int ret = 0;
    char ip[48] = {'\0'};
    if (argc == 1) {
        ret = get_ip_addr("eth0", ip);
    }
    else {
        ret = get_ip_addr(argv[1], ip);
    }

    printf("%d %s\n", ret, ip);

    return 0;
}

#endif
