/*
	osgLod.cpp 
	create an osg::LOD, osg::PaggedLOD, osg::Group or osgBillboard node from external loadable models.
	Miguel Leitao, ISEP, 2019
*/


#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include <osg/PolygonOffset>


void usage(char *app_name) {
    fprintf(stderr, "Usage: %s [options] { ModelSpec1 ModelSpec2 .... } [max_dist] \n", app_name);
    fprintf(stderr, "           options: -o OutputFile          : Save result to OutputFile\n");
    fprintf(stderr, "                    -g                     : Build an OSG::Group, without LOD\n");
    fprintf(stderr, "                    -p                     : Build an OSG::PagedLOD\n");
    fprintf(stderr, "                    -y                     : Build an OSG::Layer\n");
    fprintf(stderr, "                    -b                     : Build an OSG::BillBoard\n");
    fprintf(stderr, "                    -c min_dist            : Use min_dist as closest view distance\n");
    fprintf(stderr, "                    -s scale_dist          : Use scale_dist to increase range distances\n");
    fprintf(stderr, "           ModelSpec: [ min_dist ] ModelFilename\n");
    exit(1);
}


void addBillboardGroup(osg::Billboard *bb, osg::Group *grp) {
    for( unsigned int i=0 ; i< grp->getNumChildren() ; i++ ) {
        osg::Node *node = grp->getChild(i);
        osg::Geode *ngeo = node->asGeode();
        if ( ngeo ) {
            // parse all drawables
            for( unsigned int d=0 ; d<ngeo->getNumDrawables() ; d++ ) {
                osg::Drawable *dd = ngeo->getDrawable(d);
                bb->addDrawable(dd);
            }
            continue;
        }
        osg::Group *ngrp = node->asGroup();
        if ( ngrp ) 
            addBillboardGroup(bb, ngrp);
    }
    
}

int main(int argc, char* argv[])
{
    char *fout_name = NULL;
    char *app_name = argv[0];
    float startDist = 0.1;
    float scaleDist = 10.;
    char  nodeType = ' ';
    float endDist = startDist*scaleDist;
    argc--;
    argv++;
    while( argc>0 ) {
        //printf("processing option '%s'\n", argv[0]);
        if ( argv[0][0]!='-' ) break;
        switch( argv[0][1] ) {
            case 'o':           // Output file
                argc--;
                argv++;
                if ( argc<=0 ) usage(app_name);
                fout_name = argv[0];
                break;
            case 'p':
            case 'g':
            case 'y':
            case 'b':
                nodeType = argv[0][1];
                break;
            case 'c':           // Closest distance
                if ( strlen(argv[0])>2 ) 
                    startDist = atof(argv[0]+2);
                else {
                    argc--;
                    argv++;
                    startDist = atof(argv[0]);
                }
                break;
            case 's':           // Scale distance
                 if ( strlen(argv[0])>2 ) 
                    scaleDist = atof(argv[0]+2);
                else {
                    argc--;
                    argv++;
                    scaleDist = atof(argv[0]);
                }
                break;               
            default:
                fprintf(stderr,"Option '%s' not recognised.\n", argv[0]);
                usage(app_name);
        }
        argc--;
        argv++;
    }
    
    if ( nodeType==' ' ) nodeType = 'l';
    
	// Creating the root node
    osg::Group* SceneRoot;
    switch ( nodeType) {
        case 'l':
            SceneRoot = new osg::LOD;
            break;
        case 'p':
            SceneRoot = new osg::PagedLOD;
            break;
        case 'g':
            SceneRoot = new osg::Group;
            break;
        case 'y':
            SceneRoot = new osg::Group;
            break;
        case 'b':
            SceneRoot = new osg::Group;
            break;
        default:
            fprintf(stderr,"Error. Unkown node type.\n");
            exit(1);
            break;
    }

    unsigned int childNo = 0;
    while( argc>0 ) {
        if ( ( nodeType=='l' || nodeType=='p' ) && argv[0][0]>='0' && argv[0][0]<='9' ) {
            startDist = atof(*argv);
            endDist = startDist * scaleDist;
            if ( childNo>0 ) {
                osg::LOD *rNode = dynamic_cast<osg::LOD*>(SceneRoot);
                float min = rNode->getMinRange(childNo-1);
                rNode->setRange(childNo-1,min,startDist);
            }
        }
        else {
            osg::Node* loadedModel;
            switch( nodeType ) {
                case 'p': {
                        osg::PagedLOD *nodePLOD = dynamic_cast<osg::PagedLOD*>(SceneRoot);
                        //dynamic_cast<osg::PagedLOD*>(SceneRoot)->setFileName(childNo, *argv);
                        nodePLOD->setFileName(childNo, *argv);
                        nodePLOD->setRange(childNo,startDist,endDist);
                    }   
                    break;
                case 'l': { 
                        osg::LOD *rNode = dynamic_cast<osg::LOD*>(SceneRoot);
                        loadedModel = osgDB::readNodeFile(*argv);
                        rNode->addChild(loadedModel,startDist,endDist);
                    }
                    break;
                case 'g':
                    loadedModel = osgDB::readNodeFile(*argv);
                    SceneRoot->addChild(loadedModel);
                    break;
                case 'b': {
                        loadedModel = osgDB::readNodeFile(*argv);
                        osg::Billboard *bb = new osg::Billboard();
                        bb->setMode(osg::Billboard::AXIAL_ROT);
                        bb->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
                        bb->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
                        addBillboardGroup(bb,loadedModel->asGroup());
                        SceneRoot->addChild(bb);
                    }
                    break;
                case 'y':
                    loadedModel = osgDB::readNodeFile(*argv);
                    osg::StateSet *ss = loadedModel->getOrCreateStateSet();
                    if (ss) { 
                        osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
                        polyoffset->setFactor(-startDist);
                        polyoffset->setUnits(-1.0f);
                        ss->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
                    }
                    else 
                        fprintf(stderr,"Couldnot get StateSet\n");
                    SceneRoot->addChild(loadedModel);
                    endDist = (float)(childNo+1);
                    break;
            }
            startDist = endDist;
            endDist *= scaleDist;
            childNo++;
        }
        argc--;
        argv++;
    }
    //printf("All objects loaded\n");
    
    if ( fout_name ) 
        osgDB::writeNodeFile(*(SceneRoot), fout_name);
    else {
        // Creating the viewer
        osgViewer::Viewer viewer ;
        viewer.setSceneData( SceneRoot );

        // Setup camera
        osg::Matrix matrix;
        matrix.makeLookAt( osg::Vec3(0.,-30.,5.), osg::Vec3(0.,0.,0.), osg::Vec3(0.,0.,1.) );
        viewer.getCamera()->setViewMatrix(matrix);

        viewer.setCameraManipulator(  new osgGA::TrackballManipulator() );

        while( !viewer.done() ) {
            viewer.frame();
        }
    }

}

