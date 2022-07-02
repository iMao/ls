#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include "file_perms.h"
#include "btree.h"

#define BUFSIZE         512

typedef enum {NO_OPT,L,A}LS_MODE;

static void print_ls_l_opt(const struct stat* stat_buf, const char* fname){
     printf(" %s",   filePermStr(stat_buf->st_mode, 0)); // output: drwxrwxr-x

     printf(" %ld", stat_buf->st_nlink);                  // output: number hard links

     struct passwd* pw = getpwuid(stat_buf->st_uid);
     printf(" %s", pw->pw_name);                         // output: username

     struct group * pg = getgrgid(stat_buf->st_gid);
     printf(" %s", pg->gr_name);                         // output: groupname

     printf(" %7lld", (long long) stat_buf->st_size);    // output: file size

     char* timestr = ctime(&stat_buf->st_ctime);
     char time_substr[26];
     memset(time_substr, '\0',26);
     timestr+=4;
     strncpy(time_substr,timestr,12);
     printf(" %s", time_substr);                         // output: time of change
     printf(" %s\n", fname);                             // output: file's name
}

static void print_ls_no_opt(const char* fname){
    if(strcmp(fname, ".") != 0 && strcmp(fname,"..") != 0){
        printf("%s ", fname);
    }
}

static void print_ls_a_opt(const char* fname){
    printf("%s ", fname);
}

int main(int argc, char* argv[]){

    LS_MODE lsmode;

    DIR *current_dir;
    struct dirent *entry;
    struct stat stat_buf;
    char filepath[BUFSIZE];
    char directory_name[BUFSIZE/2];

    memset(filepath,'\0',BUFSIZE);
    memset(directory_name,'\0',BUFSIZE/2);

    // если ls запускается без параметров
    if(argc == 1){
        lsmode = NO_OPT;
        directory_name[0] = '.';
    }else{
        //разбор параметров командной строки
        int opt  = 0;
        while((opt = getopt(argc,argv,"lah")) != -1){

            if( opt == 'l'  && argc == 2 ){
                lsmode = L;
                directory_name[0] = '.';
                break;
            }else if( opt == 'a' && argc == 2){
                lsmode = A;
                directory_name[0] = '.';
                break;
            }else if( (opt == 'l' || opt == 'a') && argc >= 2){
                lsmode = (opt == 'l' ? L:A);
                strcpy(directory_name,argv[2]);
                break;
            }else if(opt == 'h') {
                printf("Please use cmdparams -l or -a like:\nls -a .\nls -l .\nls -l /etc\n");
                return EXIT_FAILURE;
            }
        }
    }


    // открываем директорию
    current_dir = opendir(directory_name);
    if(!current_dir){
        perror("Couldn't open direcory\n");
        return EXIT_FAILURE;
    }

    // читаем записи в директории и записываем их в бинарное дерево
    while((entry = readdir(current_dir)) != NULL){
        sprintf(filepath, "%s/%s", directory_name, entry->d_name);
        stat(filepath, &stat_buf);

        switch(lsmode){
            case NO_OPT:{ print_ls_no_opt(entry->d_name);  }break;
            case L:     { print_ls_l_opt(&stat_buf, entry->d_name); } break;
            case A:     { print_ls_a_opt(entry->d_name); } break;
        }

    }
    printf("\n");

    // закрываем директорию
    closedir(current_dir);

    return EXIT_SUCCESS;
}
