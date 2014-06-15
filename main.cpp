#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "Filter.h"
#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"
#include "ImageView.h"
#include "DataFileReader.h"
#include "3DView.h"
#include "ColorPallete.h"
#include "ColorProperty.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <gtest/gtest.h>

const float DEG2RAD = 3.14159/180;

/* window width and height */
//int width = 1680;
//int height = 1000;

int width = 1400;
int height = 800;

RIVClusterSet* clusters; //HERE ONLY FOR DEBUG REASONS
const size_t clusterK = 2;

bool isDirty = true;

/* window position */
int posX = 0;
int posY = 0;

int wd;                   /* GLUT window handle */

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

/* Contains pointers to all the views to be drawn */
std::vector<RIVDataView*> views;

RIVImageView *imageView;
RIV3DView *sceneView;
ParallelCoordsView *parallelCoordsView;

/* The dataset, views have pointers to this in order to draw their views consistently */
RIVDataSet dataset;

/* Draw the window - this is where all the GL actions are */
void display(void)
{



//
//    
    
    imageView->Draw();
    parallelCoordsView->Draw();
    sceneView->Draw();
    
//    glColor3f(1.F,0,0);
    
//    float radius = 10.F;
//    
//    glBegin(GL_LINE_LOOP);
//    
//    for (int i=0; i < 360; i++)
//    {
//        float degInRad = i*DEG2RAD;
//        glVertex2f(cos(degInRad)*radius + lastMouseX,sin(degInRad)*radius + lastMouseY);
//    }
//    
//    glEnd();
//    glFlush();
    
//    copy_buffer();
    
    glutSwapBuffers();
}

/* Handles mouse input */
void mouse(int button, int state, int x, int y) {
	//Check what view catches the interaction
	y = height - y; //Very important to invert Y!
	lastMouseX = x;
	lastMouseY = y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouse(button,state,x,y)) {
            printf("View %s caught the MOUSE interaction\n",views[i]->identifier.c_str());
//            if(state == GLUT_UP)
			glutPostRedisplay();
			return;
		}
	}
}

std::string lastMotionCatch = "";

void motion(int x, int y) {
	y = height - y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouseMotion(x,y)) {
            if(views[i]->identifier != lastMotionCatch) {
                printf("View %s caught the MOTION interaction\n",views[i]->identifier.c_str());
                lastMotionCatch = views[i]->identifier;
            }
			glutPostRedisplay();
			return;
		}
	}
}

void invalidateAllViews() {
	for(size_t i = 0 ; i < views.size() ; i++) {
        views[i]->Invalidate();
    }
}

void keys(int keyCode, int x, int y) {
//    printf("Pressed %d at (%d,%d)\n",keyCode,x,y);
    bool postRedisplay = true;
    switch(keyCode) {
        case 27: //ESC key
            printf("Clear filters\n");
//            invalidateAllViews();
            dataset.ClearFilters();
            break;
        case 98: // 'b' key
            glutSwapBuffers();
//            copy_buffer();
            postRedisplay = false;
            break;
        case 99: // 'c' key
            if(sceneView) {
                sceneView->ToggleDrawClusterMembers();
                postRedisplay = true;
            }
            break;
        case 114: // 'r' key, recluster {
        {
            RIVTable *intersectTable = dataset.GetTable("intersections");
            RIVClusterSet* clusters = intersectTable->Cluster("intersection X","intersection Y","intersection Z",clusterK,1);
            postRedisplay = true;
            break;
        }
        case 111: // 'o' key, optimize clusters (debug feature)
        {
            clusters->OptimizeClusters();
            postRedisplay = true;
            break;
        }
        case GLUT_KEY_UP:
            sceneView->MoveCamera(0,0,1.F);
            break;
        case GLUT_KEY_DOWN:
            sceneView->MoveCamera(0,0,-1.F);
            break;
        case GLUT_KEY_LEFT:
            sceneView->MoveCamera(-1.F,0,0);
            break;
        case GLUT_KEY_RIGHT:
            sceneView->MoveCamera(1.F,0,0);
            break;
        default:
            postRedisplay = false;
    }
    if(postRedisplay) {
        glutPostRedisplay();
    }
}

void idle() {
    //Do animations?
}



/* Called when window is resized,
 also when window is first created,
 before the first call to display(). */
void reshape(int w, int h)
{
    printf("reshape called.\n");
    /* save new screen dimensions */
    width = w;
    height = h;
    
    /* Invalidate all views */
    invalidateAllViews();
}

void initialize(int argc, char* argv[]) {
    std::string fullPath;
    std::string pbrtPath;
    std::string bmpPath;
    printf("%d additional arguments given\n", argc - 1);
    if(argc <= 1) {
        //Default values
#ifdef _WIN32
        std::string resourcesPath = "../RenderingInfoVis/Resources/";
#elif __APPLE__
        std::string resourcesPath = "../../../Resources/";
#endif
        std::string fileName = "teapot32x32x1.exr";
        
        fullPath = resourcesPath + fileName;
    } else if(argc == 2) {
        fullPath = argv[1];
        pbrtPath = fullPath + ".pbrt";
        bmpPath = fullPath + ".bmp";
    }
    else if(argc == 3) { //Explicit PBRT file path defined
        fullPath = argv[1];
        pbrtPath = argv[2];
    }
    else if(argc == 4) {
        fullPath = argv[1];
        pbrtPath = argv[2];
        bmpPath = argv[3];
    }
    
    printf("using fullpath = %s\n",fullPath.c_str());
    
//	int imageWidth = (.2F * width);
//    int imageHeight = (.2F * width);
    
    int imageWidth = .4F * height;
    int imageHeight = .4F * height;
    
    int imageSceneWidth = .4F * height;
    int imageSceneHeight = .4F * height;
    
    BMPImage image = BMPImage((bmpPath).c_str(),false);
    
    dataset = DataFileReader::ReadAsciiData(fullPath + ".txt",image);
    
    //clustering
    dataset.ClusterTable("intersections","intersection X","intersection Y","intersection Z",clusterK,1);
    
    //Set the vertices data from the PBRT file
    std::vector<float> modelData;
    modelData = DataFileReader::ReadModelData(pbrtPath);
    
	//CAUTION: Image should be power of two!
//	RIVImageView *imageView = new RIVImageView(fullPath + ".bmp",0,0,imageWidth,imageHeight,0,0);
//	ParallelCoordsView *parallelCoordsView = new ParallelCoordsView(imageWidth,0,width-imageWidth-imageSceneWidth,height,50,20);
//    RIV3DView *sceneView = new RIV3DView(width-imageSceneWidth,0,imageSceneWidth,imageSceneHeight,0,0);
    
//    RIVColorProperty *colorProperty = new RIVColorLinearProperty("path");
    
//    RIVRecord* redRecord = dataset.FindRecord("R");
//    RIVRecord* greenRecord = dataset.FindRecord("G");
//    RIVRecord* blueRecord = dataset.FindRecord("B");
//    
//    RIVColorProperty *colorProperty = new RIVColorRGBProperty("image",redRecord,greenRecord,blueRecord);
    RIVTable *imageTable = dataset.GetTable("image");
    RIVTable *pathTable = dataset.GetTable("path");
    imageTable->FilterRowsUnlinkedTo(pathTable);
    RIVColorProperty *colorProperty = new RIVInterpolatedColorProperty(pathTable,colors::GREEN,colors::RED);

    //Linear cluster coloring
    std::vector<size_t> medoidIndices = dataset.GetClusterSet()->GetMedoidIndices();
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
    RIVInterpolatedColorProperty* clusterColorProperty = new RIVInterpolatedColorProperty(intersectionsTable);
//    clusterColorProperty->EnableColorByCluster();
    
    RIVClusterSet* clusterSet = intersectionsTable->GetClusterSet();
    std::vector<size_t> medoids = clusterSet->GetMedoidIndices();
    
    clusterColorProperty->AddInterpolationScheme(medoids, new DiscreteInterpolator<size_t>(medoids));
    
    std::vector<float const*> colorPallette = colors::allColors();
    
    //Declare views
	parallelCoordsView = new ParallelCoordsView(0,imageHeight,width,height - imageHeight,50,20,clusterColorProperty);
    imageView = new RIVImageView(image,0,0,imageWidth,imageHeight,0,0,clusterColorProperty);
    sceneView = new RIV3DView(imageWidth,0,imageSceneWidth,imageSceneHeight,0,0,clusterColorProperty);
    
    //Set data source of views
	imageView->SetData(&dataset);
	parallelCoordsView->SetData(&dataset);
    sceneView->SetData(&dataset);
    sceneView->SetModelData(modelData);
    
	imageView->ComputeLayout();
    parallelCoordsView->ComputeLayout();
    sceneView->ComputeLayout();
    
    dataset.AddFilterListener(sceneView);
    dataset.AddFilterListener(parallelCoordsView);
	
    views.push_back(sceneView);
    views.push_back(imageView);
    views.push_back(parallelCoordsView);
}

//Unofficial testing
bool tests(int argc, char** argv) {
//    std::vector<size_t> pool;
//    //Generate pool
//    for(size_t i = 2 ; i < 12 ; i++) {
//        pool.push_back(i);
//    }
//    //Testing nonreplacementsampler
//    NonReplacementSampler<size_t> sampler = NonReplacementSampler<size_t>(&pool);
//    for(int i = 0 ; i < 10 ; i++) {
//        printf("Sample #%d = %zu\n",i,sampler.RequestSample());
//    }
    
//    if(dataset.IsSet()) {
//        RIVTable *intersectTable = dataset.GetTable("intersections");
////        intersectTable->ClusterWithSize("intersection X","intersection Y","intersection Z",1000);
//
//    }
    
//    std::vector<size_t> testValues;
//    testValues.push_back(0);
//    testValues.push_back(5);
//    testValues.push_back(7);
//    testValues.push_back(15);
//    
//    DiscreteInterpolator<size_t> interpolator = DiscreteInterpolator<size_t>(testValues);
//    for(size_t i = 0 ; i < 20 ; i++) {
//        printf("%zu = %f\n",i,interpolator.Interpolate(i));
//    }
    
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//    return false;
    return true;
    
    RIVTable tableOne = RIVTable("table_1");
    RIVTable tableTwo = RIVTable("table_2");
    RIVTable tableThree = RIVTable("table_3");
    
    RIVFloatRecord recordOne = RIVFloatRecord("record_1");
    RIVFloatRecord recordTwo = RIVFloatRecord("record_2");
    RIVFloatRecord recordThree = RIVFloatRecord("record_3");
    
    std::vector<float> valuesOne;
    valuesOne.push_back(1.F); // ---> referes to row 1 of table two
    valuesOne.push_back(2.F);
    valuesOne.push_back(3.F);
    
    std::vector<float> valuesTwo;
    valuesTwo.push_back(11.F);
    valuesTwo.push_back(12.F); // ---> refers to row 0 and 2 or table three
    
    std::vector<float> valuesThree;
    valuesThree.push_back(111.F);
    valuesThree.push_back(112.F);
    valuesThree.push_back(113.F);
    valuesThree.push_back(114.F);
    valuesThree.push_back(115.F);
    
    recordOne.SetValues(valuesOne);
    recordTwo.SetValues(valuesTwo);
    recordThree.SetValues(valuesThree);
    
    tableOne.AddRecord(&recordOne);
    tableTwo.AddRecord(&recordTwo);
    tableThree.AddRecord(&recordThree);
    
    // Reference from table one to table two
    RIVReference reference = RIVReference(&tableOne, &tableTwo);
    std::map<size_t,std::vector<size_t>> indexReferences;
    std::vector<size_t> targetIndices;
    targetIndices.push_back(1);
    indexReferences[0] = targetIndices;
    reference.SetReferences(indexReferences);
    tableOne.AddReference(reference);
    tableTwo.AddReference(reference.ReverseReference());
    
    //Reference from table two to table three
    RIVReference referenceTwo = RIVReference(&tableTwo, &tableThree);
    std::map<size_t,std::vector<size_t>> indexTwoReferences;
    std::vector<size_t> targetTwoIndices;
    targetTwoIndices.push_back(3);
    targetTwoIndices.push_back(4);
    indexTwoReferences[1] = targetTwoIndices;
    referenceTwo.SetReferences(indexTwoReferences);
    tableTwo.AddReference(referenceTwo);
    tableThree.AddReference(referenceTwo.ReverseReference());
    
    RIVReferenceChain chain;
    tableTwo.GetReferenceChainToTable(tableThree.GetName(), chain);
    
    std::cout << "table_one row 0 is linked to table_three rows : [";
    std::vector<size_t> resolvedRows = chain.ResolveRow(0);
    for(size_t i : chain.ResolveRow(0)) {
        std::cout << i;
    }
    
    return true;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    //Use double buffering!
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    /* set the initial window size */
    glutInitWindowSize(width, height);
    
    /* set the initial window position */
    glutInitWindowPosition(posX,posY);
    
    /* create the window and store the handle to it */
    wd = glutCreateWindow("Rendering InfoVis" /* title */ );
    
    /* Initialize the data and the views */
    initialize(argc, argv);
    
    /* --- register callbacks with GLUT --- */
    
    /* register function to handle window resizes */
    glutReshapeFunc(reshape);
    
    /* register function that draws in the window */
    glutDisplayFunc(display);
    
    /* register function that handles mouse */
    glutMouseFunc(mouse);
    
    glutSpecialFunc(keys);
    
    /* Transparency stuff */
    glEnable (GL_BLEND);
    
    glDrawBuffer(GL_FRONT); //Front is red
//    glClearColor(1,0,0, 0.0);
    glClearColor(1,1,1, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glDrawBuffer(GL_BACK);
    glClearColor(1,1,1, 0.0); //Back is green
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glutMotionFunc(motion);

    if(!tests(argc, argv)) {
        return EXIT_FAILURE;
    }
    
    /* start the GLUT main loop */
    glutMainLoop();
    
    
    return EXIT_SUCCESS;
}
