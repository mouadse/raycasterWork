# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/10 12:31:32 by msennane          #+#    #+#              #
#    Updated: 2025/05/07 15:50:01 by msennane         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = cub3d

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra #-fsanitize=address,leak,undefined -O3 -g3
MLXFLAGS = -lmlx -lXext -lX11 -lm

# Directories
LIBFT_DIR = libft
GNL_DIR = get_next_line

# Source files
SRCS = main.c validation.c check_path.c data_processing.c \
	check_rgb.c map.c map_utils.c tabs.c utils.c check_wall.c \
	camera.c event_hooks.c raycasting.c raycasting_utils.c \
	render_background.c textures.c vector_ops.c wall_rendering.c \
	wall_utils.c player_movement.c my_mlx_utils.c

# Get_next_line files
GNL_SRCS = $(GNL_DIR)/get_next_line.c $(GNL_DIR)/get_next_line_utils.c

# Object files
OBJS = $(SRCS:.c=.o)
GNL_OBJS = $(GNL_SRCS:.c=.o)

# Libraries
LIBFT = $(LIBFT_DIR)/libft.a

# Rules
all: $(NAME)

$(NAME): $(OBJS) $(GNL_OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(GNL_OBJS) $(LIBFT) $(MLXFLAGS) -o $(NAME)
	@echo "\033[32mCompilation successful! '$(NAME)' is ready.\033[0m"

$(LIBFT):
	@echo "\033[33mCompiling Libft...\033[0m"
	@make -C $(LIBFT_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -I. -I$(LIBFT_DIR) -I$(GNL_DIR) -c $< -o $@

clean:
	@echo "\033[33mCleaning object files...\033[0m"
	@rm -f $(OBJS) $(GNL_OBJS)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	@echo "\033[33mCleaning executable...\033[0m"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
