/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tabs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:29:34 by msennane          #+#    #+#             */
/*   Updated: 2025/04/10 14:08:50 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	count_tabs(char *line)
{
	int	i;
	int	tabs;

	i = 0;
	tabs = 0;
	while (line[i])
	{
		if (line[i] == '\t')
			tabs++;
		i++;
	}
	return (tabs);
}

void	found_tabs(t_config *config)
{
	int		i;
	int		tabs;
	char	*to_free;

	i = 0;
	while (config->grid[i])
	{
		tabs = count_tabs(config->grid[i]);
		if (tabs > 0)
		{
			to_free = config->grid[i];
			config->grid[i] = replace_tabs(config->grid[i], tabs);
			free(to_free);
		}
		i++;
	}
}

char	*replace_tabs(char *line, int tabs)
{
	char	*replaced;
	int		i;
	int		j;
	int		x;

	i = 0;
	j = 0;
	replaced = ft_calloc(sizeof(char), ft_strlen(line) + (tabs * 4) + 1);
	while (line[i])
	{
		x = 0;
		if (line[i] == '\t')
		{
			while (x++ < 4)
				replaced[j++] = ' ';
			i++;
			continue ;
		}
		replaced[j++] = line[i++];
	}
	return (replaced);
}
