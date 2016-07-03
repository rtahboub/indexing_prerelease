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

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdio.h>
#include <cmath>
#include <climits>
#include "util/RNG.h"
#include <iostream>

/**
 * Original code for reading ascii version of Brinkhoff network data from  
 *  http://www.comp.nus.edu.sg/~spade/benchmark/
 * Modified 5-2-10 by Ben Sowell
 */

#define MAX_DEGREE 100

namespace PSimIndex {

    class Edge;

    class Node {
    public:
        long nid;
        float x;
        float y;

        Edge ** edges;
        int degree;

        Node() {
            edges = new Edge*[MAX_DEGREE];
            degree = 0;
        }

        void add_edge(Edge * edge) {
            if(degree>=MAX_DEGREE)
                printf("max_degree is too small\n");

            edges[degree++] = edge;
        }
    };
 

    std::ostream& operator<<(std::ostream &out, const Node& s);	

    class Edge {
    public:
        long eid;
        Node * s;
        Node * e;
        float length;

        float get_edge() {
            return atan2(e->y - s->y, e->x - s->x);
        }

        void compute_length() {
            length = sqrt( pow(s->y-e->y, 2) + pow(s->x-e->x, 2));
        }
    };

    std::ostream& operator<<(std::ostream &out, const Edge& s);	

    class Network {
    public:
        int num_node;
        Node * nodes;

        int num_edge;
        Edge * edges;

        RNG* rng;

        float space;
        float total_length;

        Network(const char * nodefile, 
                const char * edgefile, 
                float _space, 
                RNG* rng) {
            space = _space;
            total_length = 0;

            std::cout << "reading: " << nodefile << std::endl;
            std::cout << "reading: " << edgefile << std::endl;
            read_node(nodefile);
            read_edge(edgefile);
            this->rng = rng;
        }

        ~Network()
        {
            write_node();
            write_edge();
            delete [] nodes;
            delete [] edges;
        }

        // Edge * get_random_edge(float rand) {
        //     float dist = rand * total_length;
        //     int i=0;
        //     float sumlength = edges[0].length;
        //     while(sumlength<dist)
        //         sumlength += edges[i++].length;
        //     return &edges[i];
        // }


        

        Edge* getRandomEdge() {
            return &edges[rng->select(num_edge)];
        }

        void read_node(const char * nodefile) {

            std::cout << "Started reading nodes." << std::endl;

            int minx, miny, maxx, maxy;
            minx = miny = INT_MAX;
            maxx = maxy  = 0;

            FILE * fp = fopen(nodefile, "r");
            fscanf(fp, "%d\n", &num_node);
            nodes = new Node[num_node];
            int x, y;
            for(int i=0;i<num_node;i++)
            {
                fscanf(fp, "%ld %d %d\n", &nodes[i].nid, &x, &y);
                //printf("%ld %d %d\n", nodes[i].nid, x, y);
                nodes[i].x = x;
                nodes[i].y = y;
                if(nodes[i].x < minx)	minx = nodes[i].x;
                if(nodes[i].y < miny)	miny = nodes[i].y;
                if(nodes[i].x > maxx)	maxx = nodes[i].x;
                if(nodes[i].y > maxy)	maxy = nodes[i].y;
            }
            fclose(fp);

            int thisspace = maxx-minx;
            if((maxy-miny)>thisspace)
                thisspace = maxy-miny;
            float zoom = space / thisspace;

            for(int i=0;i<num_node;i++)
            {
                nodes[i].x = (nodes[i].x - minx) * zoom;
                nodes[i].y = (nodes[i].y - miny) * zoom;
                
                //std::cout << nodes[i].x << " " << nodes[i].y << std::endl;

            }
            std::cout << "Finished reading nodes." << std::endl;
        }

        void read_edge(const char * edgefile) {
            std::cout << "Started reading edges." << std::endl;

            FILE * fp = fopen(edgefile, "r");
            fscanf(fp, "%d\n", &num_edge);

            std::cout << "num_edge" << num_edge << std::endl;

            edges = new Edge[num_edge];
            long snid, enid;
            for(int i=0;i<num_edge;i++)
            {


                fscanf(fp, "%ld %ld %ld\n", &edges[i].eid, &snid, &enid);

                if(i%1000 == 0) {
                    printf("i: %d\n", i);
                }

                edges[i].s = get_node(snid);
                edges[i].s->add_edge(&edges[i]);
                edges[i].e = get_node(enid);
                edges[i].e->add_edge(&edges[i]);
                edges[i].compute_length();
                total_length += edges[i].length;
            }
            fclose(fp);

            std::cout << "Finished reading edges." << std::endl;
        }


        void write_node() {
            FILE * fp = fopen("node", "w");
            for(int i=0;i<num_node;i++)
                fprintf(fp, "%ld %f %f\n", nodes[i].nid, nodes[i].x, nodes[i].y);
            fclose(fp);
        }

        void write_edge() {
            FILE * fp = fopen("edge", "w");
            for(int i=0;i<num_edge;i++)
                fprintf(fp, "%ld %ld %ld\n", edges[i].eid, edges[i].s->nid, edges[i].e->nid);
            fclose(fp);
        }

        Node * get_node(long nid) {
            for(int i=0;i<num_node;i++)
                if(nodes[i].nid == nid)
                    return &nodes[i];
            return NULL;
        }
    };
}



#endif /* _NETWORK_H_ */
