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

#ifndef _TRACE_WRITER_H_
#define _TRACE_WRITER_H_

#include <vector>
#include "2D/TestTypes2D.h"


namespace PSimIndex {

    /**
     *
     *
     */
    struct SerializableQuery {
        int type;
        double xDelta, yDelta;
        int k;

        SerializableQuery(int type, double xDelta, double yDelta, int k)
            : type(type), xDelta(xDelta), yDelta(yDelta), k(k) 
        {}

        SerializableQuery() {}
    };


    /**
     *
     *
     */
    struct SerializableUpdate {
        int type, id;
        double x, y, vx, vy, data;

        SerializableUpdate(int type, int id, double x, double y, 
                           double vx, double vy, double data)
            : type(type), id(id), x(x), y(y), vx(vx), vy(vy), data(data)
        {}

        SerializableUpdate() {}
    };

        
    /**
     * Usage:
     *  1. Construct a new instance of TraceWriter. 
     *  2. Call addPoint for each initial point in the set.  
     *  3. Call writeHeader to write out the header information. 
     *  
     *
     * @author Ben Sowell
     */
    class TraceWriter2D {
    private:
        static const int DEFAULT_POINTS = 10000;
        //static const int DEFAULT_QUERIES;        
        static const int DEFAULT_UPDATES = 10000;        

        FILE* file;
        bool isBinary;

        //std::vector<Point2D*> points;

        // Because the queries are interleaved with the query points, we don't
        // get much benefit from using an aligned array.
        std::vector<std::vector<int>*> queriers;
        std::vector<SerializableQuery*> queries;

        int numPoints, pointsSize;
        int currQuery;
        int numUpdates, updatesSize;
        double probUpdate;

        Point2D* points;
        SerializableUpdate* updates;

    public:

        /**
         *
         * @param  binaryFile
         * @param  textFile
         */
        static void binaryToText(const char* binaryFile, const char* textFile);

        /**
         *
         * @param  textFile
         * @param  binaryFile
         */
        static void textToBinary(const char* textFile, const char* binaryFile);

        /**
         *
         * @param  id
         * @param  x
         * @param  y
         * @param  vx
         * @param  vy
         * @param  data
         */
        void addPoint(int id, double x, double y, 
                      double vx, double vy, double data);

        /**
         * @param  numTicks
         */
        void writeHeader(int numTicks);

        /**
         *
         */

        void startQueries();

        /**
         * @param  id
         */
        void addQuerier(int id);

        /**
         *
         * @param  type
         * @param  xDelta
         * @param  yDelta
         * @param  k
         */
        void writeQuery(int type, double xDelta, double yDelta, int k=1);

        /**
         *
         */
        void endQueries();

        /**
         *
         *
         */
        void startUpdates();

        /**
         *
         * @param  type
         * @param  id
         * @param  x
         * @param  y
         * @param  vx
         * @param  vy
         * @param  data
         */
        void writeUpdate(int type, int id, double x, double y, 
                         double vx, double vy, double data);

        /**
         *
         */
        void endUpdates();

        /**
         *
         */
        void endTick();


        /**
         * @param  filename
         * @param  binary
         */
        TraceWriter2D(const char* filename, bool binary);
        ~TraceWriter2D();

    };




}





#endif /* _TRACE_WRITER_H_ */
