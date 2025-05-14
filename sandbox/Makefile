# Makefile for DDA Raycaster Demo

LIBMLX := MLX42
BUILD_DIR := $(LIBMLX)/build
EXEC := demo

CFLAGS := -Wall -Wextra -g
INCLUDES := -I$(LIBMLX)/include
LDFLAGS := $(BUILD_DIR)/libmlx42.a $(INCLUDES) -ldl -lglfw -pthread -lm

all: $(BUILD_DIR) $(EXEC)

$(BUILD_DIR):
	@cmake $(LIBMLX) -B $(BUILD_DIR) && make -C $(BUILD_DIR) -j4

$(EXEC): main.c | $(BUILD_DIR)
	cc $(CFLAGS) main.c $(LDFLAGS) -o $(EXEC)

clean:
	rm -f $(EXEC)
	make -C $(BUILD_DIR) clean

fclean: clean
	rm -rf $(BUILD_DIR)

re: fclean all

.PHONY: all clean fclean re
