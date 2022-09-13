#include "shell.h"

/**
 * clear_en - initializes info_t struct
 * @info: struct address
 */
void clear_en(parse_t *info)
{
	info->arg = NULL;
	info->argv = NULL;
	info->path = NULL;
	info->argc = 0;
}

/**
 * set_en - initializes info_t struct
 * @info: struct address
 * @av: argument vector
 */
void set_en(parse_t *info, char **av)
{
	int i = 0;

	info->file_name = av[0];
	if (info->arg)
	{
		info->argv = strtow(info->arg, " \t");
		if (!info->argv)
		{

			info->argv = malloc(sizeof(char *) * 2);
			if (info->argv)
			{
				info->argv[0] = _strdup(info->arg);
				info->argv[1] = NULL;
			}
		}
		for (i = 0; info->argv && info->argv[i]; i++)
			;
		info->argc = i;

		replace_alias(info);
		replace_variables(info);
	}
}

/**
 * free_en - frees info_t struct fields
 * @info: struct address
 * @all: true if freeing all fields
 */
void free_en(parse_t *info, int all)
{
	ffree(info->argv);
	info->argv = NULL;
	info->path = NULL;
	if (all)
	{
		if (!info->cmd_buf)
			free(info->arg);
		if (info->env)
			free_list(&(info->env));
		if (info->previous)
			free_list(&(info->previous));
		if (info->alias)
			free_list(&(info->alias));
		ffree(info->environ);
			info->environ = NULL;
		c_free((void **)info->cmd_buf);
		if (info->read_fd > 2)
			close(info->read_fd);
		_putchar(BUF_FLUSH);
	}
}
