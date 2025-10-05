CC       = gcc
CFLAGS   = -Wall -Wextra -O2 -I./lib        # header path, adjust if needed
LDFLAGS  = -L./lib                          # library path, adjust if needed
LDLIBS   = -lraylib \
           -framework CoreVideo \
           -framework IOKit \
           -framework Cocoa \
           -framework GLUT \
           -framework OpenGL

# ==== Targets ====
TARGET = game
SRC    = main.c
OBJ    = $(SRC:.c=.o)

# ==== Rules ====
.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
