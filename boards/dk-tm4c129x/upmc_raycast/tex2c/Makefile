CFLAGS := -O2 -Wall

all: tex2c

tex2c: tex2c.o
	gcc -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -o $@ -c $^

clean:
	rm *.o
