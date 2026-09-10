# 1. Compiler and Flag Definitions
CXX      := clang++
CXXFLAGS := -Wall -Wextra -fsanitize=address -std=c++17
LDFLAGS  := 

# 2. Target and Directories
TARGET   := my_string
SRCDIR   := src
OBJDIR   := obj

# 3. File Discovery (Ищет файлы и в src, и во всех подпапках)
SRCS     := $(shell find $(SRCDIR) -name "*.cpp")
# Это превратит, например, src/my_string/my_string.cpp в obj/my_string/my_string.o
OBJS     := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

# 4. Primary Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Шаблонное правило изменено, чтобы автоматически создавать нужные подпапки в obj
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJDIR):
	mkdir -p $(OBJDIR)

# 5. Cleanup Rule
clean:
	rm -rf $(OBJDIR) $(TARGET)

wrapper:
	clang++ -std=c++17 -shared -fPIC -I/usr/include/python3.14 src/my_string/my_string.cpp my_string_wrapper.cpp -lboost_python314 -lpython3.14 -o my_string.so
	python test.py

.PHONY: all clean
