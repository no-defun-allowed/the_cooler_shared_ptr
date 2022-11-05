TARGETS=test
CPPFLAGS=-std=c++20 -Wall -fsanitize=address -g --pedantic

test: test.o crc.o

targets: $(TARGETS)
	@:
%.o: %.cpp
	g++ $(CPPFLAGS) -c -o $@ $<
%:
	g++ $(CPPFLAGS) -o $@ $^
clean:
	rm -f *.o $(TARGETS)
