/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:32:18 by msennane          #+#    #+#             */
/*   Updated: 2025/04/11 14:02:31 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	validate_map(int argc, char **argv, t_cub3d *game)
{
	t_validator	validator;

	check_arguments(argc);
	check_extension(argv[1]);
	data_processing(argv[1], game->config);
	init_variables_valid(&validator);
	found_tabs(game->config);
	analyze_map_content(game->config, &validator);
	check_map_content(&validator);
	surrounded_by_walls(game->config);
}

void	check_arguments(int argc)
{
	if (argc < 2)
	{
		printf(ERR_MISSING_MAP);
		free_memory(get_game(NULL));
		exit(EXIT_FAILURE);
	}
	else if (argc > 2)
	{
		printf(ERR_TOO_MANY_ARGS);
		free_memory(get_game(NULL));
		exit(EXIT_FAILURE);
	}
	else
		return ;
}

void	check_extension(char *map_file)
{
	char	*extension;
	int		i;
	int		len;

	extension = ".cub";
	i = 0;
	len = ft_strlen(map_file);
	len = len - 4;
	while (map_file[len] && extension[i] && map_file[len] == extension[i])
	{
		i++;
		len++;
	}
	if (extension[i] == '\0')
		return ;
	handle_error(ERR_INVALID_EXTENSION);
}

int	check_path(char *path)
{
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return (0);
	close(fd);
	return (1);
}
