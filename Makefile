.PHONY:r s
test:test.cpp
	g++ test.cpp -D RECV -o r -std=c++11
	g++ test.cpp -D SEND -o s -std=c++11
.PHONY:clean
clean:
	rm s r
