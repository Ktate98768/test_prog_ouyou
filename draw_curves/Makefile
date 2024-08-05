CC			:= gcc
CFLAGS		:= -O2 -Wall -MP -MMD -I../include -DGL_SILENCE_DEPRECATION
LDFLAGS		:= -L../lib -lmatrix -llinked_list
SRC			:= draw_curves_refined.c
OBJS		:= $(patsubst %.c,%.o,$(SRC))
DEPS		:= $(patsubst %.c,%.d,$(SRC))
OUTPUT		:= draw_curves_refined
UNAME		:= $(shell uname | tr '[:lower:]' '[:upper:]')

ifeq ($(UNAME),DARWIN)
FRAMEWORKS	:= -framework OpenGL -framework GLUT
else ifeq ($(findstring CYGWIN, $(UNAME)),CYGWIN)
FRAMEWORKS	:= -lopengl32 -lglu32 -lglut
else
FRAMEWORKS	:= -lopengl32 -lglu32 -lfreeglut
endif

.PHONY: all
all: $(OUTPUT)

-include $(DEPS)

$(OUTPUT): $(OBJS)
	$(CC) $^ $(LDFLAGS) $(FRAMEWORKS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) -f $(DEPS) $(OBJS) $(OUTPUT) $(addsuffix .exe,$(OUTPUT))
