
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
 * File:   simrandom.c
 * Author: bradley
 *
 * Created on September 15, 2007, 10:36 PM
 */

#include "assert.h"
#include "simtypes.h"

void seedMyRandom( my_random * rand, unsigned long long seed){
    
    rand->w = 1;
    rand->v = 4101842887655102017LL;
    rand->u = seed ^ rand->v;
    int64( rand );
    rand->v = rand->u;
    int64( rand );
    rand->w = rand->v;
    int64( rand );
}

unsigned long long int64( my_random * rand ){
    
    rand->u = rand->u * 2862933555777941757LL + 7046029254386353087LL;
    
    rand->v ^= rand->v >> 17;
    rand->v ^= rand->v << 31;
    rand->v ^= rand->v >> 8;
    
    rand->w = 4294957665U * ( rand->w & 0xffffffff ) + ( rand->w >> 32 );
    
    unsigned long long x = rand->u ^ ( rand->u << 21 );
    x ^= x >> 35;
    x ^= x << 4;
    
    return ( x + rand->v ) ^ rand->w;
}

double doub( my_random * rand ){
    return 5.42101086242752217E-20 * int64( rand );
}

unsigned int int32( my_random * rand ){
    return (unsigned int) int64( rand );
}
/*
double Lognormal( double mu, double sigma, gsl_rng * rnd ){
    
    double result = gsl_ran_lognormal( rnd, mu, sigma );
    
    return result;
}

double Normal( double mu, double sigma, gsl_rng * rnd ){
    
    double result = gsl_ran_gaussian( rnd, sigma ) + mu;
   
    return result;
}
*/

double uniform( double lower, double upper, my_random * rand ){
    
    assert( upper >= lower );
    
    double result = doub( rand );
    double diff = upper - lower;
    result = result * diff;
    
    return result + lower;
}

int uniformInt( int lower, int upper, my_random * rand ){
    
    assert( upper >= lower );
    
    int diff = upper - lower;
    int result = int32( rand ) % diff;   
    
    return result + lower;
}

// Box-Muller method
double normal( double mu, double sigma, my_random * rand ){
    
    double v1, v2, rsq, fac;
    
    do{
        v1 = 2.0 * doub( rand ) - 1.0;
        v2 = 2.0 * doub( rand ) - 1.0;
        rsq = v1 * v1 + v2 * v2;
    }while ( rsq >= 1.0 || rsq == 0.0 );
    
    fac = sqrt( -2.0 * log( rsq ) / rsq );
    return mu + sigma * v2 * fac;
}

