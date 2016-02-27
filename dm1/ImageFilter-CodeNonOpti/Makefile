.PHONY:all tests

CC=gcc
CFLAGS=-Wall -pedantic -std=c99 


sources=$(wildcard *.c)

OBJ=$(sources:.c=.o)
DIR_OBJ=obj/
OBJS=$(addprefix $(DIR_OBJ), $(OBJ))

TARGET=imagefilter

all: $(TARGET)

clean:
	@rm -f $(OBJS) *~ $(TARGETS) $(addprefix $(DIR_OBJ), $(sources:.c=.d))


# compile
$(DIR_OBJ)%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DIR_OBJ)%.d: %.c	
	echo “make $@“
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

echo:
	echo $(sources)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

tests: $(TARGET)
	./test_script.sh all

include $(addprefix $(DIR_OBJ), $(sources:.c=.d))
# DO NOT DELETE
