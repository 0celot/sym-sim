
/*
 * Copyright (c) 2012 Bradley Mitchell
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */

/* 
 * File:   simtypes.h
 * Author: bradley
 *
 * Created on September 15, 2007, 9:37 PM
 */

#ifndef _SIMTYPES_H
#define	_SIMTYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
//#include <SDL/SDL.h>
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>
    
#define INITIAL_FUEL            1.0
#define FUEL_CONSUMPTION        0.001
#define MIN_LAUNCH              2
#define MAX_LAUNCH              10
#define MIN_FUEL                8
#define MAX_FUEL                16
#define NUM_TARGETS             100
#define NUM_UAVS                20
#define OPTIMIZING              1   // false
#define SPEED                   0.2
#define TARGET_XPOS             50  // center
#define TARGET_YPOS             50
#define TURN_ANGLE              0.1 // 0.1 radians
#define INTERVAL_LENGTH         50    // 50 ticks
#define PHI_ONE                 1.0
#define PHI_TWO                 1.0
#define MAP_HEIGHT              100.0
#define MAP_WIDTH               100.0
#define MAX_BASE_DISTANCE       50.0
#define MIN_BASE_DISTANCE       10.0
#define MAX_BASE_VELOCITY       10
#define MAX_COOPERATE           1.0
#define MIN_COOPERATE           0.0
#define MAX_COOPERATE_VELOCITY  0.25
#define WEAPON_RANGE            5.0
#define AVG_WEAPON_EFFECT       0.05
#define STDEV_WEAPON_EFFECT     0.03
#define DETECTION_RANGE         4.0
#define SAMPLING_INTERVAL       50
#define CRAZINESS               0.02
#define MAX_TICKS               5000
#define UAV_STATE_ACTIVE        0
#define UAV_STATE_INACTIVE      1
    
// Numerical Recipes 3rd edition style random
struct nr_random{
    unsigned long long int u;
    unsigned long long int v;
    unsigned long long int w;
};
typedef struct nr_random my_random;

// A singly linked list
struct list_node{
	void * data;
	struct list_node *next;
};
typedef struct list_node node;

struct model_spec{
    double targetMeanDistance;
    double targetStandardDeviation;
    double targetMaxVisibility;
    double toughness;
    double signalStrength;
    my_random * rand;
    int runs;
    int runLength;
    char outFile[100];
    char outDetailedFile[100];
};
typedef struct model_spec model;

struct binary_chrom{
    char genes[10];
    double fitness;
};
typedef struct binary_chrom individual;

struct model_stats{
    int totalAttacks;
    int totalKills;
    double totalFuelConsumed;
    int missionComplete;        // Simulation tick of last target eliminated
};
typedef struct model_stats stats;

struct detailed_model_stats{
    double avgBaseDistance[100];
    double avgCooperate[100];
    double avgFitness[100];
    double avgNeighborhood[100];
    int remainingTargets[100];
	individual chromosomes[100];
    int updates;
};
typedef struct detailed_model_stats detailed_stats;

struct uav_agent{
    double mass;
    double position[2];
    double velocity[2];
    double max_force; 
    double max_speed;
    double orientation;     // A direction in radians
    //double heading[2];      // TODO: remove and replace with orientation
    double basePos[2];
    int attacks;
    int kills;
    int detections;
    double fuel_consumed;
    //double turns;
    int ticks;
    double fuel;
    node * group;
    node * targets;
    double pPos[3];
    double pVel[2];
    double pBest[3];
    double lBest[3];
    int leader;
    //int cooperating;
    double cooperateVal;
    //double baseDistanceVal;
    double constriction;
    my_random * rand;
    int state;
};
typedef struct uav_agent uav;

struct target_agent{
    double position[2];
    double toughness;
    double damage;
    node * attackers;
    int numAttacks;
};
typedef struct target_agent target;

struct base_agent{
    double position[2];
    node * fuel;
    node * launch;
    int nextFuel;
    int nextLaunch;
    my_random * rand;
};
typedef struct base_agent base;

// list.c protos
void addFront(node **n, void * data);
void * removeFront(node **n);
void addBack(node **n, void * data);
int length( const node *n);
void copy( node **cpy, node * list );
void clear( node *list );
int hasData( const node * list, const void * d );
void mergeUnique( node ** list1, const node * list2 );

// particle.c protos
void updateParticleLocalBest( uav * plane );
void updateParticlePersonalBest( uav * plane );
void updateParticlePosition( uav * plane );
void updateParticleVelocity( uav * plane );
void evaluateParticle( uav * plane );

// simrandom.c protos
void seedMyRandom( my_random * rand, unsigned long long seed);
unsigned long long int64( my_random * rand );
double doub( my_random * rand );
unsigned int int32( my_random * rand );
//double Lognormal( double mu, double sigma, gsl_rng * rnd );
double normal( double mu, double sigma, my_random * rand );
double uniform( double lower, double upper, my_random * rand );
int uniformInt( int lower, int upper, my_random * rand );

// uav.c protos
int detectGroup( uav * plane, const uav group[], int sz, const model * mod,
        const base * bs);
int detectTargets( uav * plane, const target tgts[], int sz, 
        const model * mod );
void addTargets( uav * plane, node * tgts );
int hasTarget( const uav * plane, const target * tgt );
void clearTargets( uav * plane );
void clearGroup( uav * plane );
void finalizeUAV( uav * plane );
double moveUAV( uav * plane, base * bs, const model * mod );
void rebaseUAV( uav * plane, const model * mod );
int attackNearest( uav *plane );
target * nearestTarget( const uav * plane );
double averageGroupDistance( const double next[], const uav *plane );
double averageTargetDistance( const double next[], const uav *plane );
void randomHeading( uav * plane );

// sim.c protos
void initialize( const model * mod, uav planes[], target targs[], 
        base * bs );
void run( const model * mod , stats * st, detailed_stats * dst, uav planes[], target targs[], base * bs, int start );
void step( const model * mod, stats * st, detailed_stats * dst, uav planes[], target targs[], base * bs );
int hasUAV( const base * bs, const uav * plane );
int resolveAttacks( target * targ );
void clearAttackers( target * targ );
void updateDetailedStats( detailed_stats * dst, const stats * st, int idx, uav planes[] );
int gray( int n );
int invGray( int n );
void configureUAVS( uav planes[], const individual *chromosome );

// display.c protos
//void display( target targs[], int tSize, uav planes[], int pSize );
//void initDisplay( int width, int height );
//void init_opengl(int width, int height);
//SDL_Surface* init_sdl(int width, int height);

// base.c protos
void baseRefuelProcess( base * bs, const model * mod );
void baseRelaunchProcess( base * bs, const model * mod );

// physics.c protos
double distance( const double p1[], const double p2[] );
void normalize( double vector[] );
void truncate( double steering_direction[], double max_force );
double nextPosition( uav * plane , double steering_direction[] );
void rotate( uav * plane );
void rotateVector( double vec[] , double theta );
double lengthVector( double vec[] );
double angleTwoVectors( double vecA[], double vecB[] );
double dotProduct2D( double vecA[], double vecB[] );
void seek( uav * plane, double seekPos[], double steering[] );
void arrival( uav * plane, double seekPos[], double steering[] );
void orbit( uav * plane, double orbitPos[], double radius, double steering[] );
void offsetSeek( uav * plane, double seekPos[] , double offset, double steering[] );
int separation( uav * plane, double steering[] );
int cohesion( uav * plane, double steering[] );
int alignment( uav * plane, double steering[] );
int flocking( uav * plane, double steering[] );
int followTheLeader( uav * plane, double steering[] );

// genetic.c protos
void crossover1( const individual *parentA, const individual *parentB, individual *childA, individual *childB, my_random * rand );
void crossover2( const individual *parentA, const individual *parentB, individual *childA, individual *childB, my_random * rand );
void mutation( individual *chr, double rate, my_random * rand );
void proportionalSelection( individual pop[], int n, individual * parentA, individual * parentB , my_random * rand );
void tournamentSelection( individual pop[], int n, int k, individual * parentA, individual * parentB , my_random * rand );
void competeSurvival( individual pop[], individual children[], int n );
void noncompeteSurvival( individual pop[], individual children[], int n );

// report.c protos
void writeResults( stats results, model * mod );
void writeDetailedResults( detailed_stats results, model * mod, int generations );
void printPopulation( individual pop[], int n );

#ifdef	__cplusplus
}
#endif

#endif	/* _SIMTYPES_H */

