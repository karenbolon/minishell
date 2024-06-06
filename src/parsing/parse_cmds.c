/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kbolon <kbolon@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 14:43:30 by kbolon            #+#    #+#             */
/*   Updated: 2024/05/19 17:10:25 by kbolon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

//if (s != '\0' means we have stopped before end of string
void	parse_for_cmds(t_cmd **cmd, char *s)
{
	if (!s)
		return ;
	s = check_for_quotes(s);
	parse_for_pipe(&s, cmd, 0);
	update_fd(*cmd);
	while (*s != '\0' && is_whitespace(*s))
		(*s)++;
	if (*s != '\0')
		error_general("check syntax");
}

int	check_for_hanging_pipes(char *s)
{
	int	pipe_found;

	pipe_found = 0;
	while (*s)
	{
		if (*s == '|')
		{
			pipe_found = !pipe_found;
			s++;
			while (*s && is_whitespace(*s))
				s++;
			if (!*s && pipe_found)
			{
				perror("hanging pipe found, check syntax");
				return (1);
			}
		}
		s++;
	}
	return (0);
}

int	check_redirection_file_names(char *s)
{
	if (!s)
		return (0);
	while (*s)
	{
		if (*s == '<' || *s == '>')
		{
			s++;
			if (*s == '<' || *s == '>')
				s++;
			while (*s && is_whitespace(*s))
				s++;
			if (!*s || *s == '|' || *s == '<' || *s == '>')
			{
				perror("syntax error: missing file after redirection");
				return (1);
			}
		}
		s++;
	}
	return (0);
}

char	*check_for_quotes(char *s)
{
	int		in_single;
	int		in_double;
	char	*temp;

	if (!s)
		return (NULL);
	in_single = 0;
	in_double = 0;
	temp = s;
	while (*temp)
	{
		if (*temp == '\'' && !in_double)
			in_single = !in_single;
		else if (*temp == '\"' && !in_single)
			in_double = !in_double;
		else if ((*temp == '|' || *temp == ' ' || *temp == '<' || \
			*temp == '>' || *temp == '$') && (in_single || in_double))
			ft_replace(temp);
		temp++;
	}
	if (in_single || in_double)
		error_general("open quotes found, can't parse");
	return (s);
}

char	ft_replace(char *c)
{
	if (*c == '|')
		*c = '\xFD';
	else if (*c == ' ')
		*c = '\xFE';
	else if (*c == '<')
		*c = '\xD1';
	else if (*c == '>')
		*c = '\xA8';
	else if (*c == '$')
		*c = '\xAC';
	return (*c);
}
