# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bylee <bylee@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/17 22:44:00 by jihoolee          #+#    #+#              #
#    Updated: 2022/06/18 17:35:17 by bylee            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv

CC			=	c++
CFLAGS		=	-Wall -Wextra -Werror
RM			=	rm -rf

INCLUDE_DIR	=	./include

SRC			=	main.cpp\
				Libft.cpp\
				Connection.cpp\
				HtmlWriter.cpp\
				LocationConfig.cpp\
				Request.cpp\
				Response.cpp\
				ServerConfig.cpp\
				ServerManager.cpp\
				WebServConfig.cpp
SRC_DIR		=	src/
SRCS		=	$(addprefix $(SRC_DIR), $(SRC))

OBJ_DIR		=	obj/
OBJS		=	$(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

all : $(NAME)

$(OBJ_DIR) :
	@mkdir $(OBJ_DIR)

$(OBJS):		| $(OBJ_DIR)

${NAME} : ${OBJS}
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} ${OBJS} -o ${NAME}

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	$(CC) $(CFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

claen :
	$(RM) $(OBJS)
	$(RM) $(OBJ_DIR)

fclean : claen
	$(RM) $(NAME)

re : fclean all

test : $(OBJS)
	c++ -g3 -Wall -Wextra -Werror -fsanitize=address -o $(NAME) $^

.PHONY : all clean fclean re
