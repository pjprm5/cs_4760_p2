CFLAGS += -lpthread -lrt
all: master palin
clean:
	/bin/rm -f *.o $(TARGET) master palin
