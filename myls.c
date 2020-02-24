//includes
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

//constants
#define PERMISSIONS_ARRAY_LENGTH 10
#define TIME_ARRAY_LENGTH 100
#define FILE_NAME_LIMIT 260

//global variables
int allfiles = 0;
int longlisting = 0;

//function prototypes
void checkFile(char* content);
void printFiles(char* path);
void getPermissions(char permissions[], mode_t filePerm);

int main(int argc, char *argv[]) {
    int opt;
    char *pathName;
    struct stat isDir;

    //Parses through command line options
    //Sets optind to the index of argv that options end
    //In theory, the name of a path or file if it exists
    while((opt = getopt(argc, argv, "al")) != -1) {
        switch(opt) {
            case 'a':
                allfiles = 1;
                break;

            case 'l':
                longlisting = 1;
                break;

            case '?':
                exit(1);
        }
    }

    if(!argv[optind]) {
        pathName = ".";
        checkFile(pathName);
    }
    else if(!(argv[optind+1])){
        pathName = argv[optind];
        checkFile(pathName);
    }
    else {
        while(argv[optind]) {
            pathName = argv[optind];
            if(stat(pathName,&isDir) == 0) { //if directory, print pathName
                if(S_ISDIR(isDir.st_mode)) {
                    printf("%s:\n", pathName);
                }
            }
            checkFile(pathName);
            printf("\n");
            optind++;
        }
    }
}

void checkFile(char* content) {
    struct dirent *file;
    char updatedPath[FILE_NAME_LIMIT];
    DIR *openDir;
    struct stat validFile;

    if((allfiles == 1) && (longlisting == 1)) {
        if((openDir = opendir(content)) != NULL) {
            while((file = readdir(openDir)) != NULL) {
                snprintf(updatedPath, FILE_NAME_LIMIT, "%s/%s",
                            content, file->d_name);
                printFiles(updatedPath);
                printf("%s\n", file->d_name);
            }
        }
        else {
            printFiles(content);
            printf("%s\n", content);
        }
    }

    else if(allfiles == 1) {
        if((openDir = opendir(content)) != NULL) {
        //    printf("%s:\n", content);
            while((file = readdir(openDir)) != NULL) {
                printf("%s\n", file->d_name);
            }
        }
        else {
            printf("%s\n", content);
        }
    }

    else if(longlisting == 1) {
        if((openDir = opendir(content)) != NULL) {
            while((file = readdir(openDir)) != NULL) {
                if (file->d_name[0] != '.') {
                    snprintf(updatedPath, FILE_NAME_LIMIT, "%s/%s",
                                content, file->d_name);
                    printFiles(updatedPath);
                    printf("%s\n", file->d_name);
                }
            }
        }
        else {
            printFiles(content);
            printf("%s\n", content);
        }
    }

    else {
        if((openDir = opendir(content)) != NULL) {
        //    printf("%s:\n", content);
            while((file = readdir(openDir)) != NULL) {
                if (file->d_name[0] != '.') {
                    printf("%s\n", file->d_name);
                }
            }
        }
        else {
            if(stat(content, &validFile) == 0) {
                printf("%s\n", content);
            }
            else {
                perror("stat");
            }
        }
    }

    if(openDir) {
        if(closedir(openDir) == -1) {
            perror("closedir");
        }
    }
}

void printFiles(char* path) {

    struct stat statStruct;
    struct passwd *pwd;
    struct passwd *pwdGroup;
    int statError;
    char type;
    long long size;
    struct tm *time;
    char locltime[TIME_ARRAY_LENGTH];
    char permArray[PERMISSIONS_ARRAY_LENGTH];

    if((statError = stat(path,&statStruct)) < 0) {
        perror("stat");
    }
    else {
    //when staterror != 0; need to return permissions
    //information about the files/directories
        if(S_ISREG(statStruct.st_mode)) {
            //checks if content is a file
            type = '-';
        }
        else if (S_ISDIR(statStruct.st_mode)){
            //checks if content is a directory
            type = 'd';
        }

        if((pwd = getpwuid(statStruct.st_uid)) == 0) {
            perror("getpwuid");
            printf("%ld/n", statStruct.st_ino);
        }
        if((pwdGroup = getpwuid(statStruct.st_gid)) == 0) {
            perror("getpwuid");
            printf("%ld/n", statStruct.st_ino);
        }
        getPermissions(permArray,statStruct.st_mode);
        size = statStruct.st_size;
        time = localtime(&statStruct.st_mtime);
        strftime(locltime, TIME_ARRAY_LENGTH,
                            "%b %d %H:%M", time);
        printf("%c%s %s %s %lld %s ",
                type, permArray, pwd->pw_name,
                pwdGroup->pw_name, size, locltime);
    }
}

void getPermissions(char permissions[],mode_t filePerm) {

    // user perms
    permissions[0] = (filePerm & S_IRUSR) ? 'r' : '-' ;
    permissions[1] = (filePerm & S_IWUSR) ? 'w' : '-' ;
    permissions[2] = (filePerm & S_IXUSR) ? 'x' : '-' ;
    // group perms
    permissions[3] = (filePerm & S_IRGRP) ? 'r' : '-' ;
    permissions[4] = (filePerm & S_IWGRP) ? 'w' : '-' ;
    permissions[5] = (filePerm & S_IXGRP) ? 'x' : '-' ;
    // other perms
    permissions[6] = (filePerm & S_IROTH) ? 'r' : '-' ;
    permissions[7] = (filePerm & S_IWOTH) ? 'w' : '-' ;
    permissions[8] = (filePerm & S_IXOTH) ? 'x' : '-' ;

    permissions[9] = '\0';
}
