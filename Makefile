
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
	./${TARGET} -o outLOD.osg ${OBJDIR}/obj1.obj ${OBJDIR}/obj2.obj ${OBJDIR}/obj3.obj
	./${TARGET} -o outPLOD.osg -p ${OBJDIR}/obj1.obj ${OBJDIR}/obj2.obj ${OBJDIR}/obj3.obj
	./${TARGET} -o outGRP.osg -g ${OBJDIR}/obj1.obj ${OBJDIR}/obj2.obj ${OBJDIR}/obj3.obj
	./${TARGET} -o outLayer.osg -y ${OBJDIR}/obj1.obj ${OBJDIR}/base.obj ${OBJDIR}/over.obj
	./${TARGET} -o outBB.osg -b ${OBJDIR}/obj1.obj


push:
	git add .
	git commit -m "update"
	git push


.PHONY: clean
clean:
	rm -f ${TARGET}.o ${TARGET} 



