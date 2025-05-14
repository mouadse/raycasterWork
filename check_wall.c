/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_wall.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:27:53 by msennane          #+#    #+#             */
/*   Updated: 2025/05/06 11:26:34 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	has_valid_neighbors(t_config *config, int i, int j)
{
	if (config->grid[i][j + 1] == ' ' || config->grid[i][j - 1] == ' '
		|| config->grid[i + 1][j] == ' ' || config->grid[i - 1][j] == ' ')
		return (0);
	return (1);
}

static int	validate_map_element(t_config *config, int i, int j)
{
	char	c;

	c = config->grid[i][j];
	if (i == 0 || i == config->grid_rows - 1 || j == 0 || j == config->grid_cols
		- 1)
	{
		// Elements at map borders must be walls or spaces
		if (c != '1' && c != ' ' && c != '2')
			return (0);
	}
	else if (c == '0' || ft_strchr("NSWE", c))
	{
		// Playable spaces and player positions must have valid neighbors
		if (!has_valid_neighbors(config, i, j))
			return (0);
	}
	return (1);
}

static int	is_enclosed_map(t_config *config)
{
	int	i;
	int	j;

	i = 0;
	while (i < config->grid_rows)
	{
		j = 0;
		while (j < config->grid_cols && config->grid[i][j])
		{
			if (!validate_map_element(config, i, j))
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

int	is_map_enclosed(t_config *config)
{
	return (is_enclosed_map(config));
}

void	surrounded_by_walls(t_config *config)
{
	if (!is_map_enclosed(config))
		handle_error("Error: invalid map, check the walls.\n");
}
