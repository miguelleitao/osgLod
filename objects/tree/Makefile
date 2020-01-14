

TARGETS=arvore1_bb.osg arvore2_bb.osg arvore3_bb.osg arvore4_bb.osg

all: ${TARGETS}

%_bb.osg: %_g.osg
	../osgLod -b -o $@ $^

%_r.obj: %.obj
	ObjTool -O $@ -rx -90 $^

%_g.osg: %_r.obj
	osgconv $^ $@

.PRECIOUS: %.obj %.png
%.obj: %.flt %.png
	osgconv $< $@

%.png: %.rgb
	convert $< $@


