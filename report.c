
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
 * File:   report.c
 * Author: bradley
 *
 * Created on October 3, 2007, 1:40 PM
 */

#include "simtypes.h"

/*!
 *  Writes an array of result values to the output file specified 
 *  in the mod parameter.
 *  \param results An array of stats structures, one per simulation run.
 *  \param mod A model specification.
 */
void writeResults( stats results, model * mod )
{
  // Pointer to output file
  FILE *fileOut;
  fileOut = fopen( mod->outFile, "w" );

  int i;
  //for( i=0; i<mod->runs; ++i )
    fprintf( fileOut, "%i, %i, %i, %f\n", results.missionComplete, results.totalAttacks, results.totalKills, results.totalFuelConsumed );

  // Close the output stream	
  fclose( fileOut );
}


void writeDetailedResults( detailed_stats results, model * mod, int generations )
{
  // Pointer to output file
  FILE *fileOut;
  fileOut = fopen( mod->outDetailedFile, "w" );
  
  //detailed_stats avg_results;

  int i,j;
  //for( i=0; i<mod->runs; ++i ){
    
    //fprintf( fileOut, "Replication: %i ********\n\n", i );
    
    for( j=0; j<(generations+1); ++j ){
        
        fprintf( fileOut, "%i, %i, %f, %f, %f, %f, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %f\n", j, results.remainingTargets[j],
            results.avgBaseDistance[j],
            results.avgCooperate[j],
            results.avgFitness[j],
            results.avgNeighborhood[j],
			results.chromosomes[j].genes[0],
			results.chromosomes[j].genes[1],
			results.chromosomes[j].genes[2],
			results.chromosomes[j].genes[3],
			results.chromosomes[j].genes[4],
			results.chromosomes[j].genes[5],
			results.chromosomes[j].genes[6],
			results.chromosomes[j].genes[7],
			results.chromosomes[j].genes[8],
			results.chromosomes[j].genes[9],
			results.chromosomes[j].fitness );
    }
    
    fprintf( fileOut, "\n" );
 // }
  // Close the output stream	
  fclose( fileOut );
}

void printPopulation( individual pop[], int n ){
    
    int i; 
    for( i = 0; i < n; ++i){
        printf(" Individual %i: genes: %x %x %x %x %x %x %x %x %x %x fit: %f\n",
                i,
                pop[i].genes[0], pop[i].genes[1], 
                pop[i].genes[2], pop[i].genes[3], 
                pop[i].genes[4], pop[i].genes[5], 
                pop[i].genes[6], pop[i].genes[7], 
                pop[i].genes[8], pop[i].genes[9],
                pop[i].fitness );
    }
}

