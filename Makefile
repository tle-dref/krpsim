# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tle-dref <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/28 00:00:00 by tle-dref          #+#    #+#              #
#    Updated: 2025/08/28 00:00:00 by tle-dref         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #
NAME = krpsim

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++23 -Iinc
SRC = $(wildcard src/*.cpp)
OBJ_DIR = obj
OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)/%.o)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re