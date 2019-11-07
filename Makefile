SHELL	?=	/bin/sh

NAME	=	barnes_hut

CC		?=	gcc

RM		?= 	rm -f

SRC 	= 	src/main.c							\
			src/simulation/simulation.c			\
			src/simulation/octree.c

CFLAGS	=	-iquote./src/include/ -Wall -Wextra -O2

LDFLAGS = -lm

OBJ	=	$(SRC:.c=.o)

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

coffee:
	@echo -ne "    (  )   (   )  )\n     ) (   )  (  (\n     ( )  (    ) )\n     _____________\n    <_____________> ___\n    |             |/ _ \\ \n    |               | | |\n    |               |_| |\n ___|             |\___/\n/    \___________/    \\ \n\_____________________/\n"

.phony: all clean fclean re coffee
