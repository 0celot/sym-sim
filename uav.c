
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
 * File:   uav.c
 * Author: bradley
 *
 * Created on September 15, 2007, 10:23 PM
 */

#include "simtypes.h"

/*!
 *  Adds any other UAVs within signal range to the UAV's group list.
 *  \param plane The UAV structure in question.
 *  \param group An array of of all UAVs in the simulation.
 *  \param sz Size of the group array
 *  \param mod A pointer to a model specification structure.
 *  \param bs A pointer to a base structure.
 *  \return The number of group mates detected.
 */
int detectGroup( uav * plane, const uav group[], int sz, const model * mod , 
        const base * bs){
    
    int receivers = 0;
    double dist;
    int i;
    for( i = 0; i < sz; ++i ){
        
        const uav *u = &( group[i] );
        
        if( hasUAV( bs, u ) == 0 ){
            // Don't consider self as a group member
            if( plane != u && u->state == UAV_STATE_ACTIVE ){
            //if( plane->position[0] != u->position[0] || 
            //        plane->position[1] != u->position[1] ){

                dist = distance( plane->position, u->position );
                if( dist <= mod->signalStrength ){

                    addFront( &plane->group, (void *)u );
                    
                    if( dist < mod->signalStrength )
                        ++receivers;
                }
            }       
        }
    }

    //printf("Group is %i long.\n", length( plane->group ));
    return receivers;
}

/*!
 *  Adds any targets within detection range to the UAV's target list.
 *  \param plane The UAV structure in question.
 *  \param tgts An array of all targets in the simulation.
 *  \param sz Size of the tgts array
 *  \param mod A pointer to a model specification structure.
 *  \return The number of targets detected.
 */
int detectTargets( uav * plane, const target tgts[], int sz, 
        const model * mod ){
    
    int detections = 0;
    int i, p, r;
    for( i = 0; i < sz; ++i ){
        
        const target *t = &( tgts[i] );
        
        // Only detect "living" targets
        if( t->toughness > 0 ){
        
            p = 1 - distance( plane->position, t->position ) / 
                    mod->targetMaxVisibility;

            r = doub( plane->rand );

            if( r <= p ){
                addFront( &plane->targets, (void*)t );
                ++detections;
                //plane->detections = plane->detections + 1;
            }
        }
    }
    
    return detections;
}

/*!
 *  Adds a list of target structures to the UAV's target list.
 *  \param plane The UAV structure itself.
 *  \param tgts A list of targets to be added.
 */
void addTargets( uav * plane, node * tgts ){
    
    int n = length( tgts );
    
    int i;
    for( i = 0; i < n; ++i ){
        
        target * tg = ( target * )tgts->data;
        if( !hasTarget( plane, tg ))
            addFront( &plane->targets, (void *)tg );
        
        tgts = tgts->next;
    }
}

/*!
 *  Determines whether or not the UAV has detected a certain target.
 *  \param plane The UAV structure itself.
 *  \param tgt An pointer to a target to be queried.
 *  \return 1 if the target has been detected by the UAV and 0 otherwise.
 */
int hasTarget( const uav * plane, const target * tgt ){
    
    node * nd = plane->targets;
    int n = length( nd );
    
    int i;
    for( i = 0; i < n; ++i ){
        
        const target * tg2 = ( const target * )nd->data;
        if( tg2->position[0] == tgt->position[0] && 
                tg2->position[1] == tgt->position[1] )
            return 1;
        
        nd = nd->next;
    }
    
    return 0;
}

/*!
 *  Finds the nearest target to the UAV in it's target list.
 *  \param plane The UAV structure itself.
 *  \return The nearest target or 0 if no targets were present.
 */
target * nearestTarget( const uav * plane ){
      
    node * nd = plane->targets;
    int n = length( nd );
    target * nearest = 0;
    target * temp = 0;
    double dist_nearest = 0;
    double dist_temp = 0;
    
    int i;
    for( i = 0; i < n; ++i ){
           
        if( nearest == 0 ){          
            nearest = ( target * )nd->data;
            dist_nearest = distance( nearest->position, plane->position );
        }
        else{   
            temp = ( target * )nd->data;
            dist_temp = distance( temp->position, plane->position );
            
            if( dist_temp < dist_nearest ){
                nearest = ( target * )nd->data;
                dist_nearest = dist_temp;
            }
        }
        
        nd = nd->next;
    }

    return nearest;
}

/*!
 *  Removes all targets from the UAV's target list.
 *  \param plane The UAV structure itself.
 */
void clearTargets( uav * plane ){
    
    node * t = plane->targets;
    int n = length( t );
    
    int i;
    for( i = 0; i < n; ++i)
        removeFront( &t );
    
    plane->targets = 0;
}

/*!
 *  Removes all UAVs from the specified UAV's group list.
 *  \param plane The UAV structure itself.
 */
void clearGroup( uav * plane ){
    
    node * g = plane->group;
    int n = length( g );
    
    int i;
    for( i = 0; i < n; ++i)
        removeFront( &g );
    
    plane->group = 0;
}

/*!
 *  Moves the UAV according to a performance element implemented by a decision tree.
 *  \param plane A pointer to the UAV structure itself.
 *  \param bs A pointer to the air base structure.
 *  \param mod A pointer to a model specification structure.
 *  \return The amount of fuel consumed during the move.
 */
double moveUAV( uav * plane, base * bs, const model * mod ){
    
    double steering[2] = { 0, 0 };
    double fuel_use = 0;
   // double returning_fuel_use = 0; // used to track fuel consumed during base return
    int rebase = 0;
    
    double current_distance = distance( plane->position, bs->position );
    
    if( current_distance > MAX_BASE_DISTANCE ){
        orbit( plane, bs->position, plane->pPos[0], steering );
        fuel_use = nextPosition( plane , steering );
        rotate( plane );
        plane->fuel = plane->fuel - fuel_use;
        plane->fuel_consumed = plane->fuel_consumed + fuel_use;
        return fuel_use;
    }
    
    if( plane->fuel <= 0 )
        rebase = 1;

    if( rebase != 0 ){
        
        arrival( plane, bs->position , steering );
        nextPosition( plane , steering );
        rotate( plane );
        
        if( distance( plane->basePos, plane->position) < 0.2 ){
            plane->position[0] = plane->basePos[0];
            plane->position[1] = plane->basePos[1];
            plane->state = UAV_STATE_INACTIVE;
            plane->attacks = 0;
            plane->kills = 0;
            plane->detections = 0;
            plane->fuel_consumed = 0;
            clearTargets( plane );
            clearGroup( plane );
            addBack( &( bs->fuel ), ( void *) plane );
            //printf(" Base refuel length: %i\n", length( bs->fuel ));
            return 0;
        }
       
    }
    else{
                
        int groupLength = length( plane->group );
        if( plane->pPos[1] < plane->cooperateVal && groupLength > 0 ){

            int hasLeader = followTheLeader( plane, steering );
            if( hasLeader == 0 ){
                flocking( plane, steering );
                if( length( plane->targets ) > 0){
                    target * nearest = 0;
                    nearest = nearestTarget( plane );
                    assert( nearest != 0 );
                    double seek_steering[2] = { 0, 0 };
                    offsetSeek( plane, nearest->position, 2, seek_steering );
                    steering[0] = steering[0] + 0.2 * seek_steering[0];
                    steering[1] = steering[0] + 0.2 * seek_steering[0];
                }
            }
        }
        else{
            if( length( plane->targets ) > 0){             
                
                target * nearest = 0;
                nearest = nearestTarget( plane );
                assert( nearest != 0 );
                offsetSeek( plane, nearest->position, 2, steering );
                //seek( plane, nearest->position, steering);
            }
            else
                orbit( plane, bs->position, plane->pPos[0], steering );
        }
    
    
        fuel_use = nextPosition( plane , steering );
        rotate( plane );
        plane->fuel = plane->fuel - fuel_use;
        plane->fuel_consumed = plane->fuel_consumed + fuel_use;
    }
    
    return fuel_use;
}


/*!
 *  Attacks the nearest target in the specified UAV's target list if one is present.
 *  \param plane A pointer to the UAV structure itself.
 *  \return 1 if a target was attacked and 0 otherwise.
 */
int attackNearest( uav *plane ){
    
    int attacks = 0;
    int n = length( plane->targets );
    
    if( n == 0 )    
        return 0;
  
    target * tgt = 0; 
    
    tgt = nearestTarget( plane );
    
    if( tgt == 0 )
        return 0;
    
    double dist = distance( plane->position, tgt->position );
    // Attack if target is in range
    if( dist < WEAPON_RANGE ){
       
        ++( tgt->numAttacks );
        ( tgt->damage ) += normal( AVG_WEAPON_EFFECT, STDEV_WEAPON_EFFECT, 
                plane->rand );
        addFront( &tgt->attackers, (void * ) plane );
        plane->attacks = plane->attacks + 1;
        attacks = 1;
    }

    return attacks;
}

/*!
 *  Clears target and group lists. Additionally, fitness counters are reset to 0
 *  at the end of a sampling interval. Called at the end of step() for each UAV.
 *  \param plane A pointer to the UAV structure itself.
 *  \sa step()
 */
void finalizeUAV( uav * plane ){

    int sampling = SAMPLING_INTERVAL;
    
    if( plane->state == UAV_STATE_ACTIVE ){
        if( plane->ticks % sampling == 0){
            if( plane->ticks > 0){
                plane->attacks = 0;
                plane->kills = 0;
                plane->detections = 0;
                plane->fuel_consumed = 0;              
            }
        }

        clearTargets( plane );
        clearGroup( plane );
    }
    plane->ticks = plane->ticks + 1;
    
}

/*!
 *  Determines the distance from a future position to the centroid of all UAVs 
 *  in the specified UAV's group list.
 *  \param next The next position of the specified UAV itself.
 *  \param plane A pointer to the UAV structure itself.
 *  \return The distance to the centroid.
 */
double averageGroupDistance( const double next[], const uav *plane ){
    
    double total = 0;
    int n = length( plane->group );
    
    // Prevent divide by zero
    if( n == 0 )
        return 0;
    
    node * grp = plane->group;
    
    int i;
    for( i = 0; i < n; ++i ){
        
        uav * u = ( uav * )grp->data; 
        total += distance( next, u->position );
        
        grp = grp->next;
    }
           
    return total / (double) n;
}

/*!
 *  Determines the distance from a future position to the centroid of all targets 
 *  in the specified UAV's target list.
 *  \param next The next position of the specified UAV itself.
 *  \param plane A pointer to the UAV structure itself.
 *  \return The distance to the centroid.
 */
double averageTargetDistance( const double next[], const uav *plane ){
    
    double total = 0;
    int n = length( plane->targets );
    
    // Prevent divide by zero
    if( n == 0 )
        return 0;
    
    node * tgts = plane->targets;
    
    int i;
    for( i = 0; i < n; ++i ){
        
        target * targ = ( target * ) tgts->data;
        total += distance( next, targ->position );
        
        tgts = tgts->next;
    }
           
    return total / (double) n;
}

/*!
 *  Should assign a random velocity for the UAV but doesn't do anything at the moment.
 *  \param plane A pointer to the UAV structure itself.
 */
void randomHeading( uav * plane ){
    
    double temp[2] = { 0 , 0 };
    double origin[2] = { 0 , 0 };
    double len = 0;
    
    temp[0] = uniform( -1, 1, plane->rand );
    temp[1] = uniform( -1, 1, plane->rand );
    len = distance( temp, origin );
    //u->heading[0] = temp[0] / len;
   // u->heading[1] = temp[1] / len;
}

