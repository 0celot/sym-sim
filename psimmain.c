
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
 * File:   simmain.c
 * Author: bradley
 *
 * Created on September 15, 2007, 9:55 PM
 */


#include "simtypes.h"


void setupModel( model * mod, my_random * rand ){

	mod->signalStrength = 6;
    mod->targetMaxVisibility = 5;
    mod->targetMeanDistance = 30;
    mod->targetStandardDeviation = 4.6666;
    mod->toughness = 1.0;
    mod->runs = 30;
    mod->runLength = 5000;
	mod->rand = rand;
}

void setupRandom( my_random * rand, int pID ){
	
	time_t seed;	// random number seed

	// Create a seed for this process
	seed = time( NULL ) * ( ( pID + 1) * 11);

    // Initialize the random number generator
    seedMyRandom( rand, seed );
}

void setupMPI( int *arg_c, char*** arg_v, int *pID, int *numProc ){

	// Setup MPI
	MPI_Init( arg_c, arg_v );	
	
	// Get the current process ID
	MPI_Comm_rank( MPI_COMM_WORLD, pID );	
	
	// Get the total number of processes in use
	MPI_Comm_size( MPI_COMM_WORLD, numProc );
}

void setupStats( stats * st, detailed_stats * dst ){
	
	st->missionComplete = 0;
	st->totalAttacks = 0;
    st->totalFuelConsumed = 0;
    st->totalKills = 0;

	int i;
    for( i = 0; i < 50; ++i ){
        dst->avgBaseDistance[i] = 0;
        dst->avgCooperate[i] = 0;
        dst->avgFitness[i] = 0;
        dst->avgNeighborhood[i] = 0;
        dst->remainingTargets[i] = 0;
    }
        
    dst->updates = 0;
}

void setupPopulation( individual pop[], int pSize, my_random * rd ){

	int i, j;
    for( i = 0; i < pSize; ++i ){
        for( j = 0; j < 10; ++j)
            pop[i].genes[j] = 0;
        
        mutation( &pop[i], 0.5, rd );      
        pop[i].fitness = 0;
    }
}

void setUncontrolled( double uncontrolled[], int sz, double setting, double radius, my_random * rd  ){

	double min, max;

	min = setting - radius;

	if( min < 0 )
		min = 0;

	max = setting + radius;
	
	int i;
	for( i = 0; i < sz; ++i ){
		
		uncontrolled[i] = uniform( min, max, rd );
	}
}

void sendCME( double com_range[], double max_visi[], double tough[], individual ind, 
		      uav planes[], target targs[], base * bs, int pID, int remainingTicks ){

	int tagNum = pID * 10000;
	// This is a CME
	int cme = 1;
	MPI_Send( &cme, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
	tagNum++;
	
	// Send each model one at a time
	int i;
	for( i = 0; i < 30; ++i ){
		
		// Send uncontrollable factors
		MPI_Send( &com_range[i], 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &max_visi[i], 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &tough[i], 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;

		// Send individual
		MPI_Send( ind.genes, 10, MPI_CHAR, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &ind.fitness, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
	}

	// Send uavs
	for( i = 0; i < NUM_UAVS; ++i ){
		MPI_Send( planes[i].position, 2, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( planes[i].velocity, 2, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( planes[i].basePos, 2, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].orientation, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( &planes[i].mass, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].max_speed, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].max_force, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( &planes[i].attacks, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].kills, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].detections, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].fuel_consumed, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].ticks, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &planes[i].fuel, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;

		MPI_Send( planes[i].pPos, 3, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( planes[i].pVel, 2, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( planes[i].pBest, 3, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( planes[i].lBest, 3, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( &planes[i].constriction, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        MPI_Send( &planes[i].state, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
        
	}

	// Send targets
	for( i = 0; i < NUM_TARGETS; ++i ){
		MPI_Send( targs[i].position, 2, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
		MPI_Send( &targs[i].toughness, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
    	MPI_Send( &targs[i].damage, 1, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
    	MPI_Send( &targs[i].numAttacks, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
		tagNum++;
	}

	// Send base
	MPI_Send( bs->position, 2, MPI_DOUBLE, pID, tagNum, MPI_COMM_WORLD );
	tagNum++;
	MPI_Send( &bs->nextFuel, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
	tagNum++;
	MPI_Send( &bs->nextLaunch, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
	tagNum++;

	// Send remaining ticks
    MPI_Send( &remainingTicks, 1, MPI_INT, pID, tagNum, MPI_COMM_WORLD );
}

// Returns 1 if a CME was received and 0 if not
int receiveCME( model mod[], individual * ind, uav planes[], 
				target targs[], base * bs, int pID ){

	MPI_Status stat;

	int tagNum = pID * 10000;


	// Is this a CME?
	int cme;
	//MPI_Send( &cme, 1, MPI_INT, pID, 0, MPI_COMM_WORLD );
	MPI_Recv (&cme, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
	tagNum++;
	if( cme == 0 )
		return 0; // return if not
	
	//printf( "Model %i entering receiveCME.\n", pID );

	// Receive each model one at a time
	int i;
	for( i = 0; i < 30; ++i ){
		
		// Receive uncontrollable factors
		MPI_Recv (&mod[i].signalStrength, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv (&mod[i].targetMaxVisibility, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv (&mod[i].toughness, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;

		//printf("Received signal strength is: %f\n", mod[i].signalStrength);

		// Receive individual
		MPI_Recv ( ind->genes, 10, MPI_CHAR, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &ind->fitness, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
	}

	// Receive uavs
	for( i = 0; i < NUM_UAVS; ++i ){
		MPI_Recv ( planes[i].position, 2, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( planes[i].velocity, 2, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( planes[i].basePos, 2, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].orientation, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].mass, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].max_speed, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].max_force, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].attacks, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].kills, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].detections, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
  		MPI_Recv ( &planes[i].fuel_consumed, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].ticks, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].fuel, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( planes[i].pPos, 3, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( planes[i].pVel, 2, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( planes[i].pBest, 3, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( planes[i].lBest, 3, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].constriction, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &planes[i].state, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat); 
		tagNum++;
	}

	// Receive targets
	for( i = 0; i < NUM_TARGETS; ++i ){
		MPI_Recv ( targs[i].position, 2, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &targs[i].toughness, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &targs[i].damage, 1, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
		tagNum++;
		MPI_Recv ( &targs[i].numAttacks, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat); 
		tagNum++;
	}

	// Receive base
	MPI_Recv ( bs->position, 2, MPI_DOUBLE, 0, tagNum, MPI_COMM_WORLD, &stat);
	tagNum++;
	MPI_Recv ( &bs->nextFuel, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
	tagNum++;
	MPI_Recv ( &bs->nextLaunch, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);
	tagNum++;

	// Receive remaining ticks
	int remainingTicks;
	MPI_Recv ( &remainingTicks, 1, MPI_INT, 0, tagNum, MPI_COMM_WORLD, &stat);

	for( i = 0; i < 30; ++i )
		mod[i].runLength = remainingTicks;
	
	return 1;
}

void receiveFitness( individual population[], int pop_size ){

	MPI_Status stat;
	double fit;
	int i;
	for( i = 0; i < pop_size; ++i ){
		MPI_Recv( &fit, 1, MPI_DOUBLE, ( i + 1 ), 0, MPI_COMM_WORLD, &stat);
		population[i].fitness = fit;
	}
}

// Returns the index of the most fit individual
int bestIndividual( individual population[], int pop_size ){

	int i;
	int bestIdx = 0;
	int bestFit = population[0].fitness;
	for( i = 1; i < pop_size; ++i ){
		if( population[i].fitness > bestFit ){
			bestIdx = i;
			bestFit = population[i].fitness;
		}		
	}

	return bestIdx;
}

void copyModel( const model * mod, model * modcpy, const uav planes[], uav planescpy[], const target targs[], target targscpy[], 
        const base * bs, base * bscpy  ){
    
	// Copy model
	
	modcpy->targetMeanDistance = mod->targetMeanDistance;
    modcpy->targetStandardDeviation = mod->targetStandardDeviation;
    modcpy->targetMaxVisibility = mod->targetMaxVisibility;
    modcpy->toughness = mod->toughness;
    modcpy->signalStrength = mod->signalStrength;
	modcpy->runs = mod->runs;
	modcpy->runLength = mod->runLength;

    
    // Copy the base
    bscpy->position[0] =  bs->position[0];
    bscpy->position[1] = bs->position[1];
    bscpy->fuel = 0;
    bscpy->launch = 0;
    bscpy->rand = modcpy->rand;
 
    bscpy->nextFuel = bs->nextFuel;
    bscpy->nextLaunch = bs->nextLaunch;
    
    
    // Copy all UAVs
    int i;
    for( i = 0; i < NUM_UAVS; ++i ){
        
        planescpy[i].position[0] = planes[i].position[0];
        planescpy[i].position[1] = planes[i].position[1];
        
        planescpy[i].velocity[0] = planes[i].velocity[0];
        planescpy[i].velocity[1] = planes[i].velocity[1];
        
        
        planescpy[i].basePos[0] = planes[i].basePos[0];
        planescpy[i].basePos[1] = planes[i].basePos[1];
        
        planescpy[i].orientation = planes[i].orientation;
        rotate( &planescpy[i] );
        
		planescpy[i].mass = planes[i].mass;
        planescpy[i].max_speed = planes[i].max_speed;
        planescpy[i].max_force = planes[i].max_force;
        
        planescpy[i].attacks = planes[i].attacks;
        planescpy[i].kills = planes[i].kills;
        planescpy[i].detections = planes[i].detections;
        planescpy[i].fuel_consumed = planes[i].fuel_consumed;
        planescpy[i].ticks = planes[i].ticks;
        planescpy[i].fuel = planes[i].fuel;
        planescpy[i].group = 0;
        planescpy[i].targets = 0;

		// Copy random number gen from mod copy
        planescpy[i].rand = modcpy->rand;
     
        
        // Copy the UAV's particle
        planescpy[i].pPos[0] = planes[i].pPos[0];
        planescpy[i].pPos[1] = planes[i].pPos[1];
        planescpy[i].pPos[2] = planes[i].pPos[2];
        
        planescpy[i].pVel[0] = planes[i].pVel[0];
        planescpy[i].pVel[1] = planes[i].pVel[1];

        planescpy[i].pBest[0] = planes[i].pBest[0];
        planescpy[i].pBest[1] = planes[i].pBest[1];
        planescpy[i].pBest[2] = planes[i].pBest[2];
        
        planescpy[i].lBest[0] = planes[i].lBest[0];
        planescpy[i].lBest[1] = planes[i].lBest[1];
        planescpy[i].lBest[2] = planes[i].lBest[2];
        
        planescpy[i].constriction = planes[i].constriction;
        
        planescpy[i].state = planes[i].state;

		// TODO: Modify this code so that the original UAV sequences in the launch and fuel queues is preserved
		if( planescpy[i].state == UAV_STATE_INACTIVE ){
		
			if( planescpy[i].fuel <= 0 )
				addBack( &( bscpy->fuel ), ( void * ) &( planescpy[i] ));
			else
				addBack( &( bscpy->launch ), ( void * ) &( planescpy[i] ));
		
		}
        
    }
    
    // Copy all targets
    for( i = 0; i < NUM_TARGETS; ++i ){  
                
        targscpy[i].position[0] = targs[i].position[0];
        targscpy[i].position[1] = targs[i].position[1];
        targscpy[i].toughness = targs[i].toughness;
        targscpy[i].damage = targs[i].damage;
        targscpy[i].numAttacks = 0;
        targscpy[i].attackers = 0;
    }
    
}

void printModel( model *mod , uav planes[], target targs[], base * bs ){

	printf("Model: %f %f %f\n %f %f\n %i %i \n\n", mod->targetMeanDistance,
		mod->targetStandardDeviation,
		mod->toughness,
		mod->signalStrength,
		doub( mod->rand ),
		mod->runs,
		mod->runLength 
	);
	
	int i;
	for( i = 0; i < 1; ++i ){
		printf("UAV %i: %f\n %f %f\n %f %f\n %f %f %f\n %f %f\n %i %i %i\n %f %i %f\n\n", i, planes[i].mass, planes[i].position[0], planes[i].position[1],
							  planes[i].velocity[0], planes[i].velocity[1],
						planes[i].max_force, planes[i].max_speed, planes[i].orientation,
						planes[i].basePos[0], planes[i].basePos[1], planes[i].attacks,
						planes[i].kills, planes[i].detections, planes[i].fuel_consumed, planes[i].ticks, planes[i].fuel );

		printf("UAV Particle %i: %f %f %f\n %f %f\n %f %f %f\n %f %f %f\n %i %f\n %f %f %i\n", i, planes[i].pPos[0],planes[i].pPos[1],planes[i].pPos[2],
						planes[i].pVel[0], planes[i].pVel[1], planes[i].pBest[0],planes[i].pBest[1],planes[i].pBest[2],
						planes[i].lBest[0], planes[i].lBest[1] ,planes[i].lBest[2] ,planes[i].leader, planes[i].cooperateVal,
						planes[i].constriction, doub( planes[i].rand ), planes[i]. state );
	}

	for( i = 0; i < 1; ++i ){
		printf("Target %i: %f %f\n %f %f\n %i\n", i, targs[i].position[0], targs[i].position[1], targs[i].toughness,
												targs[i].damage, targs[i].numAttacks );
		//double position[2];
    	//double toughness;
    	//double damage;
    	//node * attackers;
    	//int numAttacks;
	}

	printf("Base: %f %f\n %i %i %f\n", bs->position[0], bs->position[1], bs->nextFuel, bs->nextLaunch, doub( bs->rand ) );

	//double position[2];
   // node * fuel;
    //node * launch;
    //int nextFuel;
   // int nextLaunch;
   // my_random * rand;

}

void masterLoop( int proc_id, int num_procs, my_random * rd ){

	model real_mod;
	double com_range[30];
	double max_visi[30];
	double tough[30];
	base bs;
    uav planes[NUM_UAVS];
    target targs[NUM_TARGETS];


	setupModel( &real_mod, rd );

	// The real unknown uncontrollable model factors
	real_mod.signalStrength = 6;
    real_mod.targetMaxVisibility = 5;
    real_mod.toughness = 10;

	// Ranges of initial uncertainty
	double crRadius = 20;
	double visRadius = 20;
	double toRadius = 5; 

	// Initialize the population of chromosomes
	int pop_size = num_procs - 1;
    individual population[pop_size];
    individual children[pop_size];  
	setupPopulation( population, pop_size, rd );

	
    //double fitness;
    stats st;  
    detailed_stats dst;

	// Setup the "real" model for the emulator
    initialize( &real_mod, planes, targs, &bs );

	// Calling once before loop... hopefully it won't blow up.
    setupStats( &st, &dst );

	int remainingTicks = 5000;
	int generation = 1;
	int mc = 0;	// mission complete 
	int tk = 0; // total kills
	dst.remainingTargets[0] = NUM_TARGETS - st.totalKills;
	//int rt = 0; // remaining targets
	
	while ( st.totalKills < NUM_TARGETS || remainingTicks <= 0 ){
		
		// Setup PME
		setUncontrolled( com_range, 30, real_mod.signalStrength, crRadius, rd );
		setUncontrolled( max_visi, 30, real_mod.targetMaxVisibility, visRadius, rd );
		setUncontrolled( tough, 30, real_mod.toughness, toRadius, rd );

		//printf("Proc %i: Sending CMEs...\n", proc_id, pop_size);
		// Send CME 
		int i, j;
		for( i = 0; i < pop_size; ++i ){
			sendCME( com_range, max_visi, tough, population[i], planes, targs, &bs, ( i + 1 ), remainingTicks );
		}

		//printf("Proc %i: %i CMEs sent...\n", proc_id, pop_size);
		// Prepare and run emulator
		receiveFitness( population, pop_size );
		//printf("Fitness received...\n");
		int best = bestIndividual( population, pop_size );

		printf("Generation %i best fitness: %f\n", generation, population[best].fitness );

		configureUAVS( planes, &population[best] );

		// Record the chromosome used
		dst.chromosomes[generation].fitness = population[best].fitness;
		for( i = 0; i < 10; ++i )
			dst.chromosomes[generation].genes[i] = population[best].genes[i];

		real_mod.runLength = 100;
		//printModel( &real_mod , planes, targs, &bs );
		mc = st.missionComplete;
		tk = st.totalKills;
		//rt = dst.remainingTargets;
		st.totalKills = 0;

		// Run the emulator!
		run( &real_mod , &st, &dst, planes, targs, &bs, 0 );

		st.missionComplete = st.missionComplete + mc;
		st.totalKills = st.totalKills + tk;
		dst.remainingTargets[generation] = NUM_TARGETS - st.totalKills;
		//printf("Emulator completed.\n");
		remainingTicks = remainingTicks - real_mod.runLength;

		// Perform genetic operations
		individual p1;
		individual p2;
		int l = pop_size / 2;
    	for( i = 0; i < l; ++i ){
			//printf("begin genetic...\n");
	        j = i * 2;
	        proportionalSelection( population, pop_size, &p1, &p2 , rd );
			//tournamentSelection( population, pop_size, 2, &p1, &p2 , rd );
			//printf("selection...\n");
			//printf("selection...\n");
	        crossover1( &p1, &p2, &children[j], &children[j+1], rd );
			//printf("crossover...\n");
	        mutation( &children[j], 0.05, rd ); 
			//printf("mutation...\n");
	        mutation( &children[j+1], 0.05, rd );
			//printf("end genetic...\n");
    	}

		// Replace parents with children
		noncompeteSurvival( population, children, pop_size );

		// Reduce uncertainties
		crRadius = crRadius / 2;
		visRadius = visRadius / 2;
		toRadius = toRadius / 2; 

		//printf("Reducing uncertainties...\n");
		generation++;
	}

	// Send termination signals
	int i;
	for( i = 0; i < pop_size; ++i ){
			// This is not a CME
			int cme = 0;
			int tagNum = (i+1) * 10000;
			MPI_Send( &cme, 1, MPI_INT, ( i + 1 ), tagNum, MPI_COMM_WORLD );
	}

	// Record data from emulator
	//real_mod.outFile = "uav_stats";
	//real_mod.outDetailedFile = "detailed_uav_stats";
	strcpy( real_mod.outFile, "uav_results");
    strcpy( real_mod.outDetailedFile, "uav_detailed_results");

	writeResults( st, &real_mod );
	writeDetailedResults( dst, &real_mod, generation );
    //char outDetailedFile[100];

}

void slaveLoop( int proc_id, int num_procs, my_random * rd ){

	model mod[30];
	model temp_model;
	individual ind;
	temp_model.rand = rd;

	base bs, temp_bs;
    uav planes[NUM_UAVS], temp_planes[NUM_UAVS];
    target targs[NUM_TARGETS], temp_targs[NUM_TARGETS];
	double fitness;
    stats st;  
    detailed_stats dst;

	//int remainingTicks;

	//printf("Proc: %i entering slave loop...\n", proc_id );
	int i, j;
	for( i = 0; i < 30; ++i )
		setupModel( &mod[0], rd );
	
	//printf("Proc: %i models setup...\n", proc_id );
	// One time initialization of planes, targs, and base
	initialize( &mod[0], planes, targs, &bs );
	
	//printf("Proc: %i models initialized...\n", proc_id );
	for( i = 0; i < NUM_UAVS; ++i )
		temp_planes[i].rand = planes[i].rand;
	//printf("Proc: %i randoms set...\n", proc_id );

	for( ; ; ){

		fitness = 0;
		//printf("Proc: %i entering for loop...\n", proc_id );
		int rcvd = receiveCME( mod, &ind, planes, 
				targs, &bs, proc_id );
		//printf("Proc: %i CME received...\n", proc_id );
		if( rcvd == 0 )
			return;

		// TODO: set to 30 iterations for each loop
		for( i = 0; i < 30; ++i ){		
			
			for( j = 0; j < 1; ++j ){

				// Setup model
				copyModel( &mod[i], &temp_model, planes, temp_planes, targs, temp_targs, &bs, &temp_bs );
				configureUAVS( temp_planes, &ind );	
	
				//printf("Proc: %i model copied and configured...\n", proc_id );
				
				// Reset stats
				setupStats( &st, &dst );

				//printModel( &temp_model , temp_planes, temp_targs, &temp_bs );

				// Run model
				run( &temp_model , &st, &dst, temp_planes, temp_targs, &temp_bs, 0 );

				fitness = fitness + (double)( temp_model.runLength - st.missionComplete );

				int num = j + ( i * 30 );
				//if( proc_id == 1 )
				//	printf( "rl: %i mc: %i fit: %f\n", temp_model.runLength, st.missionComplete, fitness );
			}
		}
	
		// Average time remaining
		//fitness = fitness / 30 * 30 ;
		fitness = fitness / 30;
		//if( proc_id == 1 )
		//	printf( "avgfit: %f\n", fitness );
		
		//printf("Proc: %i Sending result.\n", proc_id );
		// Send result
		MPI_Send( &fitness, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD );
		//printf("Proc: %i Result sent.\n", proc_id );
	}

}

/*!
 *  The main function.
 *  \return 0 if successful and non-zero otherwise.
 */
int main( int argc, char** argv ) {
    
	int proc_id, num_procs;	
	my_random * rd = malloc( sizeof( my_random ));


	setupMPI( &argc, &argv, &proc_id, &num_procs );	

	printf("Proc: %i started...\n", proc_id );
	
	setupRandom( rd, proc_id );

	if( proc_id == 0 )
		masterLoop( proc_id, num_procs, rd );
	else
		slaveLoop( proc_id, num_procs, rd );
	
	free( rd );

	printf("Proc: %i finishing...\n", proc_id );

	// Stop MPI
	MPI_Finalize();

	return (EXIT_SUCCESS);
}




