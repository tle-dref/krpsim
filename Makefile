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
CXXFLAGS = -Wall -Wextra -Werror -std=c++23
SRC = $(wildcard src/*.cpp)
OBJDIR = obj
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

$(OBJDIR):
	mkdir -p $(OBJDIR)


all: $(NAME)


$(NAME): $(OBJDIR) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)


clean:
	rm -rf $(OBJDIR)


fclean: clean
	rm -f $(NAME)


re: fclean all


.PHONY: all clean fclean re

# Compilation des .cpp en .o dans obj/
$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
