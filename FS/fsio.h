#ifndef _FS_FSIO_H_
#define _FS_FSIO_H_

#ifdef DEBUG
#define DEBUG(...) fprintf(stdout, __VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define ERROR(...) fprintf(stderr, __VA_ARGS__);

/**
 * clear all contents in path.
 */
int fio_clear_dir(const char *path);

#endif
