/*
 * Программа реализует работу утилиты Unix
 * ls, с ключами -l, -a,
 * автор: Андрей Макаров
 * email: mailmao.box@gmail.com
 * 2.07.2022
 */

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
#include <stdbool.h>

#define BUFSIZE      512
#define HALF_BUFSIZE (BUFSIZE/2)
#define STR_SIZE      sizeof("rwxrwxrwx") // размер строки вывода атрибутов файла в режиме ls -l
#define UGS   1                    // чтобы получить в строке поля: set-user-ID, set-group-ID, и sticky bit UGS (User,Group,StickyBit)

// характеристика одной записи в каталоге
typedef struct {
    struct stat stat_buf;
    char d_name[HALF_BUFSIZE];
} Entry;

// узел бинарного дерева
struct BTreeNode {
    Entry entry;
    struct BTreeNode *left;
    struct BTreeNode *right;
    struct BTreeNode *parent;
};

// режим работы утилиты ls
typedef enum {NO_OPT,L,A}LS_MODE;

//  функция для представления атрибутов файла в виде строки: rwxrwxrwx
char * file_permitions_string(mode_t perm, int flags){
    static char str[STR_SIZE];

    snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
             (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
             (perm & S_IXUSR) ?
                 (((perm & S_ISUID) && (flags & UGS)) ? 's' : 'x') :
                 (((perm & S_ISUID) && (flags & UGS)) ? 'S' : '-'),
             (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
             (perm & S_IXGRP) ?
                 (((perm & S_ISGID) && (flags & UGS)) ? 's' : 'x') :
                 (((perm & S_ISGID) && (flags & UGS)) ? 'S' : '-'),
             (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
             (perm & S_IXOTH) ?
                 (((perm & S_ISVTX) && (flags & UGS)) ? 't' : 'x') :
                 (((perm & S_ISVTX) && (flags & UGS)) ? 'T' : '-'));

    return str;
}

// функция для печати информации о файле в режиме ls -l
static void print_ls_l_opt(Entry* entry){

    if(entry->d_name[0] != '.'){
         printf(" %s", file_permitions_string(entry->stat_buf.st_mode, 0));  // печать: drwxrwxr-x

         printf(" %ld", entry->stat_buf.st_nlink);                 // печать: number hard links

         struct passwd* pw = getpwuid(entry->stat_buf.st_uid);
         printf(" %s", pw->pw_name);                               // печать: username

         struct group * pg = getgrgid(entry->stat_buf.st_gid);
         printf(" %s", pg->gr_name);                               // печать: groupname

         printf(" %7lld", (long long) entry->stat_buf.st_size);    // печать: file size

         char* timestr = ctime(&entry->stat_buf.st_ctime);
         char time_substr[26];
         memset(time_substr, '\0',26);
         timestr+=4;
         strncpy(time_substr,timestr,12);
         printf(" %s", time_substr);                               // печать: time of change
         printf(" %s\n", entry->d_name);                           // печать: file's name
    }
}

// функция для печати информации о файле в режиме ls без опций
static void print_ls_no_opt(Entry* entry){
    if(entry->d_name[0] == '.' ||
      (entry->d_name[0] == '.' && entry->d_name[1] == '.' ) ||
      (entry->d_name[0] == '.' && entry->d_name[1] != '.' )  )
    {
        return;
    }
    printf("%s ", entry->d_name);
}

// функция для печати информации о файле в режиме ls -a
static void print_ls_a_opt(Entry* entry){
    printf("%s ", entry->d_name);
}

// функция для добавления узла с записью о файле в бинарное дерево
static bool add_node_to_binary_tree(struct BTreeNode **root, Entry *entry){
    struct BTreeNode* parent = NULL;
    struct BTreeNode* new_node = NULL;

    new_node = (struct BTreeNode*)malloc(sizeof(struct BTreeNode));
    if(!new_node){
        return false;
    }

    strcpy(new_node->entry.d_name,entry->d_name);
    new_node->entry.stat_buf = entry->stat_buf;

    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;

    if((*root) == NULL){
        *root = new_node;
    }else{
        parent = *root;

        while(parent != NULL){
            if( strcasecmp(new_node->entry.d_name, parent->entry.d_name) < 0){
                if(parent->left == NULL){
                    parent->left = new_node;
                    new_node->parent = parent;
                    break;
                }else{
                    parent = parent->left;
                }
            }else{
                if(parent->right == NULL){
                    parent->right = new_node;
                    new_node->parent = parent;
                    break;
                }else{
                    parent = parent->right;
                }
            }
        }
    }
    return true;
}

// функция для обхода бинарного дерева и вывода на печать
// его узлов в отсортированном по алфавиту порядке
static void order_tree(struct BTreeNode *root, LS_MODE lsmode){
    struct BTreeNode* current_node = root;

    if(current_node != NULL){
        order_tree(current_node->left,lsmode);

        switch(lsmode){
            case NO_OPT:{ print_ls_no_opt(&current_node->entry);  } break;
            case L:     { print_ls_l_opt(&current_node->entry);   } break;
            case A:     { print_ls_a_opt(&current_node->entry);   } break;
        }

        order_tree(current_node->right,lsmode);

    }
}

// функция для удаления бинарного дерева
static void delete_btree(struct BTreeNode* node){
    if(node == NULL){
        return;
    }
    delete_btree(node->left);
    delete_btree(node->right);
    free(node);
}



int main(int argc, char* argv[]){

    LS_MODE lsmode;

    DIR *current_dir;
    struct dirent *dir_entry;
    struct stat stat_buf;
    char filepath[BUFSIZE];
    char directory_name[HALF_BUFSIZE];

    struct BTreeNode *root = NULL;
    Entry current_entry;

    int opt  = 0;

    memset(filepath,'\0',BUFSIZE);
    memset(directory_name,'\0',HALF_BUFSIZE);

    // если ls запускается без параметров
    if(argc == 1){
        lsmode = NO_OPT;
        directory_name[0] = '.';
    }else if(argc == 2 && (argv[1][0] == '.' || argv[1][0] == '/') ) //если ls без ключей но с путем ls /etc/..
    {
        lsmode = NO_OPT;
        strcpy(directory_name,argv[1]);
    }else {
        //разбор параметров командной строки
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
    while((dir_entry = readdir(current_dir)) != NULL){
        sprintf(filepath, "%s/%s", directory_name, dir_entry->d_name);
        stat(filepath, &stat_buf);

        current_entry.stat_buf = stat_buf;
        strcpy(current_entry.d_name,dir_entry->d_name);

        if(!add_node_to_binary_tree(&root, &current_entry)){
            perror("Couldn't add node to binary tree\n");
            closedir(current_dir);
            return EXIT_FAILURE;
        }
        memset(current_entry.d_name,'\0',HALF_BUFSIZE );
    }

    // вывод на печать бинарного дерева в отсортированном порядке
    // при его обходе
    order_tree(root,lsmode);
    printf("\n");

    //удаление бинарного дерева
    delete_btree(root);

    // закрываем директорию
    closedir(current_dir);

    return EXIT_SUCCESS;
}
