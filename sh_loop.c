#include "shell.h"

/**
 * hsh - main shell loop
 * @info: the parameter & return info struct
 * @av: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int hsh(parse_t *info, char **av)
{
	ssize_t r = 0;
	int builtin_ret = 0;

	while (r != -1 && builtin_ret != -2)
	{
		clear_en(info);
		if (inter(info))
			_puts("$ ");
		er_putchar(BUF_FLUSH);
		r = get_input(info);
		if (r != -1)
		{
			set_en(info, av);
			builtin_ret = find_builtin(info);
			if (builtin_ret == -1)
				find_cmd(info);
		}
		else if (inter(info))
			_putchar('\n');
		free_en(info, 0);
	}
	write_oldpath(info);
	free_en(info, 1);
	if (!inter(info) && info->stat)
		exit(info->stat);
	if (builtin_ret == -2)
	{
		if (info->error_num == -1)
			exit(info->stat);
		exit(info->error_num);
	}
	return (builtin_ret);
}

/**
 * find_builtin - finds a builtin command
 * @info: the parameter & return info struct
 *
 * Return: -1 if builtin not found,
 * 0 if builtin executed successfully,
 * 1 if builtin found but not successful,
 *-2 if builtin signals exit()
 */
int find_builtin(parse_t *info)
{
	int i, built_in_ret = -1;
	built_in_t builtintbl[] = {
		{"exit", __exit},
		{"env", _env},
		{"help", ch_dir},
		{"history", _oldpath},
		{"setenv", _set_env},
		{"unsetenv", _unset_env},
		{"cd", _cd},
		{"alias", c_alias},
		{NULL, NULL}
	};

	for (i = 0; builtintbl[i].type; i++)
		if (_strcmp(info->argv[0], builtintbl[i].type) == 0)
		{
			info->line_count++;
			built_in_ret = builtintbl[i].func(info);
			break;
		}
	return (built_in_ret);
}

/**
 * find_cmd - finds a command in PATH
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void find_cmd(parse_t *info)
{
	char *path = NULL;
	int i, k;

	info->path = info->argv[0];
	if (info->flag_count == 1)
	{
		info->line_count++;
		info->flag_count = 0;
	}
	for (i = 0, k = 0; info->arg[i]; i++)
		if (!delim(info->arg[i], " \t\n"))
			k++;
	if (!k)
		return;

	path = find_path(info, _getenv(info, "PATH="), info->argv[0]);
	if (path)
	{
		info->path = path;
		fork_cmd(info);
	}
	else
	{
		if ((inter(info) || _getenv(info, "PATH=")
			|| info->argv[0][0] == '/') && _cmd(info, info->argv[0]))
			fork_cmd(info);
		else if (*(info->arg) != '\n')
		{
			info->stat = 127;
			print_error(info, "not found\n");
		}
	}
}

/**
 * fork_cmd - forks a an exec thread to run cmd
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void fork_cmd(parse_t *info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
		/* TODO: PUT ERROR FUNCTION */
		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(info->path, info->argv, get_environ(info)) == -1)
		{
			free_en(info, 1);
			if (errno == EACCES)
				exit(126);
			exit(1);
		}
		/* TODO: PUT ERROR FUNCTION */
	}
	else
	{
		wait(&(info->stat));
		if (WIFEXITED(info->stat))
		{
			info->stat = WEXITSTATUS(info->stat);
			if (info->stat == 126)
				print_error(info, "Permission denied\n");
		}
	}
}
