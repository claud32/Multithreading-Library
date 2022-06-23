# Target library
lib := libuthread.a
# Compile options
CFLAGS = -Wall -Wextra -Werror
object := queue.o uthread.o preempt.o context.o private.o

all: $(lib)
	
$(lib): $(object)
	ar rcs $(lib) $(object)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

private.o: private.h
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(lib) $(object)