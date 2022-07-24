#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int find(char *path, char *filename);

char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

int main(int argc, char *argv[])
{
    char *path = argv[1];
    char *filename = argv[2];

    int exitcode = find(path, filename);
    exit(exitcode);
}

int find(char *path, char *filename)
{
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512], *p;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", filename);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", filename);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
    case T_FILE:
        if (strcmp(de.name, filename) == 0)
        {
            printf("%s %s\n", path, de.name);
        }
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(de.name, filename) == 0)
            {
                printf("%s/%s\n", path, fmtname(de.name));
            }

            if (st.type == T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
            {
                find(buf, filename);
            }
        }
        break;
    }
    close(fd);
    return 0;
}