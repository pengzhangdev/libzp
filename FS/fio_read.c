#include <stdio.h>
#include <unistd.h>
#include <errno.h>

ssize_t fio_read(int fd, void *buf, size_t count)
{
    ssize_t nr = -1;
    ssize_t ret = 0;

    while (count != 0 && (nr = read(fd, buf, count)) != 0) {
        if (nr == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("read");
            break;
        }

        count -= nr;
        buf += nr;
        ret += nr;
    }

    return ret;
}

#ifdef _TEST_
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    char buf[1024] = {0};
    int fd = open("./fio_read.c", O_RDONLY);
    int ret = fio_read(fd, buf, 1024);
    printf("ret = %d\n", ret);
    return 0;
}
#endif
