CC			:= gcc
RM			:= rm
SRC			:= power.c matrix.c
OBJS		:= $(patsubst %.c,%.o,$(SRC))
DEPS		:= $(patsubst %.c,%.d,$(SRC))
CFLAGS		:= -O2 -Wall -MP -MMD -pthread
LDFLAGS		:=
ifeq ($(shell uname),Darwin)
CFLAGS		+= -Xpreprocessor -fopenmp
LDFLAGS		+= -lomp
else
CFLAGS		+= -fopenmp
LDFLAGS		+= -fopenmp
endif
OUTPUT		:= power

.PHONY: all
all: $(OUTPUT)

-include $(DEPS)

$(OUTPUT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(DEPS) $(OUTPUT) $(addsuffix .exe,$(OUTPUT))
