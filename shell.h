#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

/* for read/write buffers */
#define READ_BUF_SIZE 1024
#define WRITE_BUF_SIZE 1024
#define BUF_FLUSH -1

/* for command chaining */
#define CMD_NORM	0
#define CMD_OR		1
#define CMD_AND		2
#define CMD_CHAIN	3

/* for convert_number() */
#define CONVERT_LOWERCASE	1
#define CONVERT_UNSIGNED	2

/* 1 if using system getline() */
#define _GETLINE 0
#define _STRTOK 0

#define INFO_FILE	".simple_shell_history"
#define INFO_MAX	4096

extern char **environ;


/**
 * struct list - singly linked list
 * @num: the number field
 * @str: a string
 * @next: points to the next node
 */
typedef struct list
{
	int num;
	char *str;
	struct list *next;
} list_t;

/**
 *struct parse - contains pseudo-arguements to pass into a function,
 *		allowing uniform prototype for function pointer struct
 *@arg: a string generated from getline containing arguements
 *@argv: an array of strings generated from arg
 *@path: a string path for the current command
 *@argc: the argument count
 *@line_count: the error count
 *@error_num: the error code for exit()s
 *@linecount_flag: if on count this line of input
 *@file_name: the program filename
 *@env: linked list local copy of environ
 *@environ: custom modified copy of environ from LL env
 *@previous: the previous node
 *@alias: the alias node
 *@env_changed: on if environ was changed
 *@stat: the return status of the last exec'd command
 *@cmd_buf: address of pointer to cmd_buf, on if chaining
 *@cmd_buf_type: CMD_type ||, &&, ;
 *@read_fd: the fd from which to read line input
 *@count: the history line number count
 */
typedef struct parse
{
	char *arg;
	char **argv;
	char *path;
	int argc;
	unsigned int line_count;
	int error_num;
	int flag_count;
	char *file_name;
	list_t *env;
	list_t *previous;
	list_t *alias;
	char **environ;
	int env_changed;
	int stat;

	char **cmd_buf; /* pointer to cmd ; chain buffer, for memory mangement */
	int cmd_buf_type; /* CMD_type ||, &&, ; */
	int read_fd;
	int count;
} parse_t;

#define EN_INIT \
{NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, \
	0, 0, 0}

/**
 *struct builtin - contains a builtin string and related function
 *@type: the builtin command flag
 *@func: the function
 */
typedef struct builtin
{
	char *type;
	int (*func)(parse_t *);
} built_in_t;


/*traverse func */
int hsh(parse_t *, char **);
int find_builtin(parse_t *);
void find_cmd(parse_t *);
void fork_cmd(parse_t *);

/* parser Function */
int _cmd(parse_t *, char *);
char *dup_chars(char *, int, int);
char *find_path(parse_t *, char *, char *);

/* loop */
int traverse(char **);

/* errors function */
void er_puts(char *);
int er_putchar(char);
int _putfd(char c, int fd);
int _putsfd(char *str, int fd);

/* string functions */
int _strlen(char *);
int _strcmp(char *, char *);
char *strst_with(const char *, const char *);
char *_strcat(char *, char *);

/* copy, dup string */
char *_strcpy(char *, char *);
char *_strdup(const char *);
void _puts(char *);
int _putchar(char);

/* _exits func*/
char *_strncpy(char *, char *, int);
char *_strncat(char *, char *, int);
char *_strchr(char *, char);

/* tokenization */
char **strtow(char *, char *);
char **strtow2(char *, char);

/* mem management_realloc */
char *_memset(char *, char, unsigned int);
void ffree(char **);
void *_realloc(void *, unsigned int, unsigned int);

/* free_memory.c */
int c_free(void **);

/* str to int convert funct */
int inter(parse_t *);
int delim(char, char *);
int is_alphabet(int);
int str_to_int(char *);

/*print error  */
int err_atoi(char *);
void print_error(parse_t *, char *);
int print_d(int, int);
char *convert_number(long int, int, int);
void remove_comments(char *);

/* builtin func */
int __exit(parse_t *);
int _cd(parse_t *);
int ch_dir(parse_t *);

/* tools */
int _oldpath(parse_t *);
int c_alias(parse_t *);

/*getline*/
ssize_t get_input(parse_t *);
int _getline(parse_t *, char **, size_t *);
void sigintHandler(int);

/* getinfo*/
void clear_en(parse_t *);
void set_en(parse_t *, char **);
void free_en(parse_t *, int);

/* environ */
char *_getenv(parse_t *, const char *);
int _env(parse_t *);
int _set_env(parse_t *);
int _unset_env(parse_t *);
int p_env_list(parse_t *);

/*_getenv */
char **get_environ(parse_t *);
int _unsetenv(parse_t *, char *);
int _setenv(parse_t *, char *, char *);

/* OLDPATH */
char *get_oldpath_file(parse_t *info);
int write_oldpath(parse_t *info);
int read_oldpath(parse_t *info);
int build_oldpath_list(parse_t *info, char *buf, int linecount);
int renumber_oldpath(parse_t *info);

/* linked lists */
list_t *add_node(list_t **, const char *, int);
list_t *add_node_end(list_t **, const char *, int);
size_t print_list_str(const list_t *);
int delete_node_at_index(list_t **, unsigned int);
void free_list(list_t **);
size_t list_len(const list_t *);
char **list_to_strings(list_t *);
size_t print_list(const list_t *);
list_t *node_starts_with(list_t *, char *, char);
ssize_t get_node_index(list_t *, list_t *);

/* variables*/
int is_link(parse_t *, char *, size_t *);
void check_link(parse_t *, char *, size_t *, size_t, size_t);
int replace_alias(parse_t *);
int replace_variables(parse_t *);
int replace_string(char **, char *);

#endif
