/*
	osgLod.cpp 
	create an OSG::LOD node Object from extranl lodable models.
	Miguel Leitao, ISEP, 2019
*/


#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>


void usage(char *app_name) {
    printf("Usage: %s [options] file_in [file_out]\n", app_name);
    exit(1);
}

int main(int argc, char* argv[])
{
//	osg::Matrix myMatrix;

    int runViewer = 0;
    
    char *fout_name = NULL;
    char *app_name = argv[0];
    float startDist = 0.1;
    float scaleDist = 10.;
    int pagedLod = 0;
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
            case 'p':           // PagedLOD
                pagedLod = 1;
                break;
            case 'c':           // Closest distance
                if ( strlen(argv[0])>2 ) {
                    startDist = atof(argv[0]+2);
                }
                else {
                    argc--;
                    argv++;
                    startDist = atof(argv[0]);
                }
                break;
            case 's':           // Scale distance
                 if ( strlen(argv[0])>2 ) {
                    scaleDist = atof(argv[0]+2);
                }
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
    
    //printf("startDist: %f, argc:%d, argv:%s\n", startDist, argc, *argv);
    
	// Creating the root node
    osg::LOD* SceneRoot;
    if ( pagedLod )
        SceneRoot = new osg::PagedLOD;
    else
        SceneRoot = new osg::LOD;
    unsigned int childNo = 0;
    while( argc>0 ) {
        if ( argv[0][0]>='0' && argv[0][0]<='9' ) {
            startDist = atof(*argv);
            endDist = startDist * scaleDist;
            if ( childNo>0 ) { 
                float min = SceneRoot->getMinRange(childNo-1);
                SceneRoot->setRange(childNo-1,min,startDist);
            }
        }
        else {
            if ( pagedLod ) {
                dynamic_cast<osg::PagedLOD*>(SceneRoot)->setFileName(childNo, *argv);
                SceneRoot->setRange(childNo,startDist,endDist);
            }
            else {
                osg::Node* loadedModel = osgDB::readNodeFile(*argv);
                SceneRoot->addChild(loadedModel,startDist,endDist);
            }
            //printf("Child %u added with range %f,%f\n", childNo,startDist,endDist);
            startDist = endDist;
            endDist *= scaleDist;
            childNo++;
        }
        argc--;
        argv++;
    }
   

    if ( fout_name ) {
        osgDB::writeNodeFile(*(SceneRoot), fout_name);
        //osgDB::writeNodeFile(*(SceneRoot), "scene.osg");
    }
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
