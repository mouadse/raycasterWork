/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:30:07 by msennane          #+#    #+#             */
/*   Updated: 2025/04/10 12:38:37 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
		|| c == '\r');
}

void	init_variables_valid(t_validator *validator)
{
	validator->invalid_char_count = 0;
	validator->player_count = 0;
}

/**
 * Singleton-style function to get/initialize the game instance
 * If game is NULL, creates and returns a static instance
 * If game is not NULL, updates the static instance
 */
t_cub3d	*get_game(t_cub3d *game)
{
	static t_cub3d	*instance = NULL;

	if (game != NULL)
		instance = game;
	else if (instance == NULL)
	{
		instance = (t_cub3d *)malloc(sizeof(t_cub3d));
		if (!instance)
			exit(EXIT_FAILURE);
		instance->config = (t_config *)malloc(sizeof(t_config));
		if (!instance->config)
		{
			free(instance);
			exit(EXIT_FAILURE);
		}
		/* Initialize config members */
		instance->config->grid = NULL;
		instance->config->no_texture_path = NULL;
		instance->config->so_texture_path = NULL;
		instance->config->we_texture_path = NULL;
		instance->config->ea_texture_path = NULL;
		instance->config->floor_rgb = 0;
		instance->config->ceiling_rgb = 0;
		instance->config->textures_ready = false;
		instance->config->loaded_textures = 0;
		instance->config->grid_rows = 0;
		instance->config->grid_cols = 0;
		instance->config->character_orientation = '\0';
		instance->config->character_pos_x = -1;
		instance->config->character_pos_y = -1;
	}
	return (instance);
}

/**
 * Error handling function
 * Displays error message and exits program
 */
void	handle_error(char *message)
{
	if (message)
		write(STDERR_FILENO, message, ft_strlen(message));
	else
		write(STDERR_FILENO, "Error: unknown error occurred\n", 30);
	free_memory(get_game(NULL));
	exit(EXIT_FAILURE);
}

/**
 * Safely open a file and return the file descriptor
 * If the file cannot be opened, exit with error
 */
int	open_file(char *map_file)
{
	int		fd;
	char	error_msg[100] = "Error: could not open file: ";

	fd = open(map_file, O_RDONLY);
	if (fd == -1)
	{
		ft_strlcat(error_msg, map_file, sizeof(error_msg));
		ft_strlcat(error_msg, "\n", sizeof(error_msg));
		handle_error(error_msg);
	}
	return (fd);
}

/**
 * Free a 2D array (matrix)
 * Safely handles NULL pointers
 */
void	ft_free_matrix(char **matrix)
{
	int	i;

	if (!matrix)
		return ;
	i = 0;
	while (matrix[i])
	{
		free(matrix[i]);
		i++;
	}
	free(matrix);
}

/**
 * Free all allocated memory in the game structure
 */
void	free_memory(t_cub3d *game)
{
	if (!game)
		return ;
	if (game->config)
	{
		if (game->config->grid)
			ft_free_matrix(game->config->grid);
		free(game->config->no_texture_path);
		free(game->config->so_texture_path);
		free(game->config->we_texture_path);
		free(game->config->ea_texture_path);
		free(game->config);
	}
	/* Don't free game itself if it's the static instance */
	if (game != get_game(NULL))
		free(game);
}
