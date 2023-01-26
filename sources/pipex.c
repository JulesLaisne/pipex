/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlaisne <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/18 17:09:12 by jlaisne           #+#    #+#             */
/*   Updated: 2023/01/26 16:36:30 by jlaisne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	init_struct(t_pipex *var, char **argv, char **envp)
{
	var->arg = argv;
	var->fd1 = open(argv[1], O_RDONLY, 0777);
	if (var->fd1 == -1)
		display_error("Error, infile doesn't exist");
	var->fd2 = open(var->arg[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (var->fd2 == -1)
		display_error("Error, outfile doesn't exist");
	var->env_cmd = get_path(envp);
	if (!var->env_cmd)
		display_error("Error in malloc\n");
}

void	child_process(t_pipex *var, int *file, char **envp)
{
	char	**cmd;

	close(var->fd2);
	close(file[0]);
	if (dup2(var->fd1, STDIN_FILENO) == -1)
		display_error("Error in duplication of fd1 as Stdin\n");
	if (dup2(file[1], STDOUT_FILENO) == -1)
		display_error("Error in duplication of file[1] as Stdout\n");
	cmd = get_command(var->arg[2]);
	exec_command(var->env_cmd, cmd, envp);
	exit(0);
}
#include <errno.h>
void	parent_process(t_pipex *var, int *file, char **envp)
{
	char	**cmd;

	close(file[1]);
	close(var->fd1);
	if (dup2(file[0], STDIN_FILENO) == -1)
		display_error("Error in duplication of file[0] as Stdin");
	if (dup2(var->fd2, STDOUT_FILENO) == -1)
		display_error("Error in duplication of fd2 as Stdout");
	cmd = get_command(var->arg[3]);
	exec_command(var->env_cmd, cmd, envp);
}

int	main(int argc, char **argv, char *envp[])
{
	int		file[2];
	int		id;
	t_pipex	var;

	if (argc == 5)
	{
		init_struct(&var, argv, envp);
		if (pipe(file) == -1)
			display_error("Pipe returned an error");
		id = fork();
		if (id == -1)
			display_error("Forking unsuccessful");
		if (id != 0)
		{
			 wait(NULL);
			parent_process(&var, file, envp);
		}
		else
			child_process(&var, file, envp);
		
	}
	else
		return (ft_putstr_fd("Wrong number of arguments!\n", 2), 3);
	return (free_tab(var.env_cmd), 0);
}
