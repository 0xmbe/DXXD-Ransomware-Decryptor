# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17 

# Source files
SRCS = DXXD_Ransomware_Decrypt_Algorithm.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = DXXD_Ransomware_Decrypt_Algorithm

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lgdi32 -luser32

# Compile each .cpp file to .o
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean