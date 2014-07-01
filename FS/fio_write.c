#include <stdio.h>
#include <unistd.h>
#include <errno.h>

ssize_t fio_write(int fd, const void *buf, size_t count)
{
    ssize_t ret = 0;
    ssize_t nr = 0;

    while (count != 0 && (nr = write(fd, buf, count)) != 0) {
        if (nr == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("write");
            break;
        }

        count -= nr;
        buf += nr;
        ret += nr;
    }
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
    int fd_w = open("/tmp/fio_read.c", O_WRONLY|O_CREAT, S_IWUSR);
    ret = fio_write(fd_w, buf, ret);
    printf("ret = %d\n", ret);
    
    return 0;
}
#endif
