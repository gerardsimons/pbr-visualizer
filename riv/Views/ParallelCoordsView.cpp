#include "ParallelCoordsView.h"
#include "SliderView.h"
#include "DataView.h"
#include "../Graphics/graphics_helper.h"
#include "../helper.h"
#include "../Geometry/Geometry.h"
#include "../Data/DataSet.h"
#include "../Data/Table.h"
#include "../Configuration.h"

#include <cmath>
#include <stdio.h>

//Static declarations
ParallelCoordsView* ParallelCoordsView::instance = NULL;
int ParallelCoordsView::windowHandle = -1;

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, int x, int y, int width, int height, int paddingX, int paddingY, riv::ColorMap& membershipColors,RIVSliderView* sliderView) : RIVDataView(datasetOne,datasetTwo,x,y,width,height,paddingX,paddingY), sliderView(sliderView), membershipColors(membershipColors) {
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
    identifier = "ParallelCoordsView";
    instance = this;
}
ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** datasetOne, RIVDataSet<float,ushort>** datasetTwo, riv::ColorMap& membershipColors, RIVSliderView* sliderView) :
RIVDataView(datasetOne,datasetTwo), sliderView(sliderView),membershipColors(membershipColors)
{
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
    identifier = "ParallelCoordsView";
    instance = this;
}

/** SINGLE DATASET CONSTRUCTORS **/
ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, int x, int y, int width, int height, int paddingX, int paddingY,riv::ColorMap& membershipColors, RIVSliderView* sliderView) : RIVDataView(dataset,x,y,width,height, paddingX, paddingY),sliderView(sliderView),membershipColors(membershipColors) {
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
    identifier = "ParallelCoordsView";
    instance = this;
}

ParallelCoordsView::ParallelCoordsView(RIVDataSet<float,ushort>** dataset, riv::ColorMap& membershipColors, RIVSliderView* sliderView) : RIVDataView(dataset), sliderView(sliderView),membershipColors(membershipColors) {
    if(instance != NULL) {
        throw "Only 1 instance allowed.";
    }
    identifier = "ParallelCoordsView";
    instance = this;
}
//Reset axes according to data
void ParallelCoordsView::resetAxes() {
    
    printf("Resetting parallel coordinates axes...\n");
    
    if(datasetTwo) {
        for(auto& axisGroup : axisGroups) {
            
            RIVTable<float,ushort> *table = (*datasetOne)->GetTable(axisGroup.tableName);
            RIVTable<float,ushort> *otherTable = (*datasetTwo)->GetTable(axisGroup.tableName);
            
            tuple_for_each(axisGroup.axes, [&](auto tAxes) {
                for(auto axis : tAxes) {
                    
                    typedef typename get_template_type<typename std::decay<decltype(*axis->recordOne)>::type>::type Type;
                    
                    axis->recordOne = table->GetRecord<Type>(axis->name);
                    axis->recordTwo = otherTable->GetRecord<Type>(axis->name);
                    
                    axis->ClearHistograms();
                    
                    auto record = axis->recordOne;
                    auto otherRecord = axis->recordTwo;
                    
                    auto minMax = record->MinMax();
                    auto otherMinMax = otherRecord->MinMax();
                    
                    float min = std::min(minMax.first,otherMinMax.first);
                    float max = std::min(minMax.second,otherMinMax.second);
                    
                    axis->minValue = min;
                    axis->maxValue = max;
                }
            });
        }
    }
    else {
        for(auto& axisGroup : axisGroups) {
            RIVTable<float,ushort> *table = (*datasetOne)->GetTable(axisGroup.tableName);
            
            tuple_for_each(axisGroup.axes, [&](auto tAxes) {
                for(auto axis : tAxes) {
                    
                    typedef typename get_template_type<typename std::decay<decltype(*axis->recordOne)>::type>::type Type;
                    
                    axis->recordOne = table->GetRecord<Type>(axis->name);
                    
                    auto record = axis->recordOne;
                    auto minMax = record->MinMax();
                    
                    axis->minValue = minMax.first;
                    axis->maxValue = minMax.second;
                }
            });
        }
    }
}
void ParallelCoordsView::createAxes() {
    printf("Create axes\n");
    axisGroups.clear();
    size_t totalNrRecords = 0;

    int y = 1.5F * paddingY; //View port takes care of startY
    int axisHeight = height - 2.5F * paddingY;
    int bins = 10;
    
    std::vector<std::string> tablesToDisplay = {PATHS_TABLE,INTERSECTIONS_TABLE,LIGHTS_TABLE};
    std::set<std::string> recordsToIgnore = {LIGHT_ID,OCCLUDER_COUNT,OCCLUDER_ID};
    
    for(const std::string& tableName : tablesToDisplay) {
        RIVTable<float,ushort> *table = (*datasetOne)->GetTable(tableName);
        totalNrRecords += table->NumberOfRecords();
    }
    totalNrRecords -= recordsToIgnore.size();
    
    float delta = 1.F / (totalNrRecords - 1) * (width - 2 * paddingX);
    size_t axisIndex = 0;
    const int divisionCount = 2;
    
    if(datasetTwo) {
        for(const std::string& tableName : tablesToDisplay) {
            
            RIVColorProperty* colorPropertyOne = new RIVFixedColorProperty(colors::RED);
            RIVColorProperty* colorPropertyTwo = new RIVFixedColorProperty(colors::BLUE);
            
            RIVTable<float,ushort> *table = (*datasetOne)->GetTable(tableName);
            RIVTable<float,ushort> *otherTable = (*datasetTwo)->GetTable(tableName);
            
            ParallelCoordsAxisGroup<float,ushort> axisGroup(table->GetName(),colorPropertyOne,colorPropertyTwo);
            
            auto recordsTuple = table->GetAllRecords();
            tuple_for_each(recordsTuple, [&](auto tRecords) {
                for(size_t i = 0 ; i < tRecords.size() ; ++i) {
                    auto& record = tRecords.at(i);
                    
                    if(recordsToIgnore.find(record->name) == recordsToIgnore.end()) {
                        
                        typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
                        auto otherRecord = otherTable->GetRecord<Type>(record->name);
                        
                        int x = delta * (axisIndex) + paddingX;
                        
                        //A tuple containing the min and max values of the record
                        auto minMax = record->MinMax();
                        auto otherMinMax = otherRecord->MinMax();
                        
                        float min = std::min(minMax.first,otherMinMax.first);
                        float max = std::min(minMax.second,otherMinMax.second);
                        
                        //If the type is unsigned short, its a discrete value, if there are not too many, use each discrete value as a bin, otherwise clump them together same as floats
                        if(typeid(Type) == typeid(ushort)) {
                            
                            ushort diff = max - min;
                            if(diff > bins*2) {
                                diff = bins;
                            }
                            
                            auto dataHistOne = Histogram<Type>(record->name,min,max,diff);
                            auto dataHistTwo = Histogram<Type>(record->name,min,max,diff);
                            
                            axisGroup.CreateAxis(record, otherRecord, x, y, axisWidth, axisHeight, std::min(minMax.first,otherMinMax.first), std::max(minMax.second,otherMinMax.second), record->name, divisionCount,dataHistOne,dataHistTwo);
                        }
                        else {
                            auto dataHistOne = Histogram<Type>(record->name,min,max,bins);
                            auto dataHistTwo = Histogram<Type>(record->name,min,max,bins);
                            
                            axisGroup.CreateAxis(record,otherRecord, x, y, axisWidth, axisHeight, std::min(minMax.first,otherMinMax.first), std::max(minMax.second,otherMinMax.second), record->name, divisionCount,dataHistOne,dataHistTwo);
                        }
                        axisIndex++;
                    }
                }
            });
            axisGroups.push_back(axisGroup);
        }
    }
    else {
        for(const std::string& tableName : tablesToDisplay) {
            RIVTable<float,ushort> *table = (*datasetOne)->GetTable(tableName);
            RIVColorProperty* colorPropertyOne = new RIVFixedColorProperty(colors::RED);
            ParallelCoordsAxisGroup<float,ushort> axisGroup(table->GetName(),colorPropertyOne);
            auto recordsTuple = table->GetAllRecords();
            tuple_for_each(recordsTuple, [&](auto tRecords) {
                for(size_t i = 0 ; i < tRecords.size() ; ++i) {
                    auto& record = tRecords.at(i);
                    
                    if(recordsToIgnore.find(record->name) == recordsToIgnore.end()) {
                        typedef typename get_template_type<typename std::decay<decltype(*record)>::type>::type Type;
                        
                        int x = delta * (axisIndex) + paddingX;
                        std::string name = record->name;
                        //A tuple containing the min and max values of the record
                        auto minMax = record->MinMax();

                        //If the type is unsigned short, its a discrete value, if there are not too many, use each discrete value as a bin, otherwise clump them together same as floats
                        if(typeid(Type) == typeid(ushort)) {
                            
                            ushort diff = minMax.second - minMax.first;
                            if(diff > bins*2) {
                                diff = bins;
                            }
                            
                            auto dataHistOne = Histogram<Type>(record->name,minMax.first,minMax.second,diff);
                            axisGroup.CreateAxis(record, x, y, axisWidth, axisHeight, minMax.first, minMax.second, name, divisionCount, dataHistOne);
                        }
                        else {
                            auto dataHistOne = Histogram<Type>(record->name,minMax.first,minMax.second,bins);
                            axisGroup.CreateAxis(record, x, y, axisWidth, axisHeight, minMax.first,minMax.second, record->name, divisionCount,dataHistOne);
                        }
                        axisIndex++;
                    }
                }
            });
            axisGroups.push_back(axisGroup);
        }
    }
    printf("Create axes finished\n");
}

void ParallelCoordsView::drawAxes() {
    
    bool drawMembershipBins = true;
    
    for(auto& axisGroup : axisGroups) {
        size_t rowsOne = (*datasetOne)->GetTable(axisGroup.tableName)->NumberOfRows();
        size_t rowsTwo = 0;
        float sizeDiff = 1;
        
        if(datasetTwo) {
            rowsTwo = (*datasetTwo)->GetTable(axisGroup.tableName)->NumberOfRows();
            if(rowsOne && rowsTwo) {
                sizeDiff = rowsTwo / (float)rowsOne;
            }
        }
        //        printf("Size diff = %f\n",sizeDiff);
        tuple_for_each(axisGroup.axes, [&](auto tAxes) {
            for(auto axis : tAxes) {
                
                //                printf("Draw axis %s\n",axis->name.c_str());
                
                //Draw basic black backdrop for each axis
                glColor3f(1,1,1);
                glColor3f(0,0,0);
                
                float halfWidth = axis->width / 2.F;
                glRectf(axis->x - halfWidth, axis->y, axis->x + halfWidth, axis->y + axis->height);
                
                bool hasSelectionBox = axis->HasSelectionBox;
                
                int startSelectionBin = -1;
                int endSelectionBin = -1;
                
                float maxBinWidth = 3.5 * axis->width;
//                float maxBinWidth = 1;
                
                auto& histogramOne = axis->densityHistogramOne;
                auto& histogramTwo = axis->densityHistogramTwo;
                
                int numBins = histogramOne.NumberOfBins();
                float height = axis->height / (float)numBins;
                
                if(hasSelectionBox) {
                    float selectionStartY = axis->ScaleValueForY(axis->selection.start.y);
                    float selectionEndY = axis->ScaleValueForY(axis->selection.end.y);
                    
                    //                    printf("selectionStartY,selectionEndY = %f,%f\n",selectionStartY,selectionEndY);
                    
                    startSelectionBin = std::floor(selectionStartY * numBins);
                    endSelectionBin = std::floor(selectionEndY * numBins);
                    
                    if(endSelectionBin < startSelectionBin) {
                        std::swap(startSelectionBin,endSelectionBin);
                    }
                    //                    glColor4f(0,0,0,0.3);
                    //                    glRectf(axis->selection.start.x - 10, axis->selection.start.y, axis->selection.end.x + 10, axis->selection.end.y);
                    //                    printf("Start,end selection bin = %d,%d\n",startSelectionBin,endSelectionBin);
                }
                
                float maxValueOne = histogramOne.MaximumValue() * sizeDiff;
                float maxValueTwo = histogramTwo.MaximumValue();
                
                //                float max = std::min(maxValueOne,maxValueTwo);
                //
                for(int bin = 0 ; bin < numBins ; ++bin) {
                    //                    printf("bin = %d\n",i);
                    
                    int startBinY = bin * height + axis->y;
                    int endBinY = startBinY + height;
                    
                    float binValueOne = histogramOne.BinValue(bin) * sizeDiff;
                    float binValueTwo = histogramTwo.BinValue(bin);
                    
                    //					printf("bin value one = %f\n",binValueOne);
                    //					printf("bin value two = %f\n",binValueTwo);
                    
                    size_t nrElementsOne = histogramOne.NumberOfElements();
                    size_t nrElementsTwo = histogramTwo.NumberOfElements();
                    //                    size_t maxNrElements = std::max(nrElementsOne,nrElementsTwo);
                    
                    if(nrElementsOne || nrElementsTwo) {
                        
                        float normalizedValueOne = 0;
                        float normalizedValueTwo = 0;
                        
                        if(nrElementsOne) {
                            normalizedValueOne = binValueOne / nrElementsOne;
                        }
                        if(nrElementsTwo) {
                            normalizedValueTwo = binValueTwo / nrElementsTwo;
                        }
                        riv::Color binColor;
                        //Determine the color by membership
                        
                        float blueColorValue;
                        float redColorValue;
                        float ratio = 0;
                        
                        if(binValueTwo > binValueOne) {
                            blueColorValue = ((binValueTwo - binValueOne) / binValueTwo + 1) / 2.F;
                            redColorValue = 1-blueColorValue;
                            ratio = binValueTwo / maxValueTwo;
                            
                            if(drawMembershipBins) {
                                if(useSaturation) {
                                    float h,s,v;
                                    RGBtoHSV(redColorValue, 0, blueColorValue, &h, &s, &v);
                                    float r,g,b;
                                    HSVtoRGB(&r, &g, &b, h, ratio, v);
                                    binColor = riv::Color(r,g,b);
                                }
                                else {
                                    binColor = membershipColors.ComputeColor(blueColorValue);
                                }
                            }
                        }
                        else if(binValueOne > binValueTwo) {
                            redColorValue = ((binValueOne - binValueTwo) / binValueOne + 1) / 2.F;
                            blueColorValue = 1 - redColorValue;
                            ratio = binValueOne / maxValueOne;
                            
                            if(drawMembershipBins) {
                                if(useSaturation) {
                                    float h,s,v;
                                    RGBtoHSV(redColorValue, 0, blueColorValue, &h, &s, &v);
                                    float r,g,b;
                                    HSVtoRGB(&r, &g, &b, h, ratio, v);
                                    binColor = riv::Color(r,g,b);
                                }
                                else {
                                    binColor = membershipColors.ComputeColor(blueColorValue);
                                }
                            }
                        }
                        else {
                            ratio = 0;
                            binColor = riv::Color(1.0F,1.0F,1.F);
                        }
                        if(!drawMembershipBins) {
                            binColor = axis->GetBinRadiance(bin);
                        }
                        
                        float width = ratio * (maxBinWidth - axis->width) + axis->width;
                        
                        float startBinX = axis->x - width / 2.F;
                        float endBinX = axis->x + width / 2.F;
                        
                        glColor3f(binColor.R,binColor.G,binColor.B);
                        glRectf(startBinX, startBinY, endBinX, endBinY);
                        
                        glColor3f(0,0,0);
                        glLineWidth(2);
                        glBegin(GL_LINE_LOOP);
                        
                        glVertex2f(startBinX, startBinY);
                        glVertex2f(endBinX, startBinY);
                        glVertex2f(endBinX, endBinY);
                        glVertex2f(startBinX, endBinY);
                        
                        glEnd();
                        
                        
                        if(axis->HasSelectionBox) {
                            //Draw a thicker black line around the edges of the bin when it is brushed/selected
                            const int selectionLineWidth = 8;
                            glLineWidth(selectionLineWidth);
                            
                            if(bin >= startSelectionBin && bin <= endSelectionBin) {
                                //The points of the selection rectangle for this particular bin
                                int selectionBinStartX = startBinX - selectionLineWidth / 2;
                                int selectionBinEndX = endBinX + selectionLineWidth / 2;
                                int selectionBinStartY;
                                int selectionBinEndY;
                                if(endSelectionBin == bin) {
                                    selectionBinStartY = axis->selection.start.y;
                                    if(bin == startSelectionBin) {
                                        selectionBinEndY = axis->selection.end.y;
                                    }
                                    else {
                                        selectionBinEndY = startBinY;
                                    }
                                }
                                else if(bin == startSelectionBin) {
                                    selectionBinStartY = endBinY;
                                    selectionBinEndY = axis->selection.end.y;
                                }
                                else {
                                    selectionBinStartY = startBinY;
                                    selectionBinEndY = endBinY;
                                }
                                glColor4f(0,0,0,0.4);
                                glBegin(GL_LINES);
                                glVertex2f(selectionBinStartX,selectionBinStartY);
                                glVertex2f(selectionBinStartX,selectionBinEndY);
                                glVertex2f(selectionBinEndX,selectionBinEndY);
                                glVertex2f(selectionBinEndX,selectionBinStartY);
                                glEnd();
                            }
                        }
                    }
                }
            }
        });
        
    }
}

void ParallelCoordsView::drawAxesExtras() {
    
    glColor3f(1,0,0);
    glLineWidth(1.F);
    
    float textColor[3] = {0,0,0};
    
    //Half of its size
    int selectionGlyphSize = 7;
    
    //Draw texts and extras
    for(auto& axisGroup : axisGroups) {
        //Draw name of table in middle
        auto firstAxis = axisGroup.axisInterfaces[0];
        auto lastAxis = axisGroup.axisInterfaces[axisGroup.axisInterfaces.size() - 1];
        int xCenter = (lastAxis->x - firstAxis->x) / 2.F + firstAxis->x;
        //        printf("xCenter = %d\n",xCenter);
        drawText(axisGroup.tableName,xCenter,firstAxis->y - 30,.15);
        tuple_for_each(axisGroup.axes, [&](auto tAxes) {
            
            for(auto& axis : tAxes) {
                
                //Draw the name of the data dimension at the bottom of the axis
                std::string& text = axis->name;
                drawText(text,axis->x,axis->y - 12,textColor,.08F);
                
                //Draw a selection glyph if it is selected
                if(axis->isSelected) {
                    int glyphY = axis->y - 20;
                    
                    glBegin(GL_POLYGON);
                    glVertex2f(axis->x - selectionGlyphSize,glyphY - selectionGlyphSize);
                    glVertex2f(axis->x + selectionGlyphSize,glyphY - selectionGlyphSize);
                    glVertex2f(axis->x,glyphY);
                    glEnd();
                    //					glRectf(axis->x - selectionGlyphSize, glyphY - selectionGlyphSize, axis->x + selectionGlyphSize, glyphY + selectionGlyphSize);
                }
                
                auto& scale = axis->scale;
                
                //Draw the scales indicators
            }
        });
    }
    this->Invalidate();
}
void ParallelCoordsView::createAxisPoints() {
    //	printf("CREATE AXIS POINTS!!!\n");
    createAxisPoints(0, *datasetOne);
    if(datasetTwo) {
        createAxisPoints(1, *datasetTwo);
    }
}
void ParallelCoordsView::createAxisDensities() {
    
    for(auto &axisGroup : axisGroups) {
        tuple_for_each(axisGroup.axes, [&](auto tAxes) {
            for(auto axis : tAxes) {
                axis->ClearHistograms();
            }
        });
    }
    
    createAxisDensities(0, *datasetOne);
    if(datasetTwo) {
        createAxisDensities(1, *datasetTwo);
    }
}
//void ParallelCoordsView::CycleColorScheme() {


//    delete pathColorOne;
//    delete rayColorOne;
//
//
//    delete pathColorTwo;
//}
void ParallelCoordsView::createAxisDensities(int datasetId, RIVDataSet<float,ushort>* dataset) {
    
    std::map<std::string,std::vector<std::string>> tableToRadianceRecords;
    tableToRadianceRecords[PATHS_TABLE] = {PATH_R,PATH_G,PATH_B};
    tableToRadianceRecords[INTERSECTIONS_TABLE] = {INTERSECTION_R,INTERSECTION_G,INTERSECTION_B};
    tableToRadianceRecords[LIGHTS_TABLE] = {LIGHT_R,LIGHT_G,LIGHT_B};
    
    for(auto &axisGroup : axisGroups) {
        
        auto table = dataset->GetTable(axisGroup.tableName);
        
        RIVRecord<float>* rRecord = table->GetRecord<float>(tableToRadianceRecords[table->GetName()][0]);
        RIVRecord<float>* gRecord = table->GetRecord<float>(tableToRadianceRecords[table->GetName()][1]);
        RIVRecord<float>* bRecord = table->GetRecord<float>(tableToRadianceRecords[table->GetName()][2]);
        
        tuple_for_each(axisGroup.axes, [&](auto& tAxes) {
            for(auto& axis : tAxes) {
                
                TableIterator* iterator = table->GetIterator();
                size_t row;
                
                while(iterator->GetNext(row)) {
                    
                    auto record = table->GetRecord<decltype(axis->minValue)>(axis->name);
                    auto value = record->Value(row);
                    
                    unsigned int bin = axis->GetHistogram(datasetId)->Add(value);
                    
                    //                    axis->AddRadiance(bin,rRecord->Value(row),gRecord->Value(row),bRecord->Value(row));
                    
                    riv::Color color;
                    if(axisGroup.colorPropertyOne->ComputeColor(table, row, color)) {
                        axis->AddRadiance(bin,color.R,color.G,color.B);
                    }
                    
                    if(axisGroup.colorPropertyTwo && axisGroup.colorPropertyTwo->ComputeColor(table, row, color)) {
                        axis->AddRadiance(bin,color.R,color.G,color.B);
                    }
                }
            }
        });
    }
}
void ParallelCoordsView::createAxisPoints(int datasetId, RIVDataSet<float,ushort>* dataset) {
    printf("CREATE AXIS POINTS FOR DATASET  #%d\n",datasetId);
    for(auto &axisGroup : axisGroups) {
        tuple_for_each(axisGroup.axes, [&](auto tAxes) {
            for(auto axis : tAxes) {
                axis->GetPoints(datasetId).clear();
            }
        });
    }
    size_t pointsCreated = 0;
    for(auto &axisGroup : axisGroups) {
        size_t row = 0;
        
        auto table = dataset->GetTable(axisGroup.tableName);
        TableIterator* iterator = table->GetIterator(true);
        pointsCreated = 0;
        while(iterator->GetNext(row)) {
            ++pointsCreated;
            tuple_for_each(axisGroup.axes, [&](auto& tAxes) {
                for(auto& axis : tAxes) {
                    auto record = table->GetRecord<decltype(axis->minValue)>(axis->name);
                    auto value = record->Value(row);
                    
                    axis->GetPoints(datasetId).push_back(axis->PositionOnScaleForValue(value));
                    
                }
            });
        }
        
    }
    printf("%zu points per axis created.\n",pointsCreated);
    
}
void ParallelCoordsView::drawLines(int datasetId, RIVDataSet<float,ushort>* dataset) {
    //	linesAreDirty = true;
    
    //	dataset->Print();
    
    size_t lines = 0;
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /** AD HOC MEMBERSHIP COLORING PROPERTY CREATION **/
    
    bool pathMembershipDataPresent = false;
    bool isectMembershipDataPresent = false;
    
    RIVTable<float,ushort>* pathMembershipTable = dataset->GetTable(PATH_MEMBERSHIP_TABLE);
    RIVTable<float,ushort>* isectMembershipTable = dataset->GetTable(ISECT_MEMBERSHIP_TABLE);
    
    RIVFloatRecord* pathMembershipRecord = NULL;
    RIVFloatRecord* isectMembershipRecord = NULL;
    
    riv::ColorMap colorMap;
    colorMap.AddColor(colors::RED);
    colorMap.AddColor(colors::BLUE);
    
    if(pathMembershipTable) {
        pathMembershipRecord = pathMembershipTable->GetRecord<float>(MEMBERSHIP);
        
        if(pathMembershipRecord != NULL && pathMembershipRecord->Size()) {
            //            pathMembershipDataPresent = true;
        }
    }
    
    if(isectMembershipTable) {
        isectMembershipRecord = isectMembershipTable->GetRecord<float>(MEMBERSHIP);
        if(isectMembershipRecord) {
            //            isectMembershipDataPresent = isectMembershipRecord->Size();
        }
    }
    
    glLineWidth(1);
    for(auto &axisGroup : axisGroups) {
        auto table = dataset->GetTable(axisGroup.tableName);
        size_t numRows = table->NumberOfRows();
        size_t row = 0;
        //        if(numRows != axisGroup.axisInterfaces[0]->GetPoints(datasetId).size()) {
        //            printf("ERROR: Points not yet present...\n");
        //            return;
        //        }
        
        TableIterator* iterator = table->GetIterator();
        
        //Find what color property to use for this table
        RIVColorProperty* colorProperty = axisGroup.GetColorProperty(datasetId);
        
        //You gotta love 'auto'!
        size_t lineIndex = 0;
        riv::Color lineColor;
        while(iterator->GetNext(row)) {
            float offset = axisWidth / 2.F;
            if(colorProperty->ComputeColor(table, row, lineColor)) {
                
                glColor4f(lineColor.R, lineColor.G, lineColor.B, lineOpacity);
                glBegin(GL_LINE_STRIP);
                
                for(auto axis : axisGroup.axisInterfaces) {
                    
                    float x = axis->x;
                    float y = axis->GetPoints(datasetId)[row];
                    
                    glVertex2f(x - offset, y);
                    glVertex2f(x + offset, y);
                }
                
                glEnd();
                ++lineIndex;
            }
        }
        lines += lineIndex;
    }
    printf("Parallel coordinates view drew %zu polylines\n",lines);
    //	reporter::stop("drawLines");
}

void ParallelCoordsView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
}

void ParallelCoordsView::ReshapeInstance(int width, int height) {
    if(instance != NULL) {
        instance->Reshape(width,height);
    }
}

void ParallelCoordsView::Mouse(int button, int state, int x, int y) {
    if(instance != NULL) {
        instance->HandleMouse(button,state,x,y);
    }
}

void ParallelCoordsView::Motion(int x, int y) {
    if(instance != NULL) {
        instance->HandleMouseMotion(x, y);
    }
}

void ParallelCoordsView::Reshape(int width, int height) {
    //    printf("Reshape Parallel Coords View!\n");
    
    this->width = width;
    this->height = height;
    
    printf("New width = %d\n",width);
    printf("New height = %d\n",height);
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    paddingX = 30;
    paddingY = 30;
    
    createAxes();
    this->Invalidate();
}

size_t drawCount = 0;
void ParallelCoordsView::Draw() {
    printHeader("PARALLEL COORDINATES PLOT VIEW DRAW");
    
    std::string taskName = "ParallelCoordsView Draw";
    reporter::startTask(taskName);
    printf("\n");
    
    if(needsRedraw) {
        printf("Clear PCV window\n");
        //		glClearColor(0.9, 0.9, 0.9, 0.0);
        glClearColor(backgroundColor.R,backgroundColor.G,backgroundColor.B,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        if(drawDataSetOne) {
            printf("Draw lines for dataset one\n");
            drawLines(0,*datasetOne);
        }
        if(datasetTwo && drawDataSetTwo) {
            printf("Draw lines for dataset two\n");
            drawLines(1,*datasetTwo);
        }
        drawAxes();
        drawAxesExtras();
        glFlush();
        glutSwapBuffers();
    }
    
    reporter::stop(taskName);
}

void ParallelCoordsView::ToggleDrawDataSetOne() {
    drawDataSetOne = !drawDataSetOne;
    
    this->Invalidate();
    
    redisplayWindow();
}

void ParallelCoordsView::ToggleDrawDataSetTwo() {
    drawDataSetTwo = !drawDataSetTwo;
    
    this->Invalidate();
    
    redisplayWindow();
}
//Helper functions to create color property for a given dataset
RIVColorRGBProperty<float>* ParallelCoordsView::createPathEnergyColorProperty(RIVDataSet<float,ushort>* dataset) {
    RIVTable<float,ushort> *pathsTable = dataset->GetTable(PATHS_TABLE);
    
    RIVRecord<float>* pathRRecord = pathsTable->GetRecord<float>(PATH_R);
    RIVRecord<float>* pathGRecord = pathsTable->GetRecord<float>(PATH_G);
    RIVRecord<float>* pathBRecord = pathsTable->GetRecord<float>(PATH_B);
    
    //	riv::ColorMap jetColorMap = colors::jetColorMap();
    return new RIVColorRGBProperty<float>(pathsTable,pathRRecord,pathGRecord,pathBRecord);
}

RIVColorRGBProperty<float>* ParallelCoordsView::createRayEnergyColorProperty(RIVDataSet<float,ushort>* dataset) {
    RIVTable<float,ushort> *intersectionsTable = dataset->GetTable(INTERSECTIONS_TABLE);
    
    RIVRecord<float>* isectRRecord = intersectionsTable->GetRecord<float>(INTERSECTION_R);
    RIVRecord<float>* isectGRecord = intersectionsTable->GetRecord<float>(INTERSECTION_G);
    RIVRecord<float>* isectBrRecord = intersectionsTable->GetRecord<float>(INTERSECTION_B);
    
    //	riv::ColorMap jetColorMap = colors::jetColorMap();
    return new RIVColorRGBProperty<float>(intersectionsTable,isectRRecord,isectGRecord,isectBrRecord);
}
bool ParallelCoordsView::HandleMouse(int button, int state, int x, int y) {
    //    printf("PCV HandleMouse\n");
    //    ToViewSpaceCoordinates(&x, &y);
    y = height - y;
    if(containsPoint(x,y)) {
        //What axis was selected
        if(state == GLUT_DOWN) {
            int proximityMax = 50;
            
            mouseDownX = x;
            mouseDownY = y;
            
            for(int i = 0 ; i < axisGroups.size() ; i++) {
                
                auto& axisGroup = axisGroups[i];
                bool axisFound = false;
                tuple_for_each(axisGroup.axes, [&](auto tAxes) {
                    for(auto& axis : tAxes) {
                        
                        int distanceX = abs(axis->x - x);
                        
                        if(distanceX < proximityMax) {
                            selectedAxis = axis;
                            //Select the
                            if(axis->HasSelectionBox && ((y >= axis->selection.start.y && y <= axis->selection.end.y) || (y >= axis->selection.end.y && y <= axis->selection.start.y))) {
                                //Is it inside the selection box?
                                
                                interactionState = MOUSE_DOWN_SELECTION;
                                
                                printf("NEW STATE IS MOUSE_DOWN_SELECTION\n");
                                
                                axisFound = true;
                                
                                return;
                            }
                            //Is it at least inside the axis
                            else if(y > axis->y && y < axis->y + axis->height) {
                                
                                interactionState = MOUSE_DOWN_AXIS;
                                
                                printf("NEW STATE IS MOUSE_DOWN_AXIS\n");
                                //Possibly create a selection box or swap axes
                                
                                //Close enough..
                                axisFound = true;
                                
                                return;
                                
                            }
                            else {
                                
                                interactionState = MOUSE_DOWN_AXIS_EXTRA;
                                
                                printf("NEW STATE IS MOUSE_DOWN_AXIS_EXTRA\n");
                                
                                selectedAxis = axis;
                                
                                //Create new color property from selected axis
                                axis->isSelected = !axis->isSelected;
                                
                                typedef typename get_template_type<typename std::decay<decltype(*axis)>::type>::type Type;
                                if(datasetOne) {
                                    auto table = (*datasetOne)->GetTable(axisGroup.tableName);
                                    RIVRecord<Type>* axisRecord = table->GetRecord<Type>(axis->name);
//                                    if(axisGroup.colorPropertyOne) {
//                                        delete axisGroup.colorPropertyOne;
//                                    }
//                                    axisGroup.colorPropertyOne = new RIVEvaluatedColorProperty<Type>(colors::blueYellowColorMap(),(*datasetOne)->GetTable(axisGroup.tableName),axisRecord);
                                }
                                if(datasetTwo) {
                                    auto table = (*datasetTwo)->GetTable(axisGroup.tableName);
                                    RIVRecord<Type>* axisRecord = table->GetRecord<Type>(axis->name);
//                                    if(axisGroup.colorPropertyTwo) {
//                                        delete axisGroup.colorPropertyTwo;
//                                    }
//                                    axisGroup.colorPropertyTwo = new RIVEvaluatedColorProperty<Type>(colors::blueYellowColorMap(),(*datasetOne)->GetTable(axisGroup.tableName),axisRecord);
                                }
                                
                                //
                                //                                if(sliderView) {
                                //                                    if(axis->isSelected) {
                                //                                        axis->isSelected = false;
                                //                                        sliderView->RemoveSelectedRecord(axis->name);
                                //                                    }
                                //                                    else {
                                //                                        axis->isSelected = true;
                                //                                        sliderView->AddSelectedRecord(axisGroup.tableName, axis->name);
                                //                                    }
                                //                                }
                                
                                this->Invalidate();
                                
                                axisFound = true;
                                
                                glutPostRedisplay();
                                
                                return;
                            }
                        }
                    }
                });
                if(axisFound) {
                    return true;
                }
            }
        }
        else if(state == GLUT_UP) { //Finish selection
            
            switch (interactionState) {
                case DRAG_AXIS: {
                    //					int distance = std::abs(x - mouseDownX);
                    int minDistance = 25;
                    
                    ParallelCoordsAxisInterface* swapAxis = NULL;
                    //				if(distance > 25) {
                    //Should the axis be swapped with another axis?
                    for(auto& axisGroup : axisGroups) {
                        bool axisGroupFound = false;
                        int shortestDistance = 10000;
                        int axisIndex = 0;
                        int swapAxisIndex = 0;
                        for(size_t i = 0 ; i < axisGroup.axisInterfaces.size() ; ++i) {
                            ParallelCoordsAxisInterface* axis = axisGroup.axisInterfaces[i];
                            if(axis->name == selectedAxis->name) {
                                axisGroupFound = true;
                                axisIndex = i;
                            }
                            else {
                                int distance = std::abs(axis->x - selectedAxis->x);
                                if(distance < shortestDistance) {
                                    shortestDistance = distance;
                                    swapAxis = axis;
                                    swapAxisIndex = i;
                                }
                            }
                            if(axisGroupFound && swapAxis != NULL && shortestDistance < minDistance) {
                                selectedAxis->x = axisOriginX;
                                axisGroup.SwapAxes(axisIndex,swapAxisIndex);
                                
                                this->Invalidate();
                                
                                glutPostRedisplay();
                                
                                printf("SWAP SELECTED AXIS %s WITH AXIS %s\n",selectedAxis->name.c_str(), swapAxis->name.c_str());
                                
                                return true;
                            }
                        }
                    }
                    //				}
                    break;
                }
                case DRAG_SELECTION:
                    filterData();
                    break;
                case CREATE_SELECTION_BOX:
                    filterData();
                    break;
                case MOUSE_DOWN_SELECTION:
                    for(auto& axisGroup : axisGroups) {
                        tuple_for_each(axisGroup.axes, [&](auto tAxes) {
                            for(auto& axis : tAxes) {
                                if(axis->name == selectedAxis->name) {
                                    axis->HasSelectionBox = false;
                                    typedef typename get_template_type<typename std::decay<decltype(*axis)>::type>::type Type;
                                    (*datasetOne)->StartFiltering();
                                    if(datasetTwo) {
                                        (*datasetTwo)->StartFiltering();
                                    }
                                    (*datasetOne)->ClearFilter<Type>(axis->name);
                                    if(datasetTwo) {
                                        (*datasetTwo)->ClearFilter<Type>(axis->name);
                                    }
                                    (*datasetOne)->StopFiltering();
                                    if(datasetTwo) {
                                        (*datasetTwo)->StopFiltering();
                                    }
                                }
                            }
                        });
                    }
                    break;
                default:
                    break;
            }
            
            printf("NEW STATE IS IDLE\n");
            interactionState = IDLE;
            return true;
        }
        return true;
    }
    return false;
}
void ParallelCoordsView::ToggleSaturationMode() {
    useSaturation = !useSaturation;
    
    this->Invalidate();
    
    redisplayWindow();
}
bool ParallelCoordsView::HandleMouseMotion(int x, int y) {
    y = height - y;
    
    int yDistanceTravelled = std::abs(y - mouseDownY);
    int xDistanceTravelled = std::abs(x - mouseDownX);
    
    printf("Distance travelled = (%d,%d)\n",xDistanceTravelled, yDistanceTravelled);
    
    switch (interactionState) {
        case IDLE:
            return true;
        case MOUSE_DOWN_AXIS_EXTRA:
            return true;
        case MOUSE_DOWN_AXIS:
            if(yDistanceTravelled > dragStartSensitivity) {
                
                interactionState = CREATE_SELECTION_BOX;
                printf("NEW STATE IS CREATE_SELECTION_BOX\n");
                
                selectedAxis->HasSelectionBox = true;
                selectedAxis->selection.start.x = selectedAxis->x - 10;
                selectedAxis->selection.end.x = selectedAxis->x + 10;
                selectedAxis->selection.start.y = y;
                selectedAxis->selection.end.y = y;
                
                //                selection = &selectedAxis->selection;
                
                selectionBoxChanged = false;
                this->Invalidate();
                
                axisUpdateY = y;
                return true;
            }
            else if(xDistanceTravelled > dragStartSensitivity) {
                interactionState = DRAG_AXIS;
                printf("NEW STATE IS DRAG_AXIS\n");
                
                selectionBoxChanged = false;
                this->Invalidate();
                
                axisOriginX = x;
                axisUpdateX = x;
                
                return true;
            }
        case MOUSE_DOWN_SELECTION:
            if(yDistanceTravelled > 5) {
                printf("NEW STATE IS DRAG_SELECTION\n");
                axisUpdateY = y;
                interactionState = DRAG_SELECTION;
                dragBoxLastY = y;
            }
            return true;
        case CREATE_SELECTION_BOX:
            printf("CREATE SELECTION...\n");
            //TODO: Not very efficient to find the axis everytime, but pointer sucks due to templates
            for(auto& axisGroup : axisGroups) {
                tuple_for_each(axisGroup.axes, [&](auto tAxes) {
                    for(auto& axis : tAxes) {
                        if(selectedAxis && axis->name == selectedAxis->name) {
                            if(y < axis->selection.start.y) {
                                axis->selection.end.y = axis->PositionOnScaleForViewY(y);
                            }
                            else {
                                axis->selection.start.y = axis->PositionOnScaleForViewY(y);
                            }
                        }
                    }
                });
            }
            if(std::abs(y - axisUpdateY) > updateSensitivity) {
                axisUpdateY = y;
                filterData();
                this->Invalidate();
                selectionBoxChanged = true;
            }
            this->Invalidate();
            glutPostRedisplay();
            return true;
        case DRAG_SELECTION: {
            printf("DRAG SELECTION...\n");
            int deltaY = y - dragBoxLastY;
            for(auto& axisGroup : axisGroups) {
                tuple_for_each(axisGroup.axes, [&](auto tAxes) {
                    for(auto& axis : tAxes) {
                        if(axis->name == selectedAxis->name) {
                            axis->selection.start.y += deltaY;
                            axis->selection.end.y += deltaY;
                        }
                    }
                });
            }
            if(std::abs(y - axisUpdateY) > updateSensitivity) {
                axisUpdateY = y;
                filterData();
                this->Invalidate();
                selectionBoxChanged = true;
            }
            dragBoxLastY = y;
            glutPostRedisplay();
            return true;
        }
        case DRAG_AXIS: {
            if(std::abs(x - axisUpdateX) > updateSensitivity) {
                axisUpdateX = x;
                selectedAxis->x = x;
                
                this->Invalidate();
                
                glutPostRedisplay();
            }
            return true;
        }
    }
    return false;
}


void ParallelCoordsView::filterData() {
    for(auto& axisGroup : axisGroups) {
        tuple_for_each(axisGroup.axes, [&](auto tAxes) {
            for(auto& axis : tAxes) {
                if(selectedAxis && axis->name == selectedAxis->name) {
                    int sizeBox = abs(axis->selection.start.y - axis->selection.end.y);
                    (*datasetOne)->StartFiltering();
                    if(datasetTwo) {
                        (*datasetTwo)->StartFiltering();
                    }
                    //A super clever way of deducing the type of the template parameter of the axis that was selected so we can clear/create a filter of the same type
                    typedef typename get_template_type<typename std::decay<decltype(*axis)>::type>::type Type;
                    if(sizeBox > 3) {
                        auto lowerBound = axis->ValueOnScale(axis->ScaleValueForY(axis->selection.start.y));
                        auto upperBound = axis->ValueOnScale(axis->ScaleValueForY(axis->selection.end.y));
                        
                        (*datasetOne)->ClearFilter<Type>(axis->name);
                        if(datasetTwo) {
                            (*datasetTwo)->ClearFilter<Type>(axis->name);
                        }
                        
                        riv::SingularFilter<Type>* rangeFilter = new riv::RangeFilter<Type>(axis->name,lowerBound,upperBound);
                        printf("New filter on axis %s : ",axis->name.c_str());
                        rangeFilter->Print();
                        (*datasetOne)->AddFilter(rangeFilter);
                        if(datasetTwo) {
                            (*datasetTwo)->AddFilter(rangeFilter);
                        }
                        
                        //                        latestSelectionBox = &axis->selection;
                    }
                    else {
                        (*datasetOne)->ClearFilter<Type>(axis->name);
                        if(datasetTwo) {
                            (*datasetTwo)->ClearFilter<Type>(axis->name);
                        }
                        axis->HasSelectionBox = false;
                        selectedAxis = NULL;
                    }
                    //Close access
                    (*datasetOne)->StopFiltering();
                    if(datasetTwo) {
                        (*datasetTwo)->StopFiltering();
                    }
                }
            }
        });
    }
}

void ParallelCoordsView::OnDataChanged(RIVDataSet<float,ushort>* source) {
    
    //Recreate the axes
    printf("\n**   ParallelCoordsView onDataChanged notified.\n");
    
    this->Invalidate();
    resetAxes();
    createAxisDensities();
    
    //Recreate the color property
    if(source == (*datasetOne)) {
        printf("Dataset one was changed.\n\n");
        //        auto rgbPath = dynamic_cast<RIVColorRGBProperty<float>*>(pathColorOne);
        //        auto rgbRay = dynamic_cast<RIVColorRGBProperty<float>*>(rayColorOne);
        //        //        createAxisPoints(0, *datasetOne);
        //        if(rgbPath) {
        //            auto pathTable = source->GetTable(PATHS_TABLE);
        //            auto isectTable = source->GetTable(INTERSECTIONS_TABLE);
        //            rgbPath->SetColorRecords(pathTable->GetRecord<float>(PATH_R), pathTable->GetRecord<float>(PATH_G), pathTable->GetRecord<float>(PATH_B));
        //            rgbRay->SetColorRecords(isectTable->GetRecord<float>(INTERSECTION_R), isectTable->GetRecord<float>(INTERSECTION_G), isectTable->GetRecord<float>(INTERSECTION_B));
        //        }
        //        else {
        for(auto& axisGroup : axisGroups) {
            axisGroup.colorPropertyOne->Reset(source);
        }
        //        }
    }
    else if(source == (*datasetTwo)) {
        printf("Dataset Two was changed.\n\n");
        //        auto rgbPath = dynamic_cast<RIVColorRGBProperty<float>*>(pathColorTwo);
        //        auto rgbRay = dynamic_cast<RIVColorRGBProperty<float>*>(rayColorTwo);
        //        //        createAxisPoints(1, *datasetTwo);
        //        if(rgbPath) {
        //            auto pathTable = source->GetTable(PATHS_TABLE);
        //            auto isectTable = source->GetTable(INTERSECTIONS_TABLE);
        //            rgbPath->SetColorRecords(pathTable->GetRecord<float>(PATH_R), pathTable->GetRecord<float>(PATH_G), pathTable->GetRecord<float>(PATH_B));
        //            rgbRay->SetColorRecords(isectTable->GetRecord<float>(INTERSECTION_R), isectTable->GetRecord<float>(INTERSECTION_G), isectTable->GetRecord<float>(INTERSECTION_B));
        //        }
        //        else {
        for(auto& axisGroup : axisGroups) {
            axisGroup.colorPropertyTwo->Reset(source);
        }
        //        }
    }
    else {
        throw std::runtime_error("Unknown dataset");
    }
    
    createAxisPoints();
    
    //Change the ordering to have the bounce_nr to be the first of the second axis group
    //	axisGroups[1].Reorder(6,0);
    redisplayWindow();
}
void ParallelCoordsView::OnDataStructureChanged(RIVDataSet<float, ushort>* dataset) {
    if(dataset == *datasetOne || dataset == *datasetTwo) {
        createAxes();
        createAxisDensities();
        createAxisPoints();
        redisplayWindow();
        
        
    }
}
void ParallelCoordsView::redisplayWindow() {
    int currentWindow = glutGetWindow();
    glutSetWindow(ParallelCoordsView::windowHandle);
    glutPostRedisplay();
    //Return window to given window
    glutSetWindow(currentWindow);
}

void ParallelCoordsView::OnFiltersChanged(RIVDataSet<float,ushort>* dataset) {
    printf("ParallelCoordsView received a on filters changed callback.\n");
    if(dataset == *datasetOne) {
        printf("Recreate densities for dataset 1\n");
        createAxisDensities(0, dataset);
    }
    else if(datasetTwo && dataset == *datasetTwo) {
        printf("Recreate densities for dataset 2\n");
        createAxisDensities(1, dataset);
    }
    else {
        throw std::runtime_error("Unknown dataset\n");
    }
    this->Invalidate();
    
    redisplayWindow();
}
void ParallelCoordsView::ToggleBackgroundColor() {
    
    if(backgroundColor == colors::WHITE) {
        backgroundColor = colors::BLACK;
    }
    else if(backgroundColor == colors::BLACK) {
        backgroundColor = colors::WHITE;
    }
    
    redisplayWindow();
}
bool ParallelCoordsView::DecreaseLineOpacity() {
    
    lineOpacity /= 1.5;
    if(lineOpacity < 0.01) {
        lineOpacity = 0.01;
    }
    redisplayWindow();
    return true;
}
bool ParallelCoordsView::IncreaseLineOpacity() {
    
    lineOpacity *= 1.5;
    redisplayWindow();
    return true;
}