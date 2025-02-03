# 编译器设置
CXX      := g++
CXXFLAGS := -Iinclude -Wall -Wextra -std=c++11
LDFLAGS  := 
BIN_NAME := app

# 目录设置
SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
INC_DIR  := include

# 自动获取源文件和目标文件列表
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS     := $(OBJS:.o=.d)

# 默认目标
all: create-dirs $(BIN_DIR)/$(BIN_NAME)

# 包含自动生成的依赖文件
-include $(DEPS)

# 可执行文件规则
$(BIN_DIR)/$(BIN_NAME): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# 目标文件规则（带自动依赖生成）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# 快捷目标：启动服务器
server: all
	@make clean
	@make
	@echo "Starting server on port 5005..."
	@./bin/app -s 5005

# 快捷目标：启动客户端
client: all
	@echo "Starting client connecting to 182.92.104.14:5005..."
	@./bin/app -c 182.92.104.14 5005

# 同步到服务器
sync: all
	@echo "Starting sync to server..."
	@rsync -avz ./ root@182.92.104.14:/root/code/netserver/

# 创建必要目录
create-dirs:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# 清理规则
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

# 伪目标声明
.PHONY: all clean create-dirs server client