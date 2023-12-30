CC = gcc
CFLAGS = -Wall -O2
TARGET = parallel

# Detect the operating system
UNAME := $(shell uname)

# Set default paths
LIB_PATH = /usr/local/lib
INCLUDE_PATH = /usr/local/include

# Adjust paths based on the operating system
ifeq ($(UNAME), Darwin)  # macOS
    LIB_PATH = /usr/local/opt/zlib/lib
    INCLUDE_PATH = /usr/local/opt/zlib/include
endif

# Set the flags with adjusted paths
LDFLAGS = -L$(LIB_PATH) -lz
CFLAGS += -I$(INCLUDE_PATH)

SRC = parallel.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

