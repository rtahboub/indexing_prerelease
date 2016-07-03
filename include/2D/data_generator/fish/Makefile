LDFLAGS = -lDBUtl -lGL -lglut
SRC = main.cpp simulation_core.cpp unit.cpp

debug: $(SRC)
	g++ -g -I. -I../../../../../ -I../../../../../trunk/include/ -L../../../../../DBUtl/lib -o $@ $(SRC) $(LDFLAGS)

bench: $(SRC)
	g++ -O2 -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -rf *.o debug bench *~