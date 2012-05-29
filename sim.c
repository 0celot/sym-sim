
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
 * File:   sim.c
 * Author: bradley
 *
 * Created on September 16, 2007, 4:49 PM
 */

#include "simtypes.h"

/*!
 *  Performs all setup actions needed for a single simulation run. 
 *  \param mod A pointer to a model specification structure.
 *  \param planes An array of uav structures to be initialized.
 *  \param targs An array of target structures to be initialized.
 *  \param bs A pointer to a base structure to be initialized.
 *  \param rnd A pointer to pointer of a random number generator to be seeded.
 */
void initialize( const model * mod, uav planes[], target targs[], 
        base * bs ){
    

    
    // Initialize the base
    bs->position[0] = MAP_WIDTH / 2;
    bs->position[1] = MAP_HEIGHT / 2;
    bs->fuel = 0;
    bs->launch = 0;
    bs->rand = mod->rand;
 
    bs->nextFuel = uniformInt( MIN_FUEL, MAX_FUEL, bs->rand );
    bs->nextLaunch = uniformInt( MIN_LAUNCH, MAX_LAUNCH, bs->rand );
    
    
    // Initialize all UAVs
    double len;
    double temp[2] = { 0 , 0 };
    double origin[2] = { 0 , 0 };
    double startFit = 0 ;
    //double phi = PHI_ONE + PHI_TWO;
    double constrict = 1.0; //2.0 / a;
    //double constrict = 2.0 / fabs( 2.0 - phi - sqrt( phi * phi - 4 * phi ));
    int i;
    for( i = 0; i < NUM_UAVS; ++i ){
        
        planes[i].position[0] = bs->position[0];
        planes[i].position[1] = bs->position[1];
        
        temp[0] = uniform( -1, 1, mod->rand );
        temp[1] = uniform( -1, 1, mod->rand );
        len = distance( temp, origin );
        planes[i].velocity[0] = temp[0] / len;
        planes[i].velocity[1] = temp[1] / len;
        
        //planes[i].velocity[0] = -1;
        //planes[i].velocity[1] = -1;
        //len = distance( planes[i].velocity, origin );
        //planes[i].velocity[0] = planes[i].velocity[0] / len;
        //planes[i].velocity[1] = planes[i].velocity[1] / len;
        
        planes[i].basePos[0] = bs->position[0];
        planes[i].basePos[1] = bs->position[1];
        
        planes[i].orientation = 0;
        rotate( &planes[i] );
        planes[i].mass = 2;
        planes[i].max_speed = 0.5;
        planes[i].max_force = 1;
        
        planes[i].attacks = 0;
        planes[i].kills = 0;
        planes[i].detections = 0;
        planes[i].fuel_consumed = 0;
       // planes[i].turns = 0;
        planes[i].ticks = 0;
        planes[i].fuel = INITIAL_FUEL;
        planes[i].group = 0;
        planes[i].targets = 0;
        
        
        //if( i == 0 ){
       //     planes[i].leader = 1;
       //     planes[i].cooperateVal = 0.1;
       // }
       // else{
            //planes[i].leader = 0;
            //planes[i].cooperateVal = 0.5;
           // planes[i].cooperateVal = gsl_ran_flat( *rnd, MIN_COOPERATE, 
           //     MAX_COOPERATE );
       // }
        
        // Initialize the UAV's particle
        planes[i].pPos[0] = uniform( MIN_BASE_DISTANCE, 
                MAX_BASE_DISTANCE, mod->rand );
        planes[i].pPos[1] = uniform( MIN_COOPERATE, 
                MAX_COOPERATE, mod->rand);
        //planes[i].pPos[0] = 15;
        //planes[i].pPos[1] = 6;
        planes[i].pPos[2] = startFit;
        
        planes[i].pVel[0] = uniform( 0, MAX_BASE_VELOCITY, mod->rand );
        planes[i].pVel[1] = uniform( 0, MAX_COOPERATE_VELOCITY, mod->rand );

        planes[i].pBest[0] = planes[i].pPos[0];
        planes[i].pBest[1] = planes[i].pPos[1];
        planes[i].pBest[2] = planes[i].pPos[2];
        
        planes[i].lBest[0] = planes[i].pPos[0];
        planes[i].lBest[1] = planes[i].pPos[1];
        planes[i].lBest[2] = planes[i].pPos[2];
        
        planes[i].constriction = constrict;
        
        planes[i].rand = mod->rand;
        
        planes[i].state = UAV_STATE_INACTIVE;
        addBack( &( bs->launch ), ( void * ) &( planes[i] ));
    }
    
    double rdist = 0;
    // Initialize all targets
    for( i = 0; i < NUM_TARGETS; ++i ){
        
        // Determine position
        temp[0] = uniform( -1, 1, mod->rand );
        temp[1] = uniform( -1, 1, mod->rand );
        
        len = distance( temp, origin );
        
        assert( len != 0 );
        temp[0] = temp[0] / len;
        temp[1] = temp[1] / len;
        
        rdist = normal( mod->targetMeanDistance, 
                mod->targetStandardDeviation, 
                mod->rand ); 
        
        if( rdist > MAX_BASE_DISTANCE )
            rdist = MAX_BASE_DISTANCE;
        

        temp[0] = ( temp[0] * rdist ) + bs->position[0];
        temp[1] = ( temp[1] * rdist ) + bs->position[1];
                
        // Assign position
        targs[i].position[0] = temp[0];
        targs[i].position[1] = temp[1];
        
        // Assign toughness
        targs[i].toughness = mod->toughness;
        
        // Set damage
        targs[i].damage = 0;
        
        // Set numAttacks
        targs[i].numAttacks = 0;

        targs[i].attackers = 0;
    }
    
    // Initialize SDL and OpenGL
    //initDisplay( MAP_WIDTH, MAP_HEIGHT );
}

void configureUAVS( uav planes[], const individual *chromosome ){
    
    int cp = 0;
    //int coop = 0;
    
    int i, j, r;
    for( i=0; i < NUM_UAVS; ++i ){
        
        planes[i].leader = 0;
        //planes[i].cooperateVal = 0;
        
        j = i / 2;
        r = i % 2;
        if( r == 0 ){
            planes[i].leader = 0x80 & chromosome->genes[j];
            planes[i].leader >>= 7;
            cp = 0x0070 & chromosome->genes[j];
            cp >>=4;       
        }
        else{
            planes[i].leader = 0x08 & chromosome->genes[j];
            planes[i].leader >>= 3;
            cp = 0x0007 & chromosome->genes[j];       
        }
        
        cp = invGray( cp );
        planes[i].cooperateVal = (double) cp / 7;
    }
}

int gray( int n ){
    return n ^ ( n >> 1 );
}

int invGray( int n ){
    
    int ish = 1;
    int ans = n;
    int idiv;
    
    for( ; ; ){
        
        ans ^= ( idiv = ans >> ish );
        
        if( idiv <= 1 || ish == 16 )
            return ans;
        
        ish <<= 1;
    }
}

/*!
 *  Executes a single simulation replication or run.
 *  \param mod A pointer to a model specification structure.
 *  \param st A pointer to a simulation statistics structure.
 *  \param planes An array of uav structures.
 *  \param targs An array of target structures.
 *  \param bs A pointer to a base structure.
 *  \param start Indicates the starting simulation tick.
 */
void run( const model * mod , stats * st, detailed_stats * dst, uav planes[], target targs[], base * bs, int start ){
    
	int pID; // Get the current process ID
	MPI_Comm_rank( MPI_COMM_WORLD, &pID );

	//if( pID == 1 )
	//	printf("Model run length: %i and %i kills\n", mod->runLength, st->totalKills );

	int numLiveTargets = NUM_TARGETS;
    int i;   
	for( i = 0; i < NUM_TARGETS; ++i ){
		if( targs[i].toughness <= 0 )
			--numLiveTargets;
	}

    for( i = start; i < mod->runLength; ++i ){
        
        step( mod, st, dst, planes, targs, bs );
        
        //display( targs, NUM_TARGETS, planes, NUM_UAVS );
        // Pause for 0.05 seconds
        //SDL_Delay(50);
        
        if( st->totalKills == numLiveTargets ){
            //st->missionComplete = i;
			//printf( "break\n" );
            break;
        }
    }

	st->missionComplete = i;
    //if( pID == 1 ) 
    //	printf("Mission complete %i with %i kills\n", st->missionComplete, st->totalKills );
    
}

/*!
 *  Performs a single simulation tick in which each UAV is permitted to act.
 *  \param mod A pointer to a model specification structure.
 *  \param st A pointer to a simulation statistics structure.
 *  \param planes An array of uav structures.
 *  \param targs An array of target structures.
 *  \param bs A pointer to a base structure.
 */
void step( const model * mod, stats * st, detailed_stats * dst, uav planes[], target targs[], base * bs ){


    //arrival( &planes[0], seekPos, steering );
    //nextPosition( &planes[0] , steering );
    //rotate( &planes[0] );
    
    
    // Update group and target information
    int i;
    for( i = 0; i < NUM_UAVS; ++i ){
            
        if( hasUAV( bs,  &( planes[i] ) ) == 0 ){
            
            int num_detections = 0;
            int num_group = 0;
            
            if( planes[i].state == UAV_STATE_ACTIVE ){
                num_detections = detectTargets( &( planes[i] ), targs, NUM_TARGETS, mod );
                num_group = detectGroup( &( planes[i] ), planes, NUM_UAVS, mod, bs );
            }
            
            planes[i].detections = planes[i].detections + ( num_detections * num_group );
        }
        
    }
    
    
    
    
    // Consolidate target information
    node * tcpy[NUM_UAVS];  
    node * gcpy[NUM_UAVS];   
    node * tempNode;
    uav * tempUAV;

    for( i = 0; i < NUM_UAVS; ++i ){
        
        tcpy[i] = 0;
        gcpy[i] = 0;
        if( planes[i].state == UAV_STATE_ACTIVE ){
            copy( &( tcpy[i] ), planes[i].targets ); 
            copy( &( gcpy[i] ), planes[i].group ); 
        }
    }
    
    int j, n;
    for( i = 0; i < NUM_UAVS; ++i ){
        if( planes[i].state == UAV_STATE_ACTIVE ){
            tempNode = planes[i].group;
            // The number of UAVs in the i'th UAV's group
            n = length( tempNode );

            // For each UAV in the i'th  UAV's group
            for( j = 0; j < n; ++j ){

                // Get a UAV that is in the i'th UAV's group list
                tempNode = gcpy[j];

                if(tempNode != NULL){

                    // If UAVs are in signal range
                    if( distance( planes[j].position, planes[i].position ) 
                        < mod->signalStrength ){

                        // Merge target lists
                        tempUAV = ( uav * )tempNode->data;
                        mergeUnique( &tcpy[i], tempUAV->targets );

                    }
                
                }

            }
        }
    }
    
    for( i = 0; i < NUM_UAVS; ++i )
        if( planes[i].state == UAV_STATE_ACTIVE )
            mergeUnique( &(planes[i].targets), tcpy[i] );
    
    // Perform base processes
    baseRefuelProcess( bs, mod );
    baseRelaunchProcess( bs, mod );
    
    //printf("Begin movement.\n");
    // Move UAVs
    for( i = 0; i < NUM_UAVS; ++i ){      
        if( planes[i].state == UAV_STATE_ACTIVE )
            st->totalFuelConsumed += moveUAV( &planes[i], bs, mod );

    }
    
    // Attack
    for( i = 0; i < NUM_UAVS; ++i )
        if( planes[i].state == UAV_STATE_ACTIVE )
            st->totalAttacks += attackNearest( &planes[i] );
    
    // Resolve attacks
    for( i = 0; i < NUM_TARGETS; ++i )
        st->totalKills += resolveAttacks( &targs[i] );
    
    // Update particles
    if( planes[0].ticks % SAMPLING_INTERVAL == 0 && OPTIMIZING != 0 ) {
        for( i = 0; i < NUM_UAVS; ++i ){
            if( planes[i].state == UAV_STATE_ACTIVE ){
                evaluateParticle( &planes[i] );
                updateParticlePersonalBest( &planes[i] );
                       
            }
        }
        
        // Can only be called after all particles are evaluated
        for( i = 0; i < NUM_UAVS; ++i )
            if( planes[i].state == UAV_STATE_ACTIVE )
                updateParticleLocalBest( &planes[i]);
        
        for( i = 0; i < NUM_UAVS; ++i ){
            if( planes[i].state == UAV_STATE_ACTIVE ){
                updateParticlePosition( &planes[i] );
                updateParticleVelocity( &planes[i] );
                
            }
        }
        
        // Record update for detailed stats
        int idx = (int) planes[0].ticks / SAMPLING_INTERVAL;
        updateDetailedStats( dst, st, idx, planes );
    }
    
    //printf("finalize...\n");
    // Finalize
    for( i = 0; i < NUM_UAVS; ++i ){
        
        //if( i == 0 )
        //    printf("UAV 0...\n");
        tempNode = tcpy[i];
        clear( tempNode );
        clear( gcpy[i] );      
        finalizeUAV( &planes[i] );
    }
    if( planes[0].ticks % 10 == 0 ) {
       // printf(" Base distance: %2.2f %2.2f %2.2f %2.2f %2.2f "
       //         "%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f "
        //        "%2.2f %2.2f %2.2f %2.2f %2.2f \n", 
       //         planes[0].lBest[0], planes[1].lBest[0], planes[2].lBest[0], planes[3].lBest[0], planes[4].lBest[0],
       //         planes[5].lBest[0], planes[6].lBest[0], planes[7].lBest[0], planes[8].lBest[0], planes[9].lBest[0],
       //         planes[10].lBest[0], planes[11].lBest[0], planes[12].lBest[0], planes[13].lBest[0], planes[14].lBest[0],
       //         planes[15].lBest[0], planes[16].lBest[0], planes[17].lBest[0], planes[18].lBest[0], planes[19].lBest[0] );
        //printf(" UAV 0 tick: %i atk: %i det: %i turn: %f gd: %f bd: %f fit: %f \n", planes[0].ticks,
        //    planes[0].attacks, planes[0].detections, planes[0].turns, gd, bd, fit );
        //printf(" UAV 0 tick: %i bd: %f fit: %f pbest %f pbestfit %f \n", planes[0].ticks,
        //        planes[0].pPos[0], planes[0].pPos[2], planes[0].pBest[0], planes[0].pBest[2] );
    }
    
}

/*!
 *  Determines whether or not an airbase is currently servicing a 
 *  particular UAV.
 *  \param bs A pointer to a base structure in question.
 *  \param plane A pointer to the UAV in question.
 *  \return 1 if the UAV is being serviced by the base and 0 otherwise.
 */
int hasUAV( const base * bs, const uav * plane ){
    
    node * ndFuel = bs->fuel;
    node * ndLaunch = bs->launch;
    int nFuel = length( ndFuel );
    int nLaunch = length( ndLaunch );
    
    int i;
    for( i = 0; i < nFuel; ++i ){
        
        const uav * plane2 = ( const uav * )ndFuel->data;
        if( plane2 == plane )
            return 1;
        
        ndFuel = ndFuel->next;
    }
    
    for( i = 0; i < nLaunch; ++i ){
        
        const uav * plane2 = ( const uav * )ndLaunch->data;
        if( plane2 == plane )
            return 1;
        
        ndLaunch = ndLaunch->next;
    }
    
    return 0;
}

/*!
 *  Applies damage results to the target based on the current attackers.
 *  Credits attackers with scoring hits and a kill if the target is eliminated.
 *  If any two UAVs attack the target from an angle of more than 120 degrees then
 *  damage is doubled.
 *  \param targ A pointer to a target structure in question.
 *  \return 1 if the target is eliminated and 0 otherwise.
 */
int resolveAttacks( target * targ ){
           
    if( targ->numAttacks == 0 )
        return 0;

    int n = length( targ->attackers );
    
    if( n == 0 )    
        return 0;
   
    node * nd_i = targ->attackers;
    node * nd_j = targ->attackers;

    uav * u_i = 0;
    uav * u_j = 0;
    double vecA[2] = { 0, 0 };
    double vecB[2] = { 0, 0 };
    double angle = 0;
    double two_thirds_pi = 2.094395103;
    int flanked = 0;
    
    int killed = 0;
    // Is there a flanking bonus?
    int i, j;
    for( i = 0; i < n; ++i ){
        
        u_i = ( uav *)nd_i->data;
        vecA[0] = u_i->position[0] - targ->position[0];
        vecA[1] = u_i->position[1] - targ->position[1];

        for( j = 0; j < n; ++j ){
            
            u_j = ( uav *)nd_j->data;
            vecB[0] = u_j->position[0] - targ->position[0];
            vecB[1] = u_j->position[1] - targ->position[1];

            angle = angleTwoVectors( vecA, vecB );

            if( angle >= two_thirds_pi || angle <= -two_thirds_pi ){
                flanked = 1;
                break;
            }
            nd_j = nd_j->next;
        }

        nd_j = targ->attackers;

        if( flanked != 0 )
            break;
        nd_i = nd_i->next;
    }

    double base_damage = targ->damage;
    
    if( flanked != 0 ){

        // Apply flanking bonus
        base_damage = base_damage * 2;
    }

    targ->toughness = targ->toughness - targ->damage; 

    // Credit attackers for the kill  
    if( targ->toughness < 0 ){

        for( i = 0; i < n; ++i ){
            
            nd_i = targ->attackers;
            u_i = ( uav *)nd_i->data;
            u_i->kills = u_i->kills + 1;

            nd_i = nd_i->next;
        }
        
        // Record the kill for simulation statistics
        killed = 1;
    }

    targ->damage = 0;
    targ->numAttacks = 0;
    clearAttackers( targ );
    
    return killed;
}

/*!
 *  Clears the attacker list from a given target structure.
 *  \param targ A pointer to the target structure in question.
 */
void clearAttackers( target * targ ){
    
    node * a = targ->attackers;
    int n = length( a );
    
    int i;
    for( i = 0; i < n; ++i)
        removeFront( &a );
    
    targ->attackers = 0;
}

void updateDetailedStats( detailed_stats * dst, const stats * st, int idx, uav planes[] ){
    
    int updated = 0;
    double total_base_dist = 0;
    double total_cooperate = 0;
    double total_fitness = 0;
    double total_neighborhood = 0;
    
    int i;
    for( i = 0; i < NUM_UAVS; ++i ){
        if( planes[i].state == UAV_STATE_ACTIVE ){
            ++updated;
            total_base_dist += planes[i].pPos[0];
            total_cooperate += planes[i].pPos[1];
            total_fitness += planes[i].pPos[2];
            total_neighborhood += length( planes[i].group );
        }
    }
    
    if( updated > 0 ){
        dst->avgBaseDistance[idx] = total_base_dist / updated;
        dst->avgCooperate[idx] = total_cooperate / updated;
        dst->avgFitness[idx] = total_fitness / updated;
        dst->avgNeighborhood[idx] = total_neighborhood / updated;
        dst->remainingTargets[idx] = NUM_TARGETS - st->totalKills;
        ++dst->updates;
    }
}
