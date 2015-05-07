CFLAGS := -O2 -Wall

all: pcm2c

pcm2c: pcm2c.o
	gcc -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -o $@ -c $^

clean:
	rm *.o
