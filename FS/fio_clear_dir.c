/**
 * This version of clear_dir is aimed at avoiding
 * stack overflow.
 * For the case; if the inodes of some fs are full and
 * all of the node are dirs and each dir has one child
 * dir.
 * But, if there are N dirs in target dir, we will call
 * fio_clear_dir_recursion 2*N times.
 * The fires N times, rm files and push dirs into list
 * and the second N times, pop the dirs and rmdir.
 */

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "fsio.h"

struct dir_list {
    char *path;
    struct dir_list *next;
};

typedef struct dir_list dir_list;

static int fio_clear_dir_recursion(dir_list *list)
{
    int ret = 0;
    DIR *parent = NULL;
    struct dirent *dir = NULL;
    struct dirent entry;
    dir_list *head = list;
    char *path = NULL;

    if (head == NULL)
        return 0;

    path = list->path;

    if ((parent = opendir (path)) == NULL) {
        printf("open dir %s failed (%s).\n",
               path, strerror(errno));
        free(head->path);
        free(head);
        return -1;
    }


    while (readdir_r(parent, &entry, &dir) == 0
           && dir != NULL) {
        //while ((dir = readdir(parent)) != NULL) {
        struct stat buf;
        int path_len;
        char *target_path = NULL;

        if (strcmp(dir->d_name, ".") == 0
            || strcmp(dir->d_name, "..") == 0)
            continue;

        path_len = strlen(path) + strlen(dir->d_name) + 2;
        target_path = (char *)malloc(path_len);
        memset(target_path, '\0', path_len);
        snprintf(target_path, path_len, "%s/%s", path, dir->d_name);
        printf("target_path = %s d_name = %s path_len = %d\n", target_path, dir->d_name, path_len);
        if (stat(target_path, &buf) == -1) {
            printf("stat %s errror (%s)\n", target_path, strerror(errno));
            continue;
        }

        if (S_ISDIR(buf.st_mode)) {
            printf("is_idr\n");
            dir_list *lst = (dir_list*)malloc(sizeof(dir_list));
            lst->path = target_path;
            lst->next = head;
            head = lst;
        }
        else {
            ret = unlink(target_path);
            if (ret == -1)
                printf("failed to remove %s (%s)", dir->d_name, strerror(errno));
            free(target_path);
        }
    }

    closedir(parent);
    // failed at the firest time, the dir is just pushed into list
    // success at the second time, the dir is just poped from list
    ret = rmdir(path);
    if (ret == 0) {
        // this means no dirs in this dir.
        // So the head is this dir, we rm it.
        head = head->next;
        free(list->path);
        free(list);
    }

    // don't remove the root dir
    if (head->next == NULL) {
        free(head->path);
        free(head);
        return 0;
    }

    // using tail recursion
    return fio_clear_dir_recursion(head);
}

/**
 * This function is comatible with linux
 */
int fio_clear_dir(const char *path)
{
    if (path == NULL)
        return -1;

    dir_list *lst = (dir_list  *) malloc(sizeof(dir_list));
    lst->path = strdup(path);
    lst->next = NULL;
    return fio_clear_dir_recursion(lst);
}

#ifdef TEST
int main(int argc, char *argv[])
{
    if (argc != 2) {
        return -1;
    }

    return fio_clear_dir(argv[1]);
}
#endif

/*
 * Version 0.0.1  (Werther Zhang)
 *      Create file.
 * Version 0.0.2  (Werther Zhang)
 *      use readdir_r instead of readdir
 *      fix mem leak
 */
