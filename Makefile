# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/17 22:44:00 by jihoolee          #+#    #+#              #
#    Updated: 2022/06/08 20:31:28 by jihoolee         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv

CC			=	c++
CFLAGS		=	-Wall -Wextra -Werror
RM			=	rm -rf

INCLUDE_DIR	=	./include

SRCS		=	main.cpp\
				Libft.cpp\
				Connection.cpp\
				HtmlWriter.cpp\
				LocationConfig.cpp\
				Request.cpp\
				Response.cpp\
				ServerConfig.cpp\
				ServerManager.cpp\
				WebServConfig.cpp

OBJ_DIR		=	obj
OBJS		=	$(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

all : $(NAME)

claen :
	$(RM) $(OBJS)
	@$(RM) $(OBJ_DIR)

fclean : claen
	$(RM) $(NAME)

re : fclean all

test : $(OBJS)
	c++ -g3 -Wall -Wextra -Werror -fsanitize=address -o $(NAME) $^

$(OBJ_DIR)/%.o : src/%.cpp $(OBJ_DIR)
	$(CC) $(CFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR) :
	@mkdir $(OBJ_DIR)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $^

.PHONY : all clean fclean re
