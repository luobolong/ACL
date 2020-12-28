main: demo.o libaccess.a
	g++ -std=c++11 -o demo demo.o libaccess.a

access_lib.o: access_lib.cpp
	g++ -std=c++11 -c access_lib.cpp

libaccess.a: access_lib.o
	ar rs libaccess.a access_lib.o

demo.o: demo.cpp
	g++ -std=c++11 -c demo.cpp

clean:
	rm access_lib.o
	rm demo.o
	rm demo
	rm libaccess.a
	rm *.txt

