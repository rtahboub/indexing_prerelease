/*
 * Copyright 2012 Cornell Database Group
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "2D/workload/TraceWorkload2D.h"
#include "2D/Workload2D.h"
#include "2D/TestTypes2D.h"
#include "IndexLog.h"

using namespace PSimIndex;
using namespace DBUtl;

namespace /* anon */ {
    int binsX;
    int binsY;
    int spaceX;
    int spaceY;

    const char* plotdir = "out/eps/";
    char plotName[200];

    void throwUsage(const char* msg) {
        throw std::logic_error(msg);
    }

    void applyUpdates(Workload2D* workload, int tickNum, int& numPosUpdates, int& numVelUpdates) {

        while(workload->hasNextUpdate()) {

            UpdateRec* update = workload->nextUpdate();
            Point2D* point = update->point;
        
            switch(update->type) {

            case VEL_UPDATE:
                numVelUpdates++;
                point->vx = update->newVx;
                point->vy = update->newVy;
                break;
                
            case AGG_UPDATE:
                point->data = update->newAgg;
                break;

            case POS_UPDATE:
                numPosUpdates++;
                if(update->newX < 0 || update->newY < 0) {
                    LOG(ERR) << "ERROR t" << tickNum
                             << ": Negative position update: " 
                             << *update << std::endl;
                }

                point->x = update->newX;
                point->y = update->newY;
                point->vx = update->newVx;
                point->vy = update->newVy;

                break;

            case INSERT:
                if(point->x < 0 || point->y < 0) {
                    LOG(ERR) << "ERROR t" << tickNum
                             << ": Inserting point with negative coordinate: "
                             << *point << std::endl;
                }

                workload->insertPoint(point);
                break;

            case DELETE:
                workload->deletePoint(point);
                break;


            case IMPLICIT_MOVE:
                if((point->x + point->vx) < 0 || (point->y + point->vy) < 0) {
                   LOG(ERR) << "ERROR t" << tickNum
                            << ": Point will move to a negative position: "
                            << *point << std::endl;
                }


                point->x = point->x + point->vx;
                point->y = point->y + point->vy;
                break;
            }
        }

    }

    void createMap(std::vector<Point2D*>* points, int*& pointMap) {
        if(pointMap == NULL) {
            pointMap = new int[binsX * binsY];

            for(int i = 0; i < binsX * binsY; i++) {
                pointMap[i] = 0;
            }
        }
        
        double cellSizeX = (double) spaceX / (double) binsX;
        double cellSizeY = (double) spaceY / (double) binsY;

        // std::cout << "cellSizeX = " << cellSizeX << " " 
        //           << "spaceX = " << spaceX << " " 
        //           << "width = " << width << " " 
        //           << "cellSizeY = " << cellSizeY << " " 
        //           << "spaceY = " << spaceY << " " 
        //           << "height = " << height << std::endl;

        std::vector<Point2D*>::iterator it;
        Point2D* p;
        for(it = points->begin(); it != points->end(); it++) {
            p = *it;
            int xpos = (int) (p->x / cellSizeX);
            int ypos = (int) (p->y / cellSizeY);

            pointMap[ypos*binsX + xpos]++;
        }
        
    }

//     void computeStats(int* pointMap) {

//         double avg = (double) points->size() / (double)(width * height);
//         double stddev = 0;
//         int min = std::numeric_limits<int>::max();
//         int max = std::numeric_limits<int>::min();


//         for(int i = 0; i < width * height; i++) {
// //            std::cout << "here " <<  pointMap[i] << std::endl;

//             if(pointMap[i] > max) {
//                 max = pointMap[i];
//             }
//             if(pointMap[i] < min) {
//                 min = pointMap[i];
//             }

//             stddev += (pointMap[i] - avg) * (pointMap[i] - avg);
//         }
//         stddev = sqrt(stddev / (width * height));


//         std::cout << "avg: " << avg << std::endl;
//         std::cout << "max: " << max << std::endl;
//         std::cout << "min: " << min << std::endl;
//         std::cout << "stddev: " << stddev << std::endl;

//     }


    void printMap(int*& pointMap) {
        for(int i = 0; i < binsX*binsY; i++) {
            if(i % binsX == 0 && i != 0) {
                std::cout << std::endl;
            }
            std::cout << std::setw(5) << pointMap[i] << " ";
        }
        std::cout << std::endl;
    }
    
    void plotMap(/*FILE* outfile, */int*& pointMap) {
        

        // char* tmpfile = tmpnam(NULL);

        // printf("outfile name: %s\n", tmpfile);

        // FILE* fp = fopen(tmpfile, "w");
        

        // // TODO: Convert this to pipe the data directly to gnuplot. 
        // for(int y = 0; y < binsY; y++) {
        //     for(int x = 0; x < binsX; x++) {
        //         fprintf(fp, "%d %d %d\n", x, y, pointMap[y*binsX + x]);
        //     }
        //     fprintf(fp, "\n");
        // }
        // fflush(fp);
        // fclose(fp);

        FILE* gp = popen("gnuplot", "w");
//        FILE* gp = stdout;

        fprintf(gp, "set terminal postscript eps enhanced color;");
        fprintf(gp, "set output '%s';", plotName);
        fprintf(gp, "unset key;");
        fprintf(gp, "set view map;");
        fprintf(gp, "set style data linespoints;");
        fprintf(gp, "set title 'Point Heat Map';");
        fprintf(gp, "set xrange [ -0.500000 : %.2f ] noreverse nowriteback;", (double) binsX);
        fprintf(gp, "set yrange [ -0.500000 : %.2f ] noreverse nowriteback;", (double) binsY);
        fprintf(gp, "set cblabel 'Score';");
        fprintf(gp, "set palette rgbformulae -7, 2, -7;");
        fprintf(gp, "plot '-' using 2:1:3 with image\n");

        for(int y = 0; y < binsY; y++) {
            for(int x = 0; x < binsX; x++) {
                fprintf(gp, "%d %d %d\n", x, y, pointMap[y*binsX + x]);
            }
        }
        fprintf(gp, "e\n");
            
        pclose(gp);


        // if(remove(tmpfile) != 0) {
        //     perror("Error deleting file");
        // }

    }

    void plotPoints(std::vector<Point2D*>* points, 
                    std::vector<Point2D*>* qp,
                    int tick) {

        // char tmpName1 [L_tmpnam];
        // tmpnam(tmpName1);
        // char* tmpName2 = tmpnam(NULL);

        // printf("outfile 1 name: %s\n", tmpName1);
        // printf("outfile 2 name: %s\n", tmpName2);

        // FILE* pointFile = fopen(tmpName1, "w");
        // FILE* queryFile = fopen(tmpName2, "w");

        // for(int i = 0; i < points->size(); i++) {
        //     fprintf(pointFile, "%lf %lf\n", points->at(i)->x, points->at(i)->y);
        // }

        // for(int i = 0; i < qp->size(); i++) {
        //     fprintf(queryFile, "%lf %lf\n", qp->at(i)->x, qp->at(i)->y);
        // }

        // fflush(pointFile);
        // fflush(queryFile);
        
        // fclose(pointFile);
        // fclose(queryFile);

        FILE* gp = popen("gnuplot", "w");
        fprintf(gp, "set terminal postscript eps enhanced color;");
        fprintf(gp, "set output '%s';", plotName);
        fprintf(gp, "set title 'Point Distribution Tick %d';", tick);
        fprintf(gp, "unset key;");        
        fprintf(gp, "plot '-' using 1:2, '-' using 1:2\n");

        for(int i = 0; i < points->size(); i++) {
            fprintf(gp, "%lf %lf\n", points->at(i)->x, points->at(i)->y);
        }
        fprintf(gp, "e\n");

        for(int i = 0; i < qp->size(); i++) {
            fprintf(gp, "%lf %lf\n", qp->at(i)->x, qp->at(i)->y);
        }

        fprintf(gp, "e\n");


        pclose(gp);

        // if(remove(tmpName1) != 0) {
        //     perror("Error deleting file");
        // }
        // if(remove(tmpName2) != 0) {
        //     perror("Error deleting file");
        // }
    }


  void playTrace(/*FILE* outfile,*/ Workload2D* workload, int targetTick, bool heatMap) {

        int tickNum = 0;
        int* map = NULL;
        int numPosUpdates = 0;
        int numVelUpdates = 0;
        double maxVx = 0;
        double maxVy = 0;

        while(workload->hasNextTick()) {

            maxVx = maxVy = 0;

            workload->startTick();
            std::vector<Point2D*>* points = workload->getPoints();            
            //std::cout << "Total num points: " << points->size() << std::endl;


            //std::cout << tickNum << ": " << *points->at(3) << std::endl;

            for(int i = 0; i < points->size(); i++) {
                if(points->at(i)->vx > maxVx) {
                    maxVx = points->at(i)->vx;
                }
                if(points->at(i)->vy > maxVy) {
                    maxVy = points->at(i)->vy;
                }

                // if(points->at(i)->x < 0 || points->at(i)->y < 0) {
                //     std::cout << "ERROR: Point is Negative " << *points->at(i) << std::endl;
                // }
            }

            


            //Plot heat map for a single tick. 
            if(tickNum == targetTick) {
                std::vector<Point2D*>* points = workload->getPoints();

                if(heatMap) {
                    createMap(points, map);
                    break;
                }
                else {
                    std::vector<Point2D*>* qp;

                    if(workload->hasNextQuery()) {
                        QueryRec* query = workload->nextQuery();
                        qp = (query->queryPoints == 0) ? points : query->queryPoints;
                    }

                    plotPoints(points,qp,targetTick);
                    return;
                }
            }
            //Plot heat map for entire simulation run. 
            else if(targetTick == -1) {
                std::vector<Point2D*>* points = workload->getPoints();
                createMap(points, map);
            }


            std::vector<Point2D*>* qp;

            int numQueries = 0;
            int numPoints = 0;

            while(workload->hasNextQuery()) {
                QueryRec* query = workload->nextQuery();
                qp = (query->queryPoints == 0) ? points : query->queryPoints;
                numQueries++;
                numPoints += qp->size();
            }

            std::cout << "numQueries: " << numQueries << std::endl;
            std::cout << "Avg. numQueriers: " << (numPoints / (double) numQueries) << std::endl;

            //No need to process queries. 

            numPosUpdates = 0;
            numVelUpdates = 0;            

            applyUpdates(workload, tickNum, numPosUpdates, numVelUpdates);

            //std::cout << "numPosUpdates: " << numPosUpdates << " numVelUpdates: " << numVelUpdates << std::endl;

            std::cout << "maxVx: " << maxVx << " maxVy: " << maxVy << std::endl;

            workload->endTick();

            tickNum++;

        }



        // std::cout << "tickNum: " << tickNum << std::endl;
        // std::cout << "numPosUpdates: " << ((double)numPosUpdates/(double)tickNum) << std::endl;
        // std::cout << "numVelUpdates: " << ((double)numVelUpdates/(double)tickNum) << std::endl;

        
        //plotMap(/*outfile, */map);
//        printMap(map);
    }

    // Taken from Al's code. Should use DBUtl/CmdLine, but the logging is
    // wrong...
    char const * suffixAfterMatch(const char *s, const char *flagArg) {
        size_t n = strlen(flagArg);
        char const * ans = ((strncmp(s, flagArg, n) == 0) ? (s+n) : 0);
        return ans;
    }

    //enum CheckTraceMode { TICK, TRACE_AGG }
}



int main(int argc, char *argv[]) {
//    FILE* outfile;
    Workload2D* workload;
    int tick = -1;
    binsX = -1;
    binsY = -1;
    spaceX = -1;
    spaceY = -1;


    for(int argNum = 1; argNum < argc; argNum++) {
        char const * arg = argv[argNum];
        char const * v;
        
        // if((v = suffixAfterMatch(arg, "--outfile=")) != 0) {
        //     outfile = fopen(v, "wb");
        // }
//        else 
        if((v = suffixAfterMatch(arg, "--tracefile=")) != 0) {
            bool binary;
            if(strstr(v, ".dat") != 0) {
                binary = true;
            }
            else if(strstr(v,".txt") != 0) {
                binary = false;
            }
            workload = new TraceWorkload2D(v, binary);
        }

        else if((v = suffixAfterMatch(arg, "--binsX=")) != 0) {
            binsX = strtod(v,0);
        }
        else if((v = suffixAfterMatch(arg, "--binsY=")) != 0) {
            binsY = strtod(v,0);
        }

        else if((v = suffixAfterMatch(arg, "--spaceX=")) != 0) {
            spaceX = strtod(v,0);
        }
        else if((v = suffixAfterMatch(arg, "--spaceY=")) != 0) {
            spaceY = strtod(v,0);
        }
        else if((v = suffixAfterMatch(arg, "--tick=")) != 0) {
            tick = strtod(v,0);
        }
    }

    if(workload == NULL) {
        throwUsage("Unknown Workload");
    }

    // Aggregate over entire trace, so the number of bins must be set.   
    if(tick == -1) {
        if(binsX == -1 || binsY == -1 || spaceX == -1 || spaceY == -1) {
            throwUsage("binsX, binsY, spaceX, spaceY must be set when tick = -1.");
        }

        sprintf(plotName, "%s%s_heat_map_agg.eps", 
                plotdir, 
                workload->getName());

    }
    // Heatmap of a single tick. 
    else if(tick != -1 && (binsX != -1 || binsY != -1)) {
        if(spaceX == -1 || spaceY == -1 || binsX == -1 || binsY == -1) {
            throwUsage("binsX, binsY, spaceX, spaceY when plotting a heatmap.");
        }
        sprintf(plotName, "%s%s_heat_map_%d.eps", 
                plotdir, 
                workload->getName(), 
                tick);
    }
    // Scatter plot of single tick. Don't need bins or space. 
    else {
        sprintf(plotName, "%s%s_scatter_%d.eps", 
                plotdir, 
                workload->getName(), 
                tick);
    }

     fprintf(stdout, "plotName: %s\n", plotName);
    

    playTrace(/*outfile, */workload, tick, (binsX != -1));
//    fclose(outfile);
    
    
    return 0;
}
