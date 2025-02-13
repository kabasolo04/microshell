#include "microshell.h"

int	p(char *line)
{
	if (line[0])
		return (write(2, &line[0], 1), p(&line[1]));
	return (1);
}

int	cd(int argc, char **cmd)
{
	if (argc != 2)
		return (p("error: cd: bad arguments\n"));
	if (chdir(cmd[0]) != 0)
		return (p("error: cd: cannot change directory to "), p(cmd[0]), p("\n"));
	return (0);
}

int	exec(int argc, char **argv, char **env, int pFlag)
{
	int	fd[2];
	int	pid;
	int	status;

	if (!strcmp(argv[0], "cd"))
		return (cd(argc, &argv[1]));
	if (pFlag && pipe(fd) == -1)
		return (p("error: fatal\n"));
	if ((pid = fork()) == 0)
	{
		if (pFlag && (dup2(fd[1], STDOUT_FILENO) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			return (p("error: fatal\n"));
		argv[argc] = NULL;
		return (execve(argv[0], argv, env), p("error: cannot execute "), p(argv[0]), p("\n"), exit(1), 1);
	}
	waitpid(pid, &status, 0);
	if (pFlag && (dup2(fd[0], STDIN_FILENO) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (p("error: fatal\n"));
	return (status);
}

int	main(int argc, char **argv, char **env)
{
	int	i = 0;
	int	status = 0;

	while (++i < argc)
	{
		int	j = i;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i ++;
		if (i != j)
			status = exec(i - j, &argv[j], env, (argv[i] && !strcmp(argv[i], "|")));
	}
	return (WIFEXITED(status) && WEXITSTATUS(status));
}
