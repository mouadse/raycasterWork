#include "cub3d.h"

int	main(void)
{
	t_cub3d		game;
	t_config	config;

	// simple hardcoded map
	char *grid[] = {
		"111111111111111111111111",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100010000000000100000001",
		"100000000000000000000001",
		"100000000000000000100001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000010000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000100000000000000001",
		"100000000000100000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"100000000000000000000001",
		"111111111111111111111111",
		NULL
	};
	// configure map and colors
	config.grid = grid;
	config.grid_rows = 5;
	config.grid_cols = 6;
	config.no_texture_path = "./minecraft1.xpm";
	config.so_texture_path = "./minecraft2.xpm";
	config.we_texture_path = "./minecraft3.xpm";
	config.ea_texture_path = "./minecraft4.xpm";
	config.floor_rgb = convert_rgb(100, 100, 100);
	config.ceiling_rgb = convert_rgb(200, 200, 200);
	game.config = &config;
	// initialize MLX and image
	game.mlx = mlx_init();
	game.win = mlx_new_window(game.mlx, SCREEN_WIDTH, SCREEN_HEIGHT,
			"Cub3D Test");
	init_image(&game, SCREEN_WIDTH, SCREEN_HEIGHT);
	// initial player position and camera
	game.player_pos.x = 2.5f;
	game.player_pos.y = 2.5f;
	game.player_dir.x = -1.0f;
	game.player_dir.y = 0.0f;
	game.camera_plane.x = 0.0f;
	game.camera_plane.y = 0.66f;
	memset(&game.keys, 0, sizeof(game.keys));
	// load textures and render initial scene
	load_textures(&game);
	// Draw initial frame
	draw_background2(&game);
	draw_rays(&game);
	mlx_put_image_to_window(game.mlx, game.win, game.img.ptr, 0, 0);
	// enter main loop
	mlx_hook(game.win, 2, 1L << 0, key_press, &game);
	mlx_hook(game.win, 3, 1L << 1, key_release, &game);
	mlx_hook(game.win, 17, 0, close_window, &game);
	mlx_loop_hook(game.mlx, game_loop, &game);
	mlx_loop(game.mlx);
	return (0);
}
