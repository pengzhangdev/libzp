#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <string.h>

int get_mac_addr(const char *ifname, char *mac)
{
    //unsigned char mac_addr[32] = {'\0'};
    int err = 0;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq req;

    if (sockfd == -1) {
        err = -1;
        goto exit;
    }

    strncpy(req.ifr_ifrn.ifrn_name, ifname, sizeof(req.ifr_ifrn.ifrn_name));
    err = ioctl(sockfd, SIOCGIFHWADDR, &req);

    if (err == 0) {
        snprintf(mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
                 (unsigned char)req.ifr_hwaddr.sa_data[0],
                 (unsigned char)req.ifr_hwaddr.sa_data[1],
                 (unsigned char)req.ifr_hwaddr.sa_data[2],
                 (unsigned char)req.ifr_hwaddr.sa_data[3],
                 (unsigned char)req.ifr_hwaddr.sa_data[4],
                 (unsigned char)req.ifr_hwaddr.sa_data[5]);
    }
exit:
    if (sockfd > 0) close(sockfd);
    return err;
}


#ifdef __TEST__
int main(int argc, char *argv[])
{
    int ret;
    char mac[32] = {'\0'};
    if (argc == 1)
        ret = get_mac_addr("eth0", mac);
    else
        ret = get_mac_addr(argv[1], mac);
    printf("%d %s\n", ret, mac);
}
#endif
