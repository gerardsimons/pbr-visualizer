#include "scenewidget.h"
#include "pcwidget.h"
#include "window.h"

#include "RIV/Graphics/BMPImage.h"
#include "RIV/PBRTConfig.h"
#include "RIV/Data/DataSet.h"
#include "RIV/Data/DataFileReader.h"

#include "RIV/Geometry/MeshModel.h"

#include <QApplication>

//The main window containing all the widgets
Window* rootWindow;
const int windowWidth = 1600;
const int windowHeight = 900;

SceneWidget* sceneWidget = NULL;
PCWidget* pcWidget = NULL;

//Load data according to the generated paths
void loadData(const std::string& dataPath,const std::string& pbrtPath,const std::string& bmpPath) {
    if(!dataPath.empty() && !pbrtPath.empty()) {
        BMPImage* image = new BMPImage(bmpPath.c_str(),false);
        RIVDataSet dataset = DataFileReader::ReadBinaryData(dataPath,image,0);
		PBRTConfig pbrtConfig = DataFileReader::ReadPBRTFile(pbrtPath);
		
		rootWindow = new Window(dataset,image,pbrtConfig);
		//Extra data processing
		//		DataFileReader::AssignShapeIDsToPrimitives(dataset.GetTable("intersections"), model);
		//		testPBRTParser(pbrtPath);
			
    }
    else throw "Data paths not generated.";
}

void initializeUI() {
	rootWindow->move(0, 0);
	rootWindow->resize(windowWidth, windowHeight);
		
	//Show the widget on screen
	rootWindow->show();
}


int main(int argc, char **argv)
{
	//Start up QApplication
    QApplication a(argc, argv);
	
	std::string dataPath;
	std::string pbrtPath;
	std::string bmpPath;
	
	if(argc >= 4) {
		dataPath = argv[1];
        pbrtPath = argv[2];
        bmpPath = argv[3];
    }
	else throw "Invalid number of arguments (" + std::to_string(argc) + "). 4 are required.";
	
	loadData(dataPath,pbrtPath,bmpPath);
	
	initializeUI();

    return a.exec();
}
