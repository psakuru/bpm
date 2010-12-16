
#BOOST_INCLUDE = /media/Work/Dump/boost_1_44_0/installed_libs/include
BOOST_INCLUDE = /usr/local/include

#BOOST_LIB = /media/Work/Dump/boost_1_44_0/installed_libs/lib/

ek: ek.cpp
	g++ -g -I$(BOOST_INCLUDE) ek.cpp -o ek 

pr: pr.cpp
	g++ -g -I$(BOOST_INCLUDE) pr.cpp -o pr

randomplay: randomplay.cpp
	g++ -g -I$(BOOST_INCLUDE) randomplay.cpp -o randomplay

clean:
	rm -f randomplay ek pr *.o
