# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/02 15:36:34 by eala-lah          #+#    #+#              #
#    Updated: 2025/06/19 15:07:05 by eala-lah         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= philo
INCS		= -I ./inc/
TESTER_SH	= test_philo.sh
TESTER_URL	= https://raw.githubusercontent.com/erkkaervice/area51/main/test_philo.sh

SRC_DIR		= src/
SRC		= \
	actions.c \
	exit.c \
	init.c \
	simulation.c \
	main.c \

OBJ_DIR		= obj/
OBJS		= $(addprefix $(OBJ_DIR), $(SRC:.c=.o))

CC		= cc
CFLAGS		= -Wall -Wextra -Werror $(INCS) -pthread

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR) 2> /dev/null || { echo "Failed to create object directory." >&2; exit 1; }

$(OBJ_DIR)%.o: $(SRC_DIR)%.c inc/philo.h
	@$(CC) $(CFLAGS) -c $< -o $@ 2> /dev/null || { echo "Failed to compile $<." >&2; exit 1; }

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) 2> /dev/null || { echo "Failed to create executable $(NAME)." >&2; exit 1; }

test:
	@curl -s -L $(TESTER_URL) -o $(TESTER_SH) || { echo "Failed to download test_philo.sh"; exit 1; }
	@chmod +x $(TESTER_SH) 2> /dev/null || { echo "Failed to make tester executable." >&2; exit 1; }
	@./$(TESTER_SH)

bonus: all

clean:
	@rm -rf $(OBJ_DIR) 2> /dev/null || { echo "Failed to clean object files." >&2; exit 1; }

fclean: clean
	@rm -f $(NAME) 2> /dev/null || { echo "Failed to remove executable." >&2; exit 1; }
	@rm -f $(TESTER_SH) 2> /dev/null || { if [ -f "$(TESTER_SH)" ]; then echo "Failed to remove test_philo.sh." >&2; exit 1; fi; }
	@rm -rf logs 2> /dev/null || { if [ -d "logs" ]; then echo "Failed to remove logs directory." >&2; exit 1; fi; }

re: fclean all

.PHONY: all clean fclean re bonus test
