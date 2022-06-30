#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


#define BUFSIZE 512

int main(int argc, char* argv[])
{
    DIR *current_dir;
    struct dirent *entry;
    struct stat stat_buf;

    char buf[BUFSIZE];
    current_dir = opendir(argv[1]);
    if(!current_dir){
        perror("Couldn't open direcory\n");
        return EXIT_FAILURE;
    }

    while((entry = readdir(current_dir)) != NULL){
        sprintf(buf, "%s/%s", argv[1], entry->d_name);
        stat(buf, &stat_buf);
        printf("%7zu",stat_buf.st_size);
        printf(" %s\n", entry->d_name);
    }

    closedir(current_dir);
}
