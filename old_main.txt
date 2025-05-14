#include "cub3d.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

// Helper function to create test files
void create_test_file(const char *filename, const char *content) {
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd != -1) {
    write(fd, content, strlen(content));
    close(fd);
  } else {
    printf("Failed to create test file: %s (errno: %d)\n", filename, errno);
  }
}

// Helper function to run a test case
void run_test(const char *test_name, const char *map_file,
              int expected_to_pass) {
  pid_t pid;
  int status;

  printf("Testing: %s\n", test_name);

  pid = fork();
  if (pid == 0) {
    // Child process
    char *args[] = {"./cub3D", (char *)map_file, NULL};

    // Redirect stderr to /dev/null to suppress error messages
    int null_fd = open("/dev/null", O_WRONLY);
    if (null_fd == -1) {
      perror("Failed to open /dev/null");
      exit(EXIT_FAILURE);
    }
    if (dup2(null_fd, STDERR_FILENO) == -1) {
      perror("Failed to redirect stderr");
      close(null_fd);
      exit(EXIT_FAILURE);
    }
    close(null_fd);

    // This is a hack: we'll replace the check_path function with a function
    // that always returns success for texture paths

    // Since we can't easily modify the check_path function at runtime,
    // we'll create the actual texture files

    // Initialize game structure
    t_cub3d *game = get_game(NULL);

    // Try to validate the map
    // Note: validate_map calls handle_error which exits on failure
    validate_map(2, args, game);

    // If we reach here, validation passed
    free_memory(game);
    exit(EXIT_SUCCESS);
  } else if (pid > 0) {
    // Parent process
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid failed");
      return;
    }

    int passed = WIFEXITED(status) && WEXITSTATUS(status) == 0;

    if (passed == expected_to_pass) {
      printf("  Result: %sPASSED%s\n", GREEN, RESET);
    } else {
      printf("  Result: %sFAILED%s (Expected: %s, got: %s)\n", RED, RESET,
             expected_to_pass ? "pass" : "fail", passed ? "pass" : "fail");
    }
  } else {
    // Fork failed
    perror("fork failed");
  }
}

// Clean up test files
void cleanup_test_files() {
  const char *files[] = {"valid.cub",
                         "no_extension",
                         "wrong.txt",
                         "missing_no.cub",
                         "missing_so.cub",
                         "missing_we.cub",
                         "missing_ea.cub",
                         "duplicate_no.cub",
                         "duplicate_so.cub",
                         "dup_textures.cub",
                         "missing_floor.cub",
                         "missing_ceiling.cub",
                         "invalid_rgb.cub",
                         "rgb_out_of_range.cub",
                         "duplicate_rgb.cub",
                         "no_map.cub",
                         "invalid_chars.cub",
                         "map_not_surrounded.cub",
                         "multiple_players.cub",
                         "no_player.cub",
                         "map_empty_lines.cub",
                         "map_with_tabs.cub",
                         "incorrect_order.cub",
                         NULL};

  for (int i = 0; files[i] != NULL; i++) {
    unlink(files[i]);
  }
}

// Create texture directory and files for testing
void setup_test_environment() {
  // Create texture directory
  if (mkdir("texture", 0755) == -1 && errno != EEXIST) {
    perror("Failed to create texture directory");
    return;
  }

  // Create texture files
  create_test_file("texture/north.xpm", "Dummy XPM file");
  create_test_file("texture/south.xpm", "Dummy XPM file");
  create_test_file("texture/west.xpm", "Dummy XPM file");
  create_test_file("texture/east.xpm", "Dummy XPM file");
}

int main(void) {
  printf("=== CUB3D PARSER TEST SUITE ===\n\n");

  // Setup test environment with actual texture files
  setup_test_environment();

  // Create all test files

  // Valid map - should pass
  create_test_file("valid.cub", "NO ./texture/north.xpm\n"
                                "SO ./texture/south.xpm\n"
                                "WE ./texture/west.xpm\n"
                                "EA ./texture/east.xpm\n"
                                "F 220,100,0\n"
                                "C 225,30,0\n"
                                "\n"
                                "1111111111111\n"
                                "1000000000001\n"
                                "1000000000001\n"
                                "1000000N00001\n"
                                "1000000000001\n"
                                "1111111111111\n");

  // Invalid extension tests
  create_test_file("no_extension", "NO ./texture/north.xpm\n"
                                   "SO ./texture/south.xpm\n"
                                   "WE ./texture/west.xpm\n"
                                   "EA ./texture/east.xpm\n"
                                   "F 220,100,0\n"
                                   "C 225,30,0\n"
                                   "\n"
                                   "1111111111111\n"
                                   "1000000N00001\n"
                                   "1111111111111\n");

  create_test_file("wrong.txt", "NO ./texture/north.xpm\n"
                                "SO ./texture/south.xpm\n"
                                "WE ./texture/west.xpm\n"
                                "EA ./texture/east.xpm\n"
                                "F 220,100,0\n"
                                "C 225,30,0\n"
                                "\n"
                                "1111111111111\n"
                                "1000000N00001\n"
                                "1111111111111\n");

  // Missing texture tests
  create_test_file("missing_no.cub", "SO ./texture/south.xpm\n"
                                     "WE ./texture/west.xpm\n"
                                     "EA ./texture/east.xpm\n"
                                     "F 220,100,0\n"
                                     "C 225,30,0\n"
                                     "\n"
                                     "1111111111111\n"
                                     "1000000000001\n"
                                     "1000000N00001\n"
                                     "1111111111111\n");

  create_test_file("missing_so.cub", "NO ./texture/north.xpm\n"
                                     "WE ./texture/west.xpm\n"
                                     "EA ./texture/east.xpm\n"
                                     "F 220,100,0\n"
                                     "C 225,30,0\n"
                                     "\n"
                                     "1111111111111\n"
                                     "1000000N00001\n"
                                     "1111111111111\n");

  create_test_file("missing_we.cub", "NO ./texture/north.xpm\n"
                                     "SO ./texture/south.xpm\n"
                                     "EA ./texture/east.xpm\n"
                                     "F 220,100,0\n"
                                     "C 225,30,0\n"
                                     "\n"
                                     "1111111111111\n"
                                     "1000000N00001\n"
                                     "1111111111111\n");

  create_test_file("missing_ea.cub", "NO ./texture/north.xpm\n"
                                     "SO ./texture/south.xpm\n"
                                     "WE ./texture/west.xpm\n"
                                     "F 220,100,0\n"
                                     "C 225,30,0\n"
                                     "\n"
                                     "1111111111111\n"
                                     "1000000N00001\n"
                                     "1111111111111\n");

  // Duplicate texture tests
  create_test_file("duplicate_no.cub", "NO ./texture/north.xpm\n"
                                       "NO ./texture/north.xpm\n"
                                       "SO ./texture/south.xpm\n"
                                       "WE ./texture/west.xpm\n"
                                       "EA ./texture/east.xpm\n"
                                       "F 220,100,0\n"
                                       "C 225,30,0\n"
                                       "\n"
                                       "1111111111111\n"
                                       "1000000N00001\n"
                                       "1111111111111\n");

  create_test_file("duplicate_so.cub", "NO ./texture/north.xpm\n"
                                       "SO ./texture/south.xpm\n"
                                       "SO ./texture/south.xpm\n"
                                       "WE ./texture/west.xpm\n"
                                       "EA ./texture/east.xpm\n"
                                       "F 220,100,0\n"
                                       "C 225,30,0\n"
                                       "\n"
                                       "1111111111111\n"
                                       "1000000N00001\n"
                                       "1111111111111\n");

  create_test_file("dup_textures.cub", "NO ./texture/north.xpm\n"
                                       "SO ./texture/south.xpm\n"
                                       "WE ./texture/west.xpm\n"
                                       "EA ./texture/east.xpm\n"
                                       "EA ./texture/east.xpm\n"
                                       "F 220,100,0\n"
                                       "C 225,30,0\n"
                                       "\n"
                                       "1111111111111\n"
                                       "1000000N00001\n"
                                       "1111111111111\n");

  // Missing RGB color tests
  create_test_file("missing_floor.cub", "NO ./texture/north.xpm\n"
                                        "SO ./texture/south.xpm\n"
                                        "WE ./texture/west.xpm\n"
                                        "EA ./texture/east.xpm\n"
                                        "C 225,30,0\n"
                                        "\n"
                                        "1111111111111\n"
                                        "1000000N00001\n"
                                        "1111111111111\n");

  create_test_file("missing_ceiling.cub", "NO ./texture/north.xpm\n"
                                          "SO ./texture/south.xpm\n"
                                          "WE ./texture/west.xpm\n"
                                          "EA ./texture/east.xpm\n"
                                          "F 220,100,0\n"
                                          "\n"
                                          "1111111111111\n"
                                          "1000000N00001\n"
                                          "1111111111111\n");

  // Invalid RGB color format
  create_test_file("invalid_rgb.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,abc\n" // Non-numeric RGB
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000N00001\n"
                   "1111111111111\n");

  create_test_file("rgb_out_of_range.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,300,0\n" // Value > 255
                   "\n"
                   "1111111111111\n"
                   "1000000N00001\n"
                   "1111111111111\n");

  // Duplicate RGB colors
  create_test_file("duplicate_rgb.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "F 100,100,100\n" // Duplicate floor
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000N00001\n"
                   "1111111111111\n");

  // Missing map
  create_test_file("no_map.cub", "NO ./texture/north.xpm\n"
                                 "SO ./texture/south.xpm\n"
                                 "WE ./texture/west.xpm\n"
                                 "EA ./texture/east.xpm\n"
                                 "F 220,100,0\n"
                                 "C 225,30,0\n");

  // Invalid map characters
  create_test_file("invalid_chars.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000000001\n"
                   "10000X0N00001\n" // X is invalid
                   "1000000000001\n"
                   "1111111111111\n");

  // Map not surrounded by walls
  create_test_file("map_not_surrounded.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000000001\n"
                   "10000N0000001\n"
                   "1000000000000\n" // Open wall
                   "1111111111111\n");

  // Multiple players
  create_test_file("multiple_players.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000000001\n"
                   "10000N0000S01\n" // Two players
                   "1000000000001\n"
                   "1111111111111\n");

  // No player
  create_test_file("no_player.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000000001\n"
                   "1000000000001\n" // No player
                   "1000000000001\n"
                   "1111111111111\n");

  // Map with empty lines within it
  create_test_file("map_empty_lines.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1000000000001\n"
                   "\n" // Empty line in map
                   "10000N0000001\n"
                   "1000000000001\n"
                   "1111111111111\n");

  // Map with tabs
  create_test_file("map_with_tabs.cub",
                   "NO ./texture/north.xpm\n"
                   "SO ./texture/south.xpm\n"
                   "WE ./texture/west.xpm\n"
                   "EA ./texture/east.xpm\n"
                   "F 220,100,0\n"
                   "C 225,30,0\n"
                   "\n"
                   "1111111111111\n"
                   "1\t000000001\n" // Tab in map
                   "10000N0000001\n"
                   "1000000000001\n"
                   "1111111111111\n");

  // Incorrect order (map before textures)
  create_test_file("incorrect_order.cub", "1111111111111\n"
                                          "1000000000001\n"
                                          "10000N0000001\n"
                                          "1000000000001\n"
                                          "1111111111111\n"
                                          "\n"
                                          "NO ./texture/north.xpm\n"
                                          "SO ./texture/south.xpm\n"
                                          "WE ./texture/west.xpm\n"
                                          "EA ./texture/east.xpm\n"
                                          "F 220,100,0\n"
                                          "C 225,30,0\n");

  // Run all tests
  run_test("Valid map", "valid.cub", 1);
  run_test("No file extension", "no_extension", 0);
  run_test("Wrong file extension", "wrong.txt", 0);
  run_test("Missing NO texture", "missing_no.cub", 0);
  run_test("Missing SO texture", "missing_so.cub", 0);
  run_test("Missing WE texture", "missing_we.cub", 0);
  run_test("Missing EA texture", "missing_ea.cub", 0);
  run_test("Duplicate NO texture", "duplicate_no.cub", 0);
  run_test("Duplicate SO texture", "duplicate_so.cub", 0);
  run_test("Duplicate EA texture", "dup_textures.cub", 0);
  run_test("Missing floor color", "missing_floor.cub", 0);
  run_test("Missing ceiling color", "missing_ceiling.cub", 0);
  run_test("Invalid RGB format", "invalid_rgb.cub", 0);
  run_test("RGB out of range", "rgb_out_of_range.cub", 0);
  run_test("Duplicate RGB", "duplicate_rgb.cub", 0);
  run_test("No map", "no_map.cub", 0);
  run_test("Invalid characters in map", "invalid_chars.cub", 0);
  run_test("Map not surrounded by walls", "map_not_surrounded.cub", 0);
  run_test("Multiple players", "multiple_players.cub", 0);
  run_test("No player", "no_player.cub", 0);
  run_test("Map with empty lines", "map_empty_lines.cub", 0);
  run_test("Map with tabs", "map_with_tabs.cub",
           1); // Should pass since tabs are handled
  run_test("Incorrect order", "incorrect_order.cub", 0);

  printf("\n=== TEST SUITE COMPLETE ===\n");

  // Clean up test files
  cleanup_test_files();

  // Clean up texture files
  unlink("texture/north.xpm");
  unlink("texture/south.xpm");
  unlink("texture/west.xpm");
  unlink("texture/east.xpm");
  rmdir("texture");

  return 0;
}
