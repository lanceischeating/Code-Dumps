#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

int check_path(const char* PATH)
{
    int valid_executable = access(PATH, X_OK);
    if (valid_executable == 0)
        return 0;
    switch (errno)
    {
        case EACCES:
            printf("EACCES - Permission denied.\n");
            return EACCES;
        case EBADF:
            printf("EBADF - Pathname is relative but dirfd is netiehr AT_FDCWD (faccessat()) nor with a valid file descriptor\n");
            return EBADF;
        case EFAULT:
            printf("EFAULT - Pathname points outside your accessible address space\n");
            return EFAULT;
        case EIO:
            printf("IO Error\n");
            return EIO;
        case ENAMETOOLONG:
            printf("Pathname specified was too long\n");
            return ENAMETOOLONG;
        case ENOENT:
            printf("A component of pathname does not exist or is a dangling symbolic link.\n");
            return ENOENT;
        default:
            printf("Error number %d\n", errno);
            return errno;
    }
}

int check_decision(const char* PATH)
{
    while(1)
    {
        printf("Please confirm that this is the correct path passed to the desired directory: %s\n", PATH);
        printf("Y/n: ");
    
        char decision;
        if (scanf(" %c", &decision) != 1)
            continue;

        decision = tolower((unsigned char)decision);

        if (decision == 'y')
            return 0;

        if (decision == 'n')
            exit(EXIT_SUCCESS);

        if (decision != 'y' && decision != 'n')
        {
            printf("Incorrect argument was supplied\n");
            __fpurge(stdin);
            exit(EXIT_SUCCESS);
        }
        else
            break;
    }

    printf("\n");
    return 0;
}

int append_operation(const char* BASH_PATH, char* INPUT_BIN_PATH)
{
    if (chdir(BASH_PATH) < 0)
    {
        perror("Failed to change path to BASH directory\n");
        exit(errno);
    }
    const char* prefix = "\nexport PATH=\"";
    const char* suffix = ":$PATH\"";
    
    const int prefix_len = strlen(prefix);
    const int suffix_len = strlen(suffix);

    char* passed_bin_path = malloc(prefix_len + suffix_len + strlen(INPUT_BIN_PATH) + 1);
    if (!passed_bin_path)
    {
        printf("Failed to allocate appropriate string buffer size\n");
        return -1;
    }
    
    passed_bin_path[0] = '\0';
    
    strcat(passed_bin_path, prefix);
    strcat(passed_bin_path, INPUT_BIN_PATH);
    strcat(passed_bin_path, suffix);

    FILE* bash_file = fopen(".bashrc", "a");
    if (!bash_file)
    {
        printf("Failed to open bash file.\n");
        return -1;
    }
    fprintf(bash_file, "%s", passed_bin_path);
    printf("Successfully added %s, to .bashrc.\n", INPUT_BIN_PATH);
    printf("Recommend running the following command to update variables: source ~/.bashrc\n");

    fclose(bash_file);
    free(passed_bin_path);

    return 0;
}
int main(int argc, char* argv[])
{

    if (argc < 2)
    {
        printf("No argument was passed to the command.\n");
        printf("Add \'help\' for more information\n");
        exit(EXIT_FAILURE);
    }
    else if (strcmp(argv[1], "help") == 0)
    {
        printf("Supply the filepath. This command only takes a single argument\n");
        exit(EXIT_SUCCESS);
    }

    char* PATH_ARG = argv[1];
    if(check_path(PATH_ARG) != 0)
        exit(EXIT_FAILURE);

    
    const char* bash_path = getenv("HOME");
    if (!bash_path)
    {
        printf("Failed to retrieve home directory.\n");
        exit(EXIT_FAILURE);
    }
    check_decision(PATH_ARG);
    append_operation(bash_path, PATH_ARG);

    return 0;
    
}
