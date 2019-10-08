# disable optimization, just to make sure the dead code
# is not elliminated, furthermore we need debugging symbols
# later for perf analysis
CFLAGS  = -g -Wall -O0

TARGET = runner

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
