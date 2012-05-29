
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
 * File:   base.c
 * Author: bradley
 *
 * Created on September 25, 2007, 5:06 PM
 */

#include "simtypes.h"

/*!
 *  Called during each step() of a simulation. Refueled UAVs are added to the
 *  base's launch queue.
 *  \param bs A pointer to the air base structure itself.
 *  \param mod A pointer to a model specification structure.
 */
void baseRefuelProcess( base * bs, const model * mod ){
    
    int n = length( bs->fuel );
    uav * u = 0;
    
    if( n > 0 ){
        if( bs->nextFuel == 0 ){
            u = (uav * )removeFront( &( bs->fuel ));
            u->fuel = INITIAL_FUEL;
            addBack( &( bs->launch ), ( void * ) u );
            bs->nextFuel = uniformInt( MIN_FUEL, MAX_FUEL, bs->rand );
    
        }
        else
            --( bs->nextFuel );
    }
}

/*!
 *  Called during each step() of a simulation. Makes a call to randomHeading()
 *  for launched UAVs and sets their state to UAV_STATE_ACTIVE.
 *  \param bs A pointer to the air base structure itself.
 *  \param mod A pointer to a model specification structure.
 */
void baseRelaunchProcess( base * bs, const model * mod ){
    
    int n = length( bs->launch );
    uav * u = 0;
    
    if( n > 0 ){
        if( bs->nextLaunch == 0 ){
            u = (uav * )removeFront( &( bs->launch ));
            u->state = UAV_STATE_ACTIVE;
            randomHeading( u );
            bs->nextLaunch = uniformInt( MIN_LAUNCH, MAX_LAUNCH, bs->rand );
            //printf("Launching...");
        }
        else
            --( bs->nextLaunch );
    }
}

