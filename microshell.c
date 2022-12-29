#include <libc.h>

typedef struct s_micro
{
	char **av;
	char **env;
	int tmp;
	int fd[0];
} t_micro;

int ft_errors(char *s1, char *s2)
{
	int i = -1;
	while (s1[++i])
		write(2, &s1[i], 1);
	if (s2)
	{
		i = -1;
		while (s2[++i])
			write(2, &s2[i], 1);
	}
	return(write(2, "\n", 1), 1);
}

int count_arg(t_micro *data)
{
	int i =0;
	char **av = data->av;
	while(av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
		i++;
	return i;
}
int ft_exec_command(t_micro *data, int i)
{
	data->av[i] = NULL;
	dup2(data->tmp, 0);
	close(data->tmp);
	if(!execve(data->av[0], data->av, data->env))
		ft_errors("fatal :error", NULL);
	return (ft_errors("error: cannot execute ", data->av[0]),1);

}

int microshell(t_micro *data)
{
	int i =0;
	while (data->av[i] && data->av[i + 1])
	{
		data->av = &data->av[i +1];
		i = count_arg(data);
		if (!strcmp(data->av[0], "cd"))
		{
			if (i != 2)
				ft_errors("error: cd: bad arguments", NULL);
			else if (chdir(data->av[1]) != 0)
				ft_errors("error: cd: cannot ", data->av[1]);
		}
		else if (i != 0 && (data->av[i] == NULL || !strcmp(data->av[i], ";")))
		{
			
			if(!fork())
			{
				if (ft_exec_command(data, i))
					return 1;
			}
			else
				{
					close(data->tmp);
					while (waitpid(-1,NULL, 0)!= -1);
					
				}
				data->tmp = dup(0);

		}
		else if (i !=0 && !strcmp(data->av[i], "|"))
		{
			pipe(data->fd);
			if(!fork())
			{
				dup2(data->fd[1], 1);
				close(data->fd[1]);
				close(data->fd[0]);
				if (ft_exec_command(data, i))
					return 1;
			}else
			{
				close(data->fd[1]);
				close(data->tmp);
				data->tmp = data->fd[0];
			}
		}
	}
	close(data->tmp);
	return 1;
}

int main(int ac,char **av, char **env)
{
	t_micro micro;
	(void)ac;
	micro.tmp = dup(0);
	micro.av = av;
	micro.env = env;
	microshell(&micro);
	return 0;
}