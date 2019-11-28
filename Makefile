
INC_OSG=-I/usr/include/osg
LD_OSG=-losg -losgViewer -losgSim -l osgDB -l osgGA

TARGET=osgLod

all: ${TARGET} 

${TARGET}: ${TARGET}.o
	g++ -o $@ $^ ${LD_OSG} -l stdc++ -lm

${TARGET}.o: ${TARGET}.cpp
	g++  -Wall -O2  -c ${INC_OSG} $<

push:
	git add .
	git commit -m "update"
	git push


.PHONY: clean
clean:
	rm -f ${TARGET}.o ${TARGET} 



