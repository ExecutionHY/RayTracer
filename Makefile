CC := g++
CFLAGS := -g
TARGET := test
SRCS := $(wildcard *.cc)
OBJS := $(patsubst %cc,%o,$(SRCS))
all:$(TARGET)
%.o:%.cc
	$(CC) $(CFLAGS) -c $<
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -rf $(TARGET) *.o