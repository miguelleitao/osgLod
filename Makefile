
INC_OSG=-I/usr/include/osg
LD_OSG=-losg -losgViewer -losgSim -l osgDB -l osgGA

TARGET=osgLod

OBJDIR=objects

all: ${TARGET} 

${TARGET}: ${TARGET}.o
	g++ -o $@ $^ ${LD_OSG} -l stdc++ -lm

${TARGET}.o: ${TARGET}.cpp
	g++  -Wall -O2  -c ${INC_OSG} $<

test: ${TARGET}
	mkdir -p test
	./${TARGET} -o test/outLOD.osg ${OBJDIR}/obj1.obj ${OBJDIR}/obj2.obj ${OBJDIR}/obj3.obj
	./${TARGET} -o test/outPLOD.osg -p ${OBJDIR}/obj1.obj ${OBJDIR}/obj2.obj ${OBJDIR}/obj3.obj
	./${TARGET} -o test/outGRP.osg -g ${OBJDIR}/obj1.obj ${OBJDIR}/obj2.obj ${OBJDIR}/obj3.obj
	./${TARGET} -o test/outLayer.osg -y ${OBJDIR}/obj1.obj ${OBJDIR}/base.obj ${OBJDIR}/over.obj
	./${TARGET} -o test/outBB.osg -b ${OBJDIR}/obj1.obj


push:
	git add .
	git commit -m "update"
	git push


.PHONY: clean
clean:
	rm -f ${TARGET}.o ${TARGET} 



