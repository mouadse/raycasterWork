/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:28:43 by msennane          #+#    #+#             */
/*   Updated: 2025/04/10 12:28:50 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	is_not_bar_n(bool *is_map, bool *map_ended, char *temp, int size);

void	count_map_size(t_config *config, char *temp, int fd)
{
	int	map_size;

	map_size = 0;
	temp = get_next_line(fd);
	while (temp)
	{
		map_size++;
		free(temp);
		temp = get_next_line(fd);
	}
	close(fd);
	if (map_size == 0)
		handle_error("Error: missing map.\n");
	config->grid = ft_calloc(sizeof(char *), (map_size + 1));
	if (!config->grid)
		handle_error("Error: ft_calloc.\n");
}

void	read_and_copy_map_content(char *temp, int fd)
{
	int		size;
	bool	is_map;
	bool	map_ended;

	size = 0;
	is_map = false;
	map_ended = false;
	temp = get_next_line(fd);
	while (temp)
	{
		if (temp[0] == '\n')
		{
			if (is_map && !map_ended)
				map_ended = true;
		}
		else
			is_not_bar_n(&is_map, &map_ended, temp, size);
		size++;
		free(temp);
		temp = get_next_line(fd);
	}
	close(fd);
}

static void	is_not_bar_n(bool *is_map, bool *map_ended, char *temp, int size)
{
	t_cub3d		*game;
	static int	i;

	game = get_game(NULL);
	if (*map_ended)
	{
		free(temp);
		handle_error("Error: map invalid.\n");
	}
	if (size >= game->config->loaded_textures)
	{
		game->config->grid[i] = ft_strdup(temp);
		i++;
		*is_map = true;
	}
}

void	analyze_map_content(t_config *config, t_validator *validator)
{
	int	i;
	int	j;

	i = 0;
	while (config->grid[i])
	{
		j = 0;
		while (config->grid[i][j])
		{
			if (check_invalid_char(config->grid[i][j]) == 0)
				validator->invalid_char_count++;
			else if (config->grid[i][j] == 'N' || config->grid[i][j] == 'S'
				|| config->grid[i][j] == 'E' || config->grid[i][j] == 'W')
			{
				validator->player_count++;
				config->character_orientation = config->grid[i][j];
				config->character_pos_y = i;
				config->character_pos_x = j;
			}
			j++;
		}
		i++;
	}
}
