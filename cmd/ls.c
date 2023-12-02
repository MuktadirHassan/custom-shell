// implementation of ls command
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

void ls()
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    struct passwd *pw;
    struct group *gr;

    dir = opendir(".");
    if (dir == NULL)
    {
        perror("opendir() error");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (stat(entry->d_name, &fileStat) < 0)
        {
            perror("stat() error");
            continue;
        }

        pw = getpwuid(fileStat.st_uid);
        gr = getgrgid(fileStat.st_gid);

        printf("%s\t", entry->d_name);
        printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
        printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
        printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
        printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
        printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
        printf("\t%ld\t", fileStat.st_size);
        printf("%s\t", pw->pw_name);
        printf("%s\n", gr->gr_name);
    }

    closedir(dir);
}
