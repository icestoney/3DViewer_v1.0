#================================== COMPILER ==================================#
PROJECT_NAME = S21_3D_Viewer
CC = gcc
#==============================================================================#


#=============================== DIRECTORY NAMES ==============================#
OBJ_DIR = ./obj
MODULES_DIR = ./modules
GUI_DIR = ./gui
TEST_DIR = ./tests
REPORT_DIR = ./report
DVI_DIR = ./../docs
DIST_DIR = ./../dist
BUILD = ./../build
PROGRAM = S21_3D_ViewerAPP
#==============================================================================#


#==================================== FLAGS ===================================#
# MAIN FLAGS FOR COMPILER
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c11 -g

# CHECK LIBRARY FOR LINKING
LDFLAGS = -lcheck -lsubunit -lm

# FLAGS FOR COVERING MODULES
GCOV_FLAGS = -fprofile-arcs -ftest-coverage -lgcov $(LDFLAGS)

# FLAGS FOR VALGRING TEST
VAL = --tool=memcheck --leak-check=yes

# FLAGS FOR CPPCHECK TEST
CPPCHECK_CFLAGS = --enable=all --suppress=unusedFunction --suppress=missingIncludeSystem 
CPPCHECK_CPPFLAGS = --enable=all --suppress=unusedFunction --suppress=missingInclude --language=c++
#==============================================================================#


#================================ TARGET NAMES ================================#
LIBRARY = $(PROJECT_NAME).a
TEST = test
GCOV = gcov_report
#==============================================================================#


#====================== LIST OF FILE AND DIRS IN MODULES ======================#
MODULES_DIRS = $(shell find $(MODULES_DIR) -type d)
MODULES_C = $(shell find $(MODULES_DIR) -type f -name "*.c")
MODULES_H = $(shell find $(MODULES_DIR) -type f -name "*.h")
MODULES_O = $(notdir $(MODULES_C:%.c=%.o))
MODULES_OBJ_PATH = $(addprefix $(OBJ_DIR)/, $(MODULES_O))
GUI_CPP = $(shell find $(GUI_DIR) -type f -name "*.cpp")
GUI_H = $(shell find $(GUI_DIR) -type f -name "*.h")
#==============================================================================#


#==================== LIST OF FILE AND DIRS IN TEST MODULES ===================#
TEST_DIRS = $(shell find $(TEST_DIR) -type d)
TEST_C = $(shell find $(TEST_DIR) -type f -name "*.c")
TEST_H = $(shell find $(TEST_DIR) -type f -name "*.h")
TEST_O = $(notdir $(TEST_C:%.c=%.o))
TEST_OBJ_PATH = $(addprefix $(OBJ_DIR)/, $(TEST_O))
#==============================================================================#


#================= LIST OF FILES TO CLANG-FORMAT AND CPPCHECK =================#
C_FILES = $(MODULES_C) $(TEST_C)
H_FILES = $(MODULES_H) $(TEST_H)
ALL_FILES = $(C_FILES) $(H_FILES) $(GUI_CPP) $(GUI_H)
#==============================================================================#


#=============== CONFIGURING PATH TO SEARCH FOR UNPREFIXED PATHS ===============#
vpath %.c $(MODULES_DIRS) : $(GUI_DIRS) : $(TEST_DIRS)
vpath %.o $(OBJ_DIR)
#==============================================================================#


#================================= MAIN TARGETS ===============================#

.PHONY: $(LIBRARY) $(TEST) $(GCOV)

all: build

build: $(LIBRARY)
	mkdir -p $(BUILD)
	cd $(BUILD) && cmake ../src/ && make

install: build
	mkdir -p ~/$(PROGRAM)
	cp $(BUILD)/$(PROJECT_NAME) ~/$(PROGRAM)

uninstall: 
	rm -rf ~/$(PROGRAM)

$(LIBRARY): $(OBJ_DIR) $(MODULES_O) $(MODULES_H)
	ar rcs $@ $(MODULES_OBJ_PATH)
	ranlib $@

tests: $(LIBRARY) $(TEST_O)
	@$(CC) $(TEST_OBJ_PATH) $(LIBRARY) $(LDFLAGS) -o $(TEST)
	@-./$(TEST)

$(GCOV): $(REPORT_DIR) $(LIBRARY) $(TEST_O)
	$(CC) $(TEST_OBJ_PATH) $(MODULES_C) $(GCOV_FLAGS) -o $(addprefix $(OBJ_DIR)/, $(TEST))
	./$(addprefix $(OBJ_DIR)/, $(TEST))
	gcovr -r . --html --html-details -o $(REPORT_DIR)/coverage.html
	xdg-open $(REPORT_DIR)/coverage.html

dvi:
	rm -rf $(DVI_DIR)
	doxygen Doxyfile
	xdg-open $(DVI_DIR)/html/index.html

dist: clean
	@rm -rf $(DIST_DIR)
	@echo "Creating a distribution..."
	@mkdir -p $(DIST_DIR)/$(PROJECT_NAME)
	@cp -r $(GUI_DIR) $(MODULES_DIR) $(DIST_DIR)/$(PROJECT_NAME)
	@cp CMakeLists.txt Doxyfile Makefile $(DIST_DIR)/$(PROJECT_NAME) 
	@echo "Distribution packaging..."
	@tar -czf $(DIST_DIR)/$(PROJECT_NAME)-1.0.tar.gz -C $(DIST_DIR) $(PROJECT_NAME)
	@echo "\033[33mDistribution created: $(DIST_DIR)/$(PROJECT_NAME).tar.gz\033[0m"
	@rm -rf $(DIST_DIR)/$(PROJECT_NAME)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(REPORT_DIR)
	rm -rf $(DVI_DIR)
	rm -rf $(DIST_DIR)
	rm -rf $(GCOV)
	rm -rf $(BUILD)
	rm -rf ~/.config/$(PROJECT_NAME)
	rm -f $(TEST)
	rm -f $(GUI_DIR)/*.o $(GUI_DIR)/Makefile $(GUI_DIR)/moc* $(GUI_DIR)/qrc* $(GUI_DIR)/ui_* $(GUI_DIR)/.qmake*
	rm -f ./CMake*.user
	rm -f *.a
	rm -f val.txt

rebuild: clean build
#==============================================================================#


#============================== CREATE DIRECTORIES ============================#
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(REPORT_DIR):
	@mkdir -p $(REPORT_DIR)
#==============================================================================#


#============================== OBJECTIVE TARGETS =============================#
%.o: %.c $(MODULES_H)
	@$(CC) $(CFLAGS) -c -o $(addprefix $(OBJ_DIR)/, $@) $<

%_test.o: %_test.c
	@$(CC) $(CFLAGS) -c -o $(addprefix $(OBJ_DIR)/, $@) $<
#==============================================================================#


#=============================== CHECKS TARGETS ===============================#
clang:
	clang-format -style=Google -n $(ALL_FILES)

cppcheck:
	$@ $(CPPCHECK_CPPFLAGS) $(GUI_CPP) $(GUI_H)
	$@ $(CPPCHECK_CFLAGS) $(MODULES_C) $(MODULES_H)

valgrind: tests
	@$@ $(VAL) ./$(TEST) > val.txt 2>&1 && \
	{ 	printf "\033[1;33m\n"; \
		printf "                        Valgrind test results:\n"; \
		printf "\033[0m\n"; \
	}
	@tail -n 8 val.txt
#==============================================================================#


#============================= ADDITIONAL TARGETS =============================#
run: install
	@~/$(PROGRAM)/$(PROJECT_NAME)

clang_fix:
	clang-format -style=Google -i $(ALL_FILES)

dependencies:
	sudo apt update
	sudo apt install -y cppcheck
	sudo apt install -y clang-format
	sudo apt-get install -y check
	sudo apt install -y doxygen
	sudo apt-get install -y graphviz
	sudo apt install -y gcc g++ cmake qt6-base-dev libqt6opengl6-dev libgl1-mesa-dev
#==============================================================================#
