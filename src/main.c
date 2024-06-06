/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chbuerge <chbuerge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/05 15:54:42 by kbolon            #+#    #+#             */
/*   Updated: 2024/05/21 15:10:59 by chbuerge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
** global variable for signal communication, needs to be init in main
*/
int	g_signal;

void	ft_execute(char *line, t_cmd **list,
		t_minishell *minishell_struct, int *exit_status)
{
	parse_for_cmds(list, line);
	parse_cmds_for_expansions(list, minishell_struct->env_list, exit_status);
	restore_pipes_and_spaces(*list);
	*exit_status = ft_executor(*list, minishell_struct);
	dup2(minishell_struct->og_stdin, STDIN_FILENO);
	dup2(minishell_struct->og_stdout, STDOUT_FILENO);
}

char	*read_command(t_cmd *list,
		t_minishell *minishell_struct, int *exit_status)
{
	char	*line;

	while (1)
	{
		ft_init_signals();
		line = readline("minishell: ");
		if (!line)
		{
			printf("exit\n");
			ft_exit_free(minishell_struct, NULL, 0);
		}
		while (*line != '\0' && is_whitespace(*line))
			line++;
		if (*line == '\0')
			break ;
		else if (check_for_hanging_pipes(line) == 0)
		{
			add_history(line);
			ft_execute(line, &list, minishell_struct, exit_status);
			list = NULL;
			return (line);
		}
	}
	return (line);
}

void	handle_exit(t_env *env_list, char *line)
{
	rl_clear_history();
	free(line);
	ft_free_env_list(&env_list);
}

t_minishell	*init_minishell(char **env)
{
	t_minishell	*minishell_struct;

	minishell_struct = malloc(sizeof(t_minishell));
	minishell_struct->og_stdin = dup(STDIN_FILENO);
	minishell_struct->og_stdout = dup(STDOUT_FILENO);
	minishell_struct->env_list = fill_env_struct(env);
	return (minishell_struct);
}

int	main(int argc, char **argv, char **env)
{
	static char	*line;
	t_cmd		*list;
	int			exit_status;
	t_minishell	*minishell_struct;

	(void)argv;
	g_signal = 0;
	line = NULL;
	list = NULL;
	exit_status = 0;
	if (argc != 1)
	{
		write(STDERR_FILENO, "invalid arguments: ambiguous redirect\n", 38);
		exit(1);
	}
	if (!env)
		return (0);
	minishell_struct = init_minishell(env);
	while (1)
		line = read_command(list, minishell_struct, &exit_status);
	handle_exit(minishell_struct->env_list, line);
	close(minishell_struct->og_stdin);
	close(minishell_struct->og_stdout);
	return (exit_status);
}
