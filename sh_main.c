#include "shell.h"

/**
 * main - entry point
 * @ac: arg count
 * @av: arg vector
 *
 * Return: 0 on success, 1 on error
 */
int main(int ac, char **av)
{
	parse_t info[] = { EN_INIT };
	int fd = 2;

	asm ("mov %1, %0\n\t"
		"add $3, %0"
		: "=r" (fd)
		: "r" (fd));

	if (ac == 2)
	{
		fd = open(av[1], O_RDONLY);
		if (fd == -1)
		{
			if (errno == EACCES)
				exit(126);
			if (errno == ENOENT)
			{
				er_puts(av[0]);
				er_puts(": 0: Can't open ");
				er_puts(av[1]);
				er_putchar('\n');
				er_putchar(BUF_FLUSH);
				exit(127);
			}
			return (EXIT_FAILURE);
		}
		info->read_fd = fd;
	}
	p_env_list(info);
	read_oldpath(info);
	hsh(info, av);
	return (EXIT_SUCCESS);
}
