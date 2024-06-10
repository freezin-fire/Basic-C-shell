#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*** Defines ***/

#define GLSH_RL_BUFSIZE 1024
#define GLSH_TOK_BUFSIZE 64
#define GLSH_TOK_DELIM " \t\r\n\a"

/*** In-Built Function Declarations***/
int glsh_cd(char **args);
int glsh_help(char **args);
int glsh_exit(char **args);

// List of commands with functions
char *builtin_str[] = {
	"cd",
	"help",
	"exit"};

int (*builtin_func[])(char **) = {&glsh_cd, &glsh_help, &glsh_exit};

int glsh_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

/*** Function implementations ***/
int glsh_cd(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "gsh: expected argument to\"cd\"\n");
	}
	else
	{
		if (chdir(args[1]) != 0)
		{
			perror("lsh");
		}
	}
	return 1;
}

int glsh_help(char **args)
{
	int i;
	printf("FreezinFire's GLSH\n\n");
	printf("Type program names and arguments, and hit enter.\n\n");
	printf("The following are the bult-in commands\n");

	for (i = 0; i < glsh_num_builtins(); i++)
	{
		printf(" %s\n", builtin_str[i]);
	}

	printf("\nUse the man command for information on other programs\n");
	return 1;
}

int glsh_exit(char **args)
{
	return 0;
}

/*** Old C style read line implimentation ***/
// char *glsh_read_line(){
// 	int bufsize = GLSH_RL_BUFSIZE;
// 	int pos = 0;
// 	char *buffer = malloc(sizeof(char) * bufsize);
// 	int c;
// 	if (!buffer){
// 		fprintf(stderr, "gsh: allocation error\n");
// 		exit(EXIT_FAILURE);
// 	}

// 	while (1){
// 		// Read chararcter
// 		c = getchar();
// 		// IF at EOF, replace with null char and return
// 		if (c == EOF || c == '\n'){
// 			buffer[pos] = '\0';
// 			return buffer;
// 		} else {
// 			buffer[pos] = c;
// 		}
// 		pos++;

// 		// If buffer exceeded, reallocate
// 		if (pos >= bufsize){
// 			bufsize += GLSH_RL_BUFSIZE;
// 			buffer = realloc(buffer, bufsize);
// 			if (!buffer){
// 				fprintf(stderr, "gsh: allocation error\n");
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 	}
// }

/*** Modern read line implimentation ***/
char *glsh_read_line()
{
	char *line = NULL;
	ssize_t bufsize = 0; // Buffer allocation

	if (getline(&line, &bufsize, stdin) == -1)
	{
		if (feof(stdin))
		{
			exit(EXIT_SUCCESS); // Reached EOF
		}
		else
		{
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}

	return line;
}

char **glsh_split_line(char *line)
{
	int bufsize = GLSH_TOK_BUFSIZE, pos = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		fprintf(stderr, "gsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, GLSH_TOK_DELIM);
	while (token != NULL)
	{
		tokens[pos] = token;
		pos++;

		if (pos >= bufsize)
		{
			bufsize += GLSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, GLSH_TOK_DELIM);
	}
	tokens[pos] = NULL;
	return tokens;
}

int glsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		// Child process
		if (execvp(args[0], args) == -1)
		{
			perror("gsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		// Error forking
		perror("gsh");
	}
	else
	{
		// Parent process
		do
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int glsh_execute(char **args)
{
	int i;

	if (args[0] == NULL)
	{
		return 1; // Empty command recieved
	}

	for (i = 0; i < glsh_num_builtins(); i++)
	{
		if (strcmp(args[0], builtin_str[i]) == 0)
		{
			return (*builtin_func[i])(args);
		}
	}

	return glsh_launch(args);
}

void glsh_loop()
{
	char *line;
	char **args;
	int status;

	do
	{
		printf("> ");
		line = glsh_read_line();
		args = glsh_split_line(line);
		status = glsh_execute(args);

		free(line);
		free(args);
	} while (status);
}

/*** Main ***/

int main(int argc, char **argv)
{
	// Load config files

	// Run command loop
	glsh_loop();

	// Perform shutdown/cleanup
	return EXIT_SUCCESS;
}
