
#BOOST_INCLUDE = /media/Work/Dump/boost_1_44_0/installed_libs/include
BOOST_INCLUDE = /usr/local/include

test: test.cpp dyndis_max_flow.hpp
	g++ -g -I$(BOOST_INCLUDE) test.cpp -o test

clean:
	rm -f test *.o
