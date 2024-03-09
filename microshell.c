#include "microshell.h"

int print_error(char *s)
{
    while (*s)
        write(2,s++, 1);
    return (1);
}

int cd(char **av, int i)
{
    if (i != 2)
        return (print_error("error: cd: bad arguments\n"));
    else if (chdir(av[1]) == -1)
        return (print_error("error: cd: cannot change directory to "), print_error(av[1]), print_error("\n"));
    return (0);
}

int exec(char **av, char **env, int i)
{
    int fd[2];
    int s;
    int is_pipe = av[i] && !strcmp(av[i], "|");

    if (is_pipe && pipe(fd) == -1)
        return (print_error("error: fatal\n"));
    
    int pid = fork();
    if (!pid)
    {
        av[i] = 0;
        if ((is_pipe && dup2(fd[1], 1) == -1) || close(fd[0]) == -1 || close(fd[1]) == -1)
            return (print_error("error: fatal\n"));
        execve(*av, av, env);
        return (print_error("error: cannot execute "), print_error(*av), print_error("\n"));
    }

    waitpid(pid, &s, 0);
    if ((is_pipe && (dup2(fd[0], 0)) == -1) || close(fd[0]) == -1 || close(fd[1]) == -1)
        return (print_error("error: fatal\n"));
    
    return (WIFEXITED(s) && WEXITSTATUS(s));
}

int main(int ac, char **av, char **env)
{
    int i = 0;
    int s = 0;

    if (ac > 1)
    {
    while (av[i] && av[++i])
    {
        av += i;
        i = 0;
        while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        if (!strcmp(*av,"cd"))
            s = cd(av, i);
        else if (i)
        s = exec(av, env, i);
    }
    }
    return (s);
}