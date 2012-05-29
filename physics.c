
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
 * Created on September 26, 2007, 8:24 PM
 */

#include "simtypes.h"

double distance( const double p1[], const double p2[] ){
    
    double x = p1[0] - p2[0];
    double y = p1[1] - p2[1];
    return sqrt(( x * x ) + ( y * y ));
}

void normalize( double vector[] )
{
    
    double origin[2] = { 0, 0 };
    double magnitude = distance( vector, origin );
    
    if( magnitude == 0 )
        return;
    
    vector[0] = vector[0] / magnitude;
    vector[1] = vector[1] / magnitude;   
}

void truncate( double steering_direction[], double max_force )
{
    double origin[2] = { 0, 0 };
    double magnitude = distance( steering_direction, origin );
    
    if( magnitude > max_force ){
        normalize( steering_direction );
        steering_direction[0] = steering_direction[0] * max_force;
        steering_direction[1] = steering_direction[1] * max_force;
    }
}

// Returns the amount of fuel consumed
double nextPosition( uav * plane , double steering_direction[] )
{
    
    double steering_force[2] = { steering_direction[0],
        steering_direction[1] };
        
    double initial_velocity[2] = { plane->velocity[0], plane->velocity[1]};
        
    truncate ( steering_force, plane->max_force );   
    
    double acceleration[2] = { steering_force[0] / plane->mass,
    steering_force[1] / plane->mass };
    plane->velocity[0] += acceleration[0];
    plane->velocity[1] += acceleration[1];
    
    truncate ( plane->velocity, plane->max_speed );
    
    
    
    //double vel = lengthVector( plane->velocity );
    //if( vel < 0.1 * plane->max_speed )
    //    printf("Too slow...\n");
    plane->position[0] = plane->position[0] + plane->velocity[0];
    plane->position[1] = plane->position[1] + plane->velocity[1];
    
    double vel_change = distance( initial_velocity, plane->velocity );
    double fuel_consumed = plane->mass * vel_change * FUEL_CONSUMPTION;
    
    return fuel_consumed;
}

/* Obtains a new orientation for the plane in radians
 * after a call to nextPosition().
 */
void rotate( uav * plane )
{
    
    double vel[2] = { plane->velocity[0], plane->velocity[1] };
    
    plane->orientation = atan2( vel[1], vel[0] ) - atan2( 1, 0 );
}

void rotateVector( double vec[] , double theta )
{
    
    vec[0] = cos( theta ) * vec[0] - sin( theta )* vec[1]; 

    vec[1] = cos( theta ) * vec[1] + sin( theta )* vec[0];

}

double lengthVector( double vec[] )
{
    return sqrt( pow( vec[0], 2) + pow( vec[1], 2) );
}

// Angle of B relative to A
double angleTwoVectors( double vecA[], double vecB[2] )
{
    normalize( vecA );
    normalize( vecB );
   // double val = dotProduct2D( vecA, vecB );
    
    double result = 0;
    //result = acos( val );
    result = atan2( vecB[1], vecB[0] ) - atan2( vecA[1], vecA[0] );
    return result;

}

double dotProduct2D( double vecA[], double vecB[] )
{
    return vecA[0] * vecB[0] + vecA[1] * vecB[1];
}

void seek( uav * plane, double seekPos[], double steering[] ){
    
    // Adapted from Criag Reynold's paper "Steering Behaviors"
    double desired_velocity[2] = { seekPos[0] - plane->position[0], 
                                   seekPos[1] - plane->position[1]};
                                   
    normalize ( desired_velocity );
    desired_velocity[0] = desired_velocity[0] * plane->max_speed;
    desired_velocity[1] = desired_velocity[1] * plane->max_speed;
    
    steering[0] = steering[0] + desired_velocity[0] - plane->velocity[0];
    steering[1] = steering[1] + desired_velocity[1] - plane->velocity[1];

}

void flee( uav * plane, double fleePos[], double steering[] ){
    
    double desired_velocity[2] = { plane->position[0] - fleePos[0], 
                                   plane->position[1] - fleePos[1] };
                                   
    normalize ( desired_velocity );
    desired_velocity[0] = desired_velocity[0] * plane->max_speed;
    desired_velocity[1] = desired_velocity[1] * plane->max_speed;
    
    steering[0] = steering[0] + desired_velocity[0] - plane->velocity[0];
    steering[1] = steering[1] + desired_velocity[1] - plane->velocity[1];
}

void arrival( uav * plane, double seekPos[] , double steering[] )
{
    // Adapted from Criag Reynold's paper "Steering Behaviors"
    double target_offset[2] = { seekPos[0] - plane->position[0], 
                                seekPos[1] - plane->position[1] };
                                
    double distance = lengthVector( target_offset );
    double slowing_distance = 3;
    double ramped_speed = plane->max_speed * ( distance / slowing_distance );
    if( plane->max_speed < ramped_speed )
        ramped_speed = plane->max_speed;
    
    double desired_vel[2] = { (ramped_speed / distance) * target_offset[0], 
                              (ramped_speed / distance) * target_offset[1] };
    
    steering[0] = steering[0] + desired_vel[0] - plane->velocity[0];
    steering[1] = steering[1] + desired_vel[1] - plane->velocity[1];
    
    
}

void orbit( uav * plane, double orbitPos[], double radius, double steering[] ){
    
    
    double plane_to_orbit[2] = { orbitPos[0] - plane->position[0],
                                 orbitPos[1] - plane->position[1] };
                                
    double angle_of_approach = angleTwoVectors( plane_to_orbit, plane->velocity );
    //double current_velocity = lengthVector( plane->velocity );
    double current_distance = lengthVector( plane_to_orbit );
    double future_pos[2] = { plane->position[0] + plane->velocity[0],
                             plane->position[1] + plane->velocity[1] };
    double projection_vector[2] = { plane->position[0] - orbitPos[0], 
                                   plane->position[1] - orbitPos[1] };
    
    
    double new_pos[2] = { 0, 0};
    double new_distance = 0;
    double plane_to_new[2] = { plane_to_orbit[0], plane_to_orbit[1] };
    
    normalize( plane_to_new );
    plane_to_new[0] = plane_to_new[0] * plane->max_speed;
    plane_to_new[1] = plane_to_new[1] * plane->max_speed;
    
    double one_third_pi = 1.047197551;
    
    if( current_distance < radius ){
        
        if( angle_of_approach < 0 )
            rotateVector( plane_to_new, -2 * one_third_pi );
        else
            rotateVector( plane_to_new, 2 * one_third_pi );

        new_pos[0] = plane->position[0] + plane_to_new[0];
        new_pos[1] = plane->position[1] + plane_to_new[1];
        
        new_distance = distance( orbitPos, new_pos );
        
        if( new_distance > radius ){
            projection_vector[0] = future_pos[0] - orbitPos[0];
            projection_vector[1] = future_pos[1] - orbitPos[1];
            normalize( projection_vector );
        
            projection_vector[0] = projection_vector[0] * radius;
            projection_vector[1] = projection_vector[1] * radius; 
        
            new_pos[0] = orbitPos[0] + projection_vector[0];
            new_pos[1] = orbitPos[1] + projection_vector[1];
        }
    }
    else{
        if( angle_of_approach < 0 )
            rotateVector( plane_to_new, -one_third_pi );
        else
            rotateVector( plane_to_new, one_third_pi );

        new_pos[0] = plane->position[0] + plane_to_new[0];
        new_pos[1] = plane->position[1] + plane_to_new[1];
        
        new_distance = distance( orbitPos, new_pos );
        
        if( new_distance < radius ){
            projection_vector[0] = future_pos[0] - orbitPos[0];
            projection_vector[1] = future_pos[1] - orbitPos[1];
            normalize( projection_vector );
        
            projection_vector[0] = projection_vector[0] * radius;
            projection_vector[1] = projection_vector[1] * radius; 
        
            new_pos[0] = orbitPos[0] + projection_vector[0];
            new_pos[1] = orbitPos[1] + projection_vector[1];
        }
    }

    //if( new_distance < 0.5 )
   //         printf("Slow plane at %f,%f with velocity of %f", plane->position[0], plane->position[1], new_distance );
    

   /* This section works, sort of...
   double future_pos[2] = { plane->position[0] + plane->velocity[0],
                             plane->position[1] + plane->velocity[1] };
   double new_pos[2] = { 0, 0};
   double projection_vector[2] = { plane->position[0] - orbitPos[0], 
                                   plane->position[1] - orbitPos[1] };
      
        projection_vector[0] = future_pos[0] - orbitPos[0];
        projection_vector[1] = future_pos[1] - orbitPos[1];
        normalize( projection_vector );
        
        projection_vector[0] = projection_vector[0] * radius;
        projection_vector[1] = projection_vector[1] * radius; 
        
        new_pos[0] = orbitPos[0] + projection_vector[0];
        new_pos[1] = orbitPos[1] + projection_vector[1];
   */
     
   seek( plane, new_pos, steering );
    
}

void offsetSeek( uav * plane, double seekPos[] , double offset, double steering[] )
{
    double seekDist = distance( plane->position, seekPos );
    
    if( seekDist < offset * 1.2 ){
        orbit( plane, seekPos, offset, steering );
        return;
    }
        
    double myPi = 3.141592654;
    // From seekPos to the UAV
    double straight_line[2] = { plane->position[0] - seekPos[0], 
                                plane->position[1] - seekPos[1]};
    normalize( straight_line );
    
    // runs from the seekPos to a point on the offset circle which is 
    // perpendicular to the straight line from the uav to the seekPos
    double right_line[2] = { straight_line[0], straight_line[1] };        
    //normalize( right_line );
    rotateVector( right_line, ( 0.5 * myPi ) );
    
    
    right_line[0] = right_line[0] * offset;
    right_line[1] = right_line[1] * offset;
    
    
    
    // The end point of right_line
    double right_pos[2] = {right_line[0] + seekPos[0], 
                          right_line[1] + seekPos[1]};    
                          
    
    double future_pos[2] = { plane->position[0] + plane->velocity[0], 
                             plane->position[1] + plane->velocity[1] };
                             
    double right_dist = distance( future_pos, right_pos );
    // runs directly from the uav to the right_pos                      
    //double right_vec[2] = { right_pos[0] - plane->position[0], 
    //                        right_pos[1] - plane->position[1]};
                            
    //double right_angle = 0;
   // right_angle = angleTwoVectors( plane->velocity, right_vec );
   // right_angle = fabs( right_angle );
    
    double left_line[2] = { straight_line[0], straight_line[1] };        
    normalize( left_line );
    rotateVector( left_line, ( -0.5 * myPi ) );
    
    
    left_line[0] = left_line[0] * offset;
    left_line[1] = left_line[1] * offset;
    
    double left_pos[2] = {left_line[0] + seekPos[0], 
                          left_line[1] + seekPos[1]};
                          
                          
    double left_dist = distance( future_pos, left_pos );                          
    
    if( left_dist < right_dist ){
        seek( plane, left_pos, steering );
    }
            
    else{
        seek( plane, right_pos, steering );
    }
}

int separation( uav * plane , double steering[] ){
    
    int n = length( plane->group );
    
    if( n == 0 )    
        return 0;
  
    node * grp = plane->group; 
    double dist = 0;
    double temp_force[2] = { 0, 0 };
    int num_separation = 0;
    
    int i;
    for( i = 0; i < n; ++i ){

        uav * u = ( uav * )grp->data; 
        dist = distance( plane->position, u->position );
        
        if( dist != 0 && u->pPos[0] < u->cooperateVal ){
        
            temp_force[0] = plane->position[0] - u->position[0];
            temp_force[1] = plane->position[1] - u->position[1];

            normalize( temp_force );

            temp_force[0] = temp_force[0] / dist;
            temp_force[1] = temp_force[1] / dist;

            steering[0] = steering[0] + temp_force[0];
            steering[1] = steering[0] + temp_force[1];
            
            ++num_separation;
        }
        
        grp = grp->next;
    }
    
    return num_separation;
}

int cohesion( uav * plane, double steering[] ){
    
    int n = length( plane->group );
    
    if( n == 0 )
        return 0;
    
    node * grp = plane->group; 
    double average_position[2] = { 0, 0 };
    int num_cohesion = 0;
    
    int i;
    for( i = 0; i < n; ++i ){
        
        uav * u = ( uav * )grp->data;
        
        if( u->pPos[0] < u->cooperateVal ){
            average_position[0] = average_position[0] + u->position[0];
            average_position[1] = average_position[1] + u->position[1]; 
            ++num_cohesion;
        }
        grp = grp->next;
    }
    
    if( num_cohesion > 0 ){
        average_position[0] = average_position[0] / num_cohesion; 
        average_position[1] = average_position[1] / num_cohesion;
        steering[0] = steering[0] + ( average_position[0] - plane->position[0] );
        steering[1] = steering[1] + ( average_position[1] - plane->position[1] );
    }
   
    return num_cohesion;
}

int alignment( uav * plane, double steering[] ){
    
    int n = length( plane->group );
    
    if( n == 0 )
        return 0;
    
    node * grp = plane->group; 
    double average_velocity[2] = { 0, 0 };
    int num_alignment = 0;
    
    int i;
    for( i = 0; i < n; ++i ){
        
        uav * u = ( uav * )grp->data;
        
        if( u->pPos[0] < u->cooperateVal ){
            average_velocity[0] = average_velocity[0] + u->velocity[0];
            average_velocity[1] = average_velocity[1] + u->velocity[1];
            ++num_alignment;
        }
        grp = grp->next;
    }
    
    if( num_alignment > 0){
        average_velocity[0] = average_velocity[0] / num_alignment; 
        average_velocity[1] = average_velocity[1] / num_alignment;   
        steering[0] = steering[0] + ( average_velocity[0] - plane->velocity[0] );
        steering[1] = steering[1] + ( average_velocity[1] - plane->velocity[1] );
    }
    
    return num_alignment;
}

int flocking( uav * plane, double steering[] ){
    
    double sepSteer[2] = { 0, 0 };
    int num_separation = 0;
    num_separation = separation( plane, sepSteer );
    normalize( sepSteer );
    
    double coSteer[2] = { 0, 0 };
    
    cohesion( plane, coSteer );
    normalize( coSteer );
    
    double alignSteer[2] = { 0, 0 };
    
    alignment( plane, alignSteer );
    normalize( alignSteer );
    
    if( num_separation > 0 ){
        steering[0] = steering[0] + 1.0 * sepSteer[0] + 1.0 * coSteer[0] + 1.0 * alignSteer[0];
        steering[1] = steering[1] + 1.0 * sepSteer[1] + 1.0 * coSteer[1] + 1.0 *  alignSteer[1];
    }
    
    return num_separation;
}

// Returns 0 if no leader was found in the uav's group
int followTheLeader( uav * plane, double steering[] ){
    
    // Look for a leader 
    int n = length( plane->group );
    
    if( n == 0 )
        return 0;
    
    node * grp = plane->group; 
    uav * u = 0;
    uav * leader = 0;
    //uav * closest = 0;
    double distance_leader = 0;
    //double closest_leader_dist = 0;
    //double distance_closest = 0;
    double temp_distance = 0;
    
    int i;
    for( i = 0; i < n; ++i ){
        
        u = ( uav * )grp->data;
        
        if( u->leader != 0 && u->pPos[0] < u->cooperateVal ){
            if( leader == 0 ){
                leader = ( uav * )grp->data;
                distance_leader = distance( plane->position, leader->position );
            }
            else{
                temp_distance = distance( plane->position, u->position );
                if( temp_distance < distance_leader ){
                    leader = ( uav * )grp->data;
                    distance_leader = temp_distance;
                }
            }
        }
        
        grp = grp->next;
    }
    
    // No leader found
    if( leader == 0)
        return 0;

    double dist = 0;
    double leader_steering[2] = { 0, 0 };
    double temp_force[2] = { 0, 0 };
    double ldr_move_pos[2] = { leader->position[0] - ( 0.5 * leader->velocity[0] ), 
        leader->position[1] - ( 0.5 * leader->velocity[1] ) };

    arrival( plane, ldr_move_pos, leader_steering );

    double separation_steering[2] = { 0, 0};
    grp = plane->group;
    for( i = 0; i < n; ++i ){
        
        u = ( uav * )grp->data;
        
        if( u->pPos[0] < u->cooperateVal ){

            dist = distance( plane->position, u->position );
        
            if( dist != 0 ){
        
                temp_force[0] = plane->position[0] - u->position[0];
                temp_force[1] = plane->position[1] - u->position[1];

                normalize( temp_force );

                temp_force[0] = temp_force[0] / dist;
                temp_force[1] = temp_force[1] / dist;

                separation_steering[0] = separation_steering[0] + temp_force[0];
                separation_steering[1] = separation_steering[0] + temp_force[1];
        
            }
        }
        
        grp = grp->next;
    }

    steering[0] = leader_steering[0] + 0.1 * ( leader->velocity[0] - plane->velocity[0] ) + 0.01 * separation_steering[0];
    steering[1] = leader_steering[1] + 0.1 * ( leader->velocity[1] - plane->velocity[1] ) + 0.01 * separation_steering[1];

        
    return 1;
}

