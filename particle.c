
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
 * File:   particle.c
 * Author: bradley
 *
 * Created on September 15, 2007, 10:22 PM
 */

#include "simtypes.h"

/*
 * Calculates a new position and velocity vector based on percept data from the 
 * UAV. Updates pBest if necessary.
 */

void updateParticlePosition( uav *plane )
{
   
    // Calculate a new position for each decision variable
    int i;
    for( i = 0; i < 2; i++ )                
        plane->pPos[i] = plane->pPos[i] + plane->pVel[i];              
      
    
    // Position must not be outside decision var's range
    if( plane->pPos[0] < MIN_BASE_DISTANCE )
        plane->pPos[0] = MIN_BASE_DISTANCE;
    if( plane->pPos[0] > MAX_BASE_DISTANCE )
        plane->pPos[0] = MAX_BASE_DISTANCE;
    if( plane->pPos[1] < MIN_COOPERATE )
        plane->pPos[1] = MIN_COOPERATE;
    if( plane->pPos[1] > MAX_COOPERATE )
        plane->pPos[1] = MAX_COOPERATE;
    
    
}

void updateParticleVelocity( uav *plane )
{
    double a, b, c = 0;
    double delta[2] = { 0, 0 };
      
    double phi1 = PHI_ONE;
    double phi2 = PHI_TWO;
        
    double maxVel[2] = { MAX_BASE_VELOCITY, MAX_COOPERATE_VELOCITY };
    
    
    
    // Update pBest
       
    // Calculate a new velocity for each decision variable
    int i;
    for( i = 0; i < 2; i++ ){
              
        // Calculate  new velocity, delta
        a = plane->pVel[i];
        b = phi1 * doub( plane->rand ) * ( plane->pBest[i] - 
                plane->pPos[i] );
        //b = plane->pBest[i] - plane->pPos[i];
        c = phi2 * doub( plane->rand ) * ( plane->lBest[i] - 
               plane->pPos[i] );
        //c = 0;
        delta[i] = plane->constriction * ( a + b + c );	
        
        
        // Update velocity 
        plane->pVel[i] = delta[i];

        // Velocity must not exceed maximum
        if( plane->pVel[i] > maxVel[i] )
            plane->pVel[i] = maxVel[i];     
        
        if( plane->pVel[i] < - ( maxVel[i] ))
            plane->pVel[i] = - ( maxVel[i] );                     
    }
    
    
    double craziness = CRAZINESS;
        
    if( doub( plane->rand ) <= craziness ){
            plane->pVel[0] = uniform( MIN_BASE_DISTANCE, 
                    MAX_BASE_DISTANCE, plane->rand );
            plane->pVel[0] = uniform( MIN_COOPERATE, 
                    MAX_COOPERATE, plane->rand );
    }

}
/*
 * Replaces the particle's current group best with 'best' if the passed value 
 * has a higher fitness.
 */
void updateParticleLocalBest( uav * plane )
{

    // Age local best value;
    plane->lBest[2] = plane->lBest[2] * 0.9;
    
    node * grp = plane->group;
    int n = length( grp );
    int i;
    
    for( i = 0; i < n; ++i ){
       
        uav *u = (uav*)grp->data;
        double *p = (double *)u->pBest;
        
        if ( *(p+2) >  plane->lBest[2] ){     
            plane->lBest[0] = p[0];
            plane->lBest[1] = p[1];
            plane->lBest[2] = p[2];
        }
 
        grp = grp->next;
    } 
    
}

void updateParticlePersonalBest( uav * plane )
{
    // Age personal best value;
    plane->pBest[2] = plane->pBest[2] * 0.9;
    
    // Update personal best if necessary
    if(plane->pBest[2] < plane->pPos[2] ){
        plane->pBest[0] = plane->pPos[0];
        plane->pBest[1] = plane->pPos[1];
        plane->pBest[2] = plane->pPos[2];
    }
     
}


/*
 * The Agent Fitness function.
 */
void evaluateParticle( uav * plane )
{
    double result = 0;
    
    double attack_denom = pow( plane->attacks, ( plane->kills + 1.0 )) + 2.0;
    double attack_result = 0.5 - ( 1.0 / attack_denom );
    
    double detect_denom = plane->detections + 2;
    double detection_result = 0.5 - ( 1.0 / detect_denom );
    
    result = attack_result + detection_result;  
    
    //if(result > 0 )
    //    printf("Result: %f\n", result );
    
    plane->pPos[2] = result;
}

//void evaluateParticle2( uav * plane )
//{
//    double result = 0;

//}
