SHELL	=	/bin/sh

NAME	=	barnes_hut

CC		?=	gcc

RM		?= 	rm -f

SRC 	= 	src/main.c							\
			src/simulation/simulation.c

CFLAGS	=	-iquote./src/include/ -Wall -Wextra -O2

OBJ	=	$(SRC:.c=.o)

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.phony: all clean fclean re
