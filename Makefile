# title: Makefile
# author: Alok Sood

BOOST_INCLUDE = /media/Work/Dump/boost_1_44_0/installed_libs/include

BOOST_LIB = /media/Work/Dump/boost_1_44_0/installed_libs/lib/

bpm: bpm.cpp read_bipartite.hpp
	g++ -g -I$(BOOST_INCLUDE) bpm.cpp -o bpm

clean:
	rm -f bpm *.o
