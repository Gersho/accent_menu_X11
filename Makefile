SRCS		= src/main.cpp src/InputRemap.cpp src/XServManager.cpp

OBJS			= $(SRCS:.cpp=.o)

NAME			= accent_menu_X11
CC				= clang++
RM				= rm -f
CFLAGS			= -Wall -Wextra -Werror

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CC) $(CFLAGS) $(SRCS) -lstdc++ -lX11 -o $(NAME)

run:			all
				./$(NAME) accent_menu_X11.exemple.conf

clean:
				$(RM) $(OBJS)

fclean:			clean
				$(RM) $(NAME)

re:			fclean all

.PHONY:		re fclean clean all