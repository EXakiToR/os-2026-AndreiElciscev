#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *name, int show_inode)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);

    if (p > buf && *(p - 1) != '/')
        *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        if (strcmp(de.name, ".") == 0 ||
            strcmp(de.name, "..") == 0)
            continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';

        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        if (strcmp(de.name, name) == 0) {
            if (show_inode)
                printf("%s (inode %d)\n", buf, st.ino);
            else
                printf("%s\n", buf);
        }

        if (st.type == T_DIR)
            find(buf, name, show_inode);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    int show_inode = 0;

    if (argc != 3 && argc != 4) {
        fprintf(2, "Usage: find <directory> <name> [-n]\n");
        exit(1);
    }

    if (argc == 4) {
        if (strcmp(argv[3], "-n") != 0) {
            fprintf(2, "find: unknown option %s\n", argv[3]);
            exit(1);
        }

        show_inode = 1;
    }

    find(argv[1], argv[2], show_inode);

    exit(0);
}
