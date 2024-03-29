#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char *generator_command = NULL;
    char *consumer_command = NULL;

    // Check for correct number of command line arguments
    if (argc != 5)
    {
        return errno;
    }

    // Parse command line arguments
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-g") == 0)
        {
            generator_command = argv[i + 1];
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            consumer_command = argv[i + 1];
        }
        else
        {
            return errno;
        }
    }

    // Ensure both generator and consumer commands are provided
    if (generator_command == NULL || consumer_command == NULL)
    {
        return errno;
    }

    int pipefd[2]; // File descriptors for pipe
    pid_t pid;     // Process ID

    // Create pipe
    if (pipe(pipefd) == -1)
    {
        return errno;
    }

    // Fork a child process
    pid = fork();
    if (pid == -1)
    {
        return errno;
    }

    if (pid == 0)
    {                     // Child process (consumer)
        close(pipefd[1]); // Close unused write end

        // Redirect stdin to read from pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            return errno;
        }
        close(pipefd[0]); // Close original pipe read end

        // Split consumer command and its arguments
        char *token;
        char *args[10]; // Assuming a maximum of 10 arguments
        int i = 0;
        token = strtok(consumer_command, " ");
        while (token != NULL)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Execute consumer command
        if (execvp(args[0], args) == -1)
        {
            return errno;
        }
    }
    else
    {                     // Parent process (generator)
        close(pipefd[0]); // Close unused read end

        // Redirect stdout to write to pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            return errno;
        }
        close(pipefd[1]); // Close original pipe write end

        // Execute generator command using shell
        if (execlp("sh", "sh", "-c", generator_command, NULL) == -1)
        {
            return errno;
        }
    }

    return 0;
}