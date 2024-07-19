NAME = webServer
FLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRC_PATH = source/
SRC_FILES = $(wildcard $(SRC_PATH)*.cpp, *.cpp)
OBJ = obj
OBJ_FILES = $(SRC_FILES:$(SRC_PATH)%.cpp=$(OBJ)/%.o)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	c++ $(FLAGS) -I./ $^ -o $(NAME)

${OBJ}/%.o : $(SRC_PATH)%.cpp %.cpp 
	mkdir -p ${OBJ}
	c++ $(FLAGS) -c $< -o $@

run: $(NAME)
	make re
	clear && ./$(NAME)

v: valgrind

valgrind: $(NAME)
	make re
	clear
	valgrind --show-leak-kinds=all --leak-check=full --track-fds=yes ./$(NAME)

git:
	make fclean
	git add .
	git status
	@read -p "Digite a mensagem do commit: " MESSAGE; \
	git commit -m "$$MESSAGE"

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re run git valgrind v
