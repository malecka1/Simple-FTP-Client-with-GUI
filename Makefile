CXX=g++
LD=g++
CFLAGS=-Wall -pedantic -Wno-long-long -O0 -ggdb -std=c++11
OBJECTS=cabstractDirectory.o cconnection.o cdirectoryFile.o cdirectoryFolder.o cftp.o cgui.o main.o

# $< name of the first prerequisite
# $@ name of the target
# 1st line condition, 2nd command

all: doc compile

main.o: ./src/main.cpp ./src/cftp.h ./src/cconnection.h ./src/cgui.h ./src/cabstractDirectory.h ./src/cdirectoryFolder.h ./src/cdirectoryFile.h
	$(CXX) $(CFLAGS) -c -o $@ $<

cabstractDirectory.o: ./src/cabstractDirectory.cpp ./src/cabstractDirectory.h
	$(CXX) $(CFLAGS) -c -o $@ $<

cconnection.o: ./src/cconnection.cpp ./src/cconnection.h ./src/cgui.h ./src/cabstractDirectory.h ./src/cdirectoryFolder.h ./src/cdirectoryFile.h
	$(CXX) $(CFLAGS) -c -o $@ $<

cdirectoryFile.o: ./src/cdirectoryFile.cpp ./src/cdirectoryFile.h ./src/cabstractDirectory.h
	$(CXX) $(CFLAGS) -c -o $@ $<

cdirectoryFolder.o: ./src/cdirectoryFolder.cpp ./src/cdirectoryFolder.h ./src/cabstractDirectory.h
	$(CXX) $(CFLAGS) -c -o $@ $<

cftp.o: ./src/cftp.cpp ./src/cftp.h ./src/cconnection.h ./src/cgui.h ./src/cabstractDirectory.h ./src/cdirectoryFolder.h ./src/cdirectoryFile.h
	$(CXX) $(CFLAGS) -c -o $@ $<

cgui.o: ./src/cgui.cpp ./src/cgui.h ./src/cabstractDirectory.h ./src/cdirectoryFolder.h ./src/cdirectoryFile.h
	$(CXX) $(CFLAGS) -c -o $@ $<

compile: $(OBJECTS)
	$(LD) $(CFLAGS) -o SimpleFTPClient.out $(OBJECTS) -lform -lmenu -lncurses 

run:
	./SimpleFTPClient.out

doc:
	doxygen Doxyfile

clean:
	rm -rf *.o SimpleFTPClient.out doc
