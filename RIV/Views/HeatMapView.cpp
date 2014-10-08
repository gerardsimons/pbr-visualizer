//
//  HeatmapView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 23/08/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "HeatMapView.h"
#include "../helper.h"
#include "../Graphics/ColorMap.h"
#include "../Graphics/ColorPalette.h"



	RIVHeatMapView* RIVHeatMapView::instance = NULL;

	std::vector<std::vector<float> > heatmap;
	ColorMap colorMap;

	//RIVHeatMapView::RIVHeatMapView(RIVColorProperty *controlledColorProp, RIVSizeProperty* controlledSizeProp) {
	//    if(instance != NULL) {
	//        throw "Only 1 instance of RIVHeatMapView allowed.";
	//    }
	//    instance = this;
	//}
	//

	void RIVHeatMapView::computeHeatMap() {
		printf("Computing heatmap.");
		
		heatmap.clear();
		
		//Use a custom sql to see how many intersections each path has
		std::string getMinMax = " select MIN(IMAGE_X), MAX(IMAGE_X), MIN(IMAGE_Y), MAX(IMAGE_Y) FROM PATHS";
		std::string getAllData = "select IMAGE_X,IMAGE_Y,COUNT(PID) from paths,intersections where pid = id group by id";
		
//		RIVTable *pathTable = dataset->GetTable("path");
		
		
		sqlite3_stmt* allStmt = dataController->CustomSQLStmt(getAllData);
		
		//Find x and y record
//		RIVUnsignedShortRecord* xRecord = pathTable->GetRecord<RIVUnsignedShortRecord>("x");
//		RIVUnsignedShortRecord* yRecord = pathTable->GetRecord<RIVUnsignedShortRecord>("y");
//		RIVUnsignedShortRecord* intersections = pathTable->GetRecord<RIVUnsignedShortRecord>("#intersections");
		
		sqlite3_stmt* minMaxStmt = dataController->CustomSQLStmt(getMinMax);
		sqlite3_step(minMaxStmt);
		xMax = sqlite3_column_int(minMaxStmt, 3) + 1;
		yMax = sqlite3_column_int(minMaxStmt, 5) + 1;
		
		colorMap = colors::jetColorMap();
		heatmap.resize( xMax , std::vector<float>( yMax , 0 ) );
		
//		TableIterator* iterator = pathTable->GetIterator();
		
		float max = std::numeric_limits<float>::min();
		
		while(sqlite3_step(allStmt) == SQLITE_ROW) {
			ushort x = sqlite3_column_int(allStmt, 1);
			ushort y = sqlite3_column_int(allStmt, 2);
			ushort nrIsects = sqlite3_column_int(allStmt, 3);
			//Update heatmap count
			heatmap[x][yMax - y - 1] += nrIsects;
		}
		
		//Find maximum
		for(size_t x = 0 ; x < xMax ; ++x) {
			for(size_t y = 0 ; y < yMax ; ++y) {
				float value = heatmap[x][y];
				if(value > max) {
					max = value;
				}
			}
		}
		
		//Divide all by maximum
		for(size_t x = 0 ; x < xMax ; ++x) {
			for(size_t y = 0 ; y < yMax ; ++y) {
				heatmap[x][y] /= max;
			}
		}
		
		//Done
//		glutPostRedisplay();
	}

	void RIVHeatMapView::DrawInstance() {
	//    printf("RIVHeatMapView DrawInstance()\n");
		if(instance != NULL) {
			instance->Draw();
		}
		else {
			printf("No instance to draw.\n");
		}
	}

	void RIVHeatMapView::Draw() {
		printf("***** RIVHeatMapView Draw *****\n");
		
		//Determine size of each grid tile
		float xTileSize = width / (float)xMax;
		float yTileSize = height / (float)yMax;

		glClearColor(1, 0, 0, 0.0);
		glClear( GL_COLOR_BUFFER_BIT );
		
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable(GL_DEPTH_TEST);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		//Color declaration
		const size_t nr_colors = 6;
		
		//Colors in RGB
		//Blue cyan green yellow red
	//    float colors[nr_colors][3] = {{0,0,1},{0,1,1},{0,1,0},{1,1,0},{1,0,0}};
//		float colors[nr_colors][3] = {{0,0,.57},{0,0,1},{0,1,1},{1,1,0},{1,0,0},{.5,0,0}};
		
		//Colors in HSV
	//    float colors[nr_colors][3]    = {{240,1,1},{180,1,1},{120,1,1},{60,1,1},{0,1,1}};
		
		//Draw heatmap
		for(size_t x = 0 ; x < xMax ; ++x) {
			for(size_t y = 0 ; y < yMax ; ++y) {
	//        for(size_t y = yMax ; y > 0 ; --y) {
			
				float heatmapValue = heatmap[x][y];
	//            float color[3] = {1,0,0}; //Red
				Color squareColor = colorMap.ComputeColor(heatmapValue);
				
				glColor3f(squareColor.R,squareColor.G,squareColor.B);
				
				glBegin(GL_QUADS);
				glVertex3f(x * xTileSize,y * yTileSize, 1);
				glVertex3f((x + 1) * xTileSize,y * yTileSize, 1);
				glVertex3f((x + 1) * xTileSize,(y + 1) * yTileSize, 1);
				glVertex3f(x * xTileSize,(y + 1) * yTileSize, 1);
				glEnd();
			}
		}
		
		glutSwapBuffers();
	}


	void RIVHeatMapView::Reshape(int width, int height) {
		printf("RIVHeatMapView Reshape called.\n");
		this->width = width;
		this->height = height;
		
		paddingX = 0;
		paddingY = 0;
		
		startX = 0;
		startY = 0;
		
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, width, 0.0, height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void RIVHeatMapView::ReshapeInstance(int width, int height){
		if(instance != NULL) {
			instance->Reshape(width, height);
		}
	}

	void RIVHeatMapView::Mouse(int button, int state, int x, int y) {
		if(instance != NULL) {
			instance->Mouse(button,state,x,y);
		}
	}

	void RIVHeatMapView::Motion(int x, int y) {
		if(instance != NULL) {
			instance->Motion(x,y);
		}
	}

	//Not implemented
	bool RIVHeatMapView::HandleMouse(int,int,int,int) {
		return false;
	}

	// Not implemented
	bool RIVHeatMapView::HandleMouseMotion(int,int) {
		return false;
	}
