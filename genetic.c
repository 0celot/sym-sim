
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
 * File:   genetic.c
 * Author: bradley
 *
 * Created on October 18, 2007, 8:15 PM
 */

#include "simtypes.h"

void crossover1( const individual *parentA, const individual *parentB, individual *childA, individual *childB, my_random * rand ){
    
    int n = NUM_UAVS * 4;   // number of bits in the chromosome
    int l = NUM_UAVS / 2;   // number of indexes in chromosome arrays
    
    // Obtain a cut point
    int cut;
    cut = uniformInt( 0, n, rand );
   
    //printf(" Cut: %i \n", cut);
    
    int i,j,k;
    for(i = 0; i < l; ++i){
                
        for(j = 0; j < 8; ++j){
       
            k = i * 8 + j;
            
            // select parent for each buffer
            if( k < cut ){
                childA->genes[i] = childA->genes[i] | ( parentA->genes[i] & ( 1 << ( 7 - j ))); 
                childB->genes[i] = childB->genes[i] | ( parentB->genes[i] & ( 1 << ( 7 - j )));
            }
            else{
                childA->genes[i] = childA->genes[i] | ( parentB->genes[i] & ( 1 << ( 7 - j ))); 
                childB->genes[i] = childB->genes[i] | ( parentA->genes[i] & ( 1 << ( 7 - j )));
            }          
        }
    }
}

void crossover2( const individual *parentA, const individual *parentB, individual *childA, individual *childB, my_random * rand ){
    
    int n = NUM_UAVS * 4;   // number of bits in the chromosome
    int l = NUM_UAVS / 2;   // number of indexes in chromosome arrays
    
    // Obtain 2 distinct cut points
    int cut1, cut2, temp;
    cut1 = uniformInt( 0, n, rand );
    cut2 = cut1;
    while( cut2 == cut1 )
        cut2 = uniformInt( 0, n, rand );
    
    // Make sure cut1 is the lower cut point
    if( cut1 > cut2 ){
        temp = cut2;
        cut2 = cut1;
        cut1 = temp;
    }
    
   // printf(" Cut1: %i, Cut2: %i \n", cut1, cut2);
    
    int i,j,k;
    for(i = 0; i < l; ++i){
                
        for(j = 0; j < 8; ++j){
       
            k = i * 8 + j;
            
            // select parent for each buffer
            if( k < cut1  || k >= cut2 ){
                childA->genes[i] = childA->genes[i] | ( parentA->genes[i] & ( 1 << ( 7 - j ))); 
                childB->genes[i] = childB->genes[i] | ( parentB->genes[i] & ( 1 << ( 7 - j )));
            }
            else{
                childA->genes[i] = childA->genes[i] | ( parentB->genes[i] & ( 1 << ( 7 - j ))); 
                childB->genes[i] = childB->genes[i] | ( parentA->genes[i] & ( 1 << ( 7 - j )));
            }          
        }
    }
}

void mutation( individual *chr, double rate, my_random * rand ){
    
    int l = NUM_UAVS / 2;   // number of indexes in chromosome arrays
    
    double num;
    int i,j,k;
    for(i = 0; i < l; ++i){
        for(j = 0; j < 8; ++j){
                
            num = doub( rand  );
            
            // flip the current bit
            if( num <= rate)
                chr->genes[i] = chr->genes[i] ^ ( 1 << ( 7 - j )) ;
        }
    }
}

void proportionalSelection( individual pop[], int n, individual * parentA, individual * parentB , my_random * rand ){
    
    double totalFit = 0;
    double s;
    
    int i;
    for(i = 0; i < n; ++i)
        totalFit += pop[i].fitness;
    
    s = uniform( 0, totalFit, rand );
    
    for( i = 0; i < n; ++i){
        if( s <= pop[i].fitness )
            parentA = &pop[i];
        else
            s -= pop[i].fitness;
    }
    
    parentB = parentA;
    
    while( parentB == parentA ){
        
        s = uniform( 0, totalFit, rand );
    
        for( i = 0; i < n; ++i){
            if( s <= pop[i].fitness ){
                parentB = &pop[i];
                break;
            }
            else
                s -= pop[i].fitness;
        }
    }
    
    //printf( "Total fitness: %f \n", totalFit );
    //printf( "Fitness of parentA: %f \n", parentA->fitness );
    //printf( "Fitness of parentB: %f \n", parentB->fitness );
}

void tournamentSelection( individual pop[], int n, int k, individual * parentA, individual * parentB , my_random * rand ){

	double totalFit = 0;
    double s;
    
	int competitors[k];

    int i, j, idx, best;
	double best_fit;
	// select each parent
    for( i = 0; i < 2; ++i ){

		// select each competitor
		for( j = 0; j < k; ++j ){
			
			idx = uniformInt( 0, ( n - 1 ), rand );
			competitors[j] = idx;
		}

		// compete
		best = 0;
		idx = competitors[0];
		best_fit = pop[idx].fitness;
		for( j = 1; j < k; ++j ){

			idx = competitors[j];
			if( pop[idx].fitness > best_fit ){
				best = j;
				idx = competitors[j];
				best_fit = pop[idx].fitness;
			}
		}

		idx = competitors[best];
		if( i == 0 )
			parentA = &pop[idx];

		else
			parentB = &pop[idx];
		
	}
        
	return;
}

void competeSurvival( individual pop[], individual children[], int n ){
    
    int l = NUM_UAVS / 2;   // number of indexes in chromosome arrays
    
    int i, j, k;
    for( i = 0; i < n; ++i ){  // for each child

		for( j = 0; j < n; ++j ){ // for each member of existing pop

			if( children[i].fitness > pop[j].fitness ){
	        	for( k = 0; k < l; ++k)
	            	pop[j].genes[k] = children[i].genes[k];
	       
	        	pop[j].fitness = children[i].fitness;
				break;
			}
		}
    }

}

void noncompeteSurvival( individual pop[], individual children[], int n ){
    
    int l = NUM_UAVS / 2;   // number of indexes in chromosome arrays
    
    int i, j;
    for( i = 0; i < n; ++i ){
        for( j = 0; j < l; ++j)
            pop[i].genes[j] = children[i].genes[j];
       
        pop[i].fitness = children[i].fitness;
    }
}

