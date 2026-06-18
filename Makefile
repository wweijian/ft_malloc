CC		= cc
CFLAGS		= -Wall -Wextra -Werror
CPPFLAGS	= -I.

HOSTTYPE	?= $(shell uname -m)_$(shell uname -s)
NAME		= libft_malloc_$(HOSTTYPE).so
LINK_NAME	= libft_malloc.so
TEST_NAME	= malloc_tests

SRC_DIR		= srcs
OBJ_DIR		= obj

SRCS		= ft_malloc.c \
		  $(SRC_DIR)/allocate_memory.c \
		  $(SRC_DIR)/utils.c
OBJS		= $(SRCS:%.c=$(OBJ_DIR)/%.o)

TEST_SRCS	= test_main.c
TEST_OBJS	= $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)

UNAME_S		:= $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SHARED_FLAGS = -dynamiclib
else
	SHARED_FLAGS = -shared
endif

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(SHARED_FLAGS) -o $@ $^
	ln -sf $@ $(LINK_NAME)

$(OBJ_DIR)/%.o: %.c ft_malloc.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@

test: $(TEST_NAME)
	./$(TEST_NAME)

$(TEST_NAME): $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(LINK_NAME) $(TEST_NAME)

re: fclean all

.PHONY: all clean fclean re test
