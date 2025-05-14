/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:29:10 by msennane          #+#    #+#             */
/*   Updated: 2025/04/10 12:29:17 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	get_max_lines(t_config *config)
{
	int	lines;

	lines = 0;
	while (config->grid[lines])
		lines++;
	config->grid_rows = lines;
}

void	get_max_columns(t_config *config)
{
	int	max_columns;
	int	columns;
	int	i;

	max_columns = 0;
	i = 0;
	while (config->grid[i])
	{
		columns = ft_strlen(config->grid[i]) - 1;
		if (columns > max_columns)
			max_columns = columns;
		i++;
	}
	config->grid_cols = max_columns;
}

void	check_map_content(t_validator *validator)
{
	if (validator->invalid_char_count != 0)
		handle_error(ERR_INVALID_CHARACTER);
	else if (validator->player_count != 1)
		handle_error(ERR_PLAYER_COUNT);
}

int	check_invalid_char(char c)
{
	if (c == ' ' || c == 'N' || c == 'S' || c == 'E' || c == 'W' || c == '0'
		|| c == '1' || c == '\0' || c == '\n' || (c >= 9 && c <= 13))
		return (1);
	return (0);
}
