include radio.makefile

	

hackathon.o:
	g++ --std=c++11 -c hackathon.cpp -lsqlite3 -o hackathon.o


arduPi_pi2.o:
	g++ -c arduPi_pi2.cpp -o arduPi_pi2.o

SX1272_pi2.o:
	g++ -DRASPBERRY2 -c SX1272.cpp -o SX1272_pi2.o

hackathon: hackathon.o arduPi_pi2.o SX1272_pi2.o
	g++ --std=c++11 -lpthread -lrt -lsqlite3 hackathon.o arduPi_pi2.o SX1272_pi2.o -o hackathon




clean:
	rm *.o hackathon

# 	g++ -lrt -lpthread hackathon.o arduPi_pi2.o SX1272_pi2.o -o hackathon
