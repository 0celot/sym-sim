
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
 * File:   display.c
 * Author: bradley
 *
 * Created on September 20, 2007, 11:39 AM
 */

//#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "simtypes.h"
#include <math.h>

void display( target targs[], int tSize, uav planes[], int pSize ){
    
   glClear( GL_COLOR_BUFFER_BIT );
   
   
   double x, y;
   
    // Draw base
    glColor3f( 0.0, 0.8, 0.1 );
    glLoadIdentity();
    glTranslatef( planes[0].basePos[0] , planes[0].basePos[1], 0.0 );

    glBegin( GL_QUADS );
        glVertex2f( -0.5, -0.5 );
        glVertex2f( 0.5, -0.5 );
        glVertex2f( 0.5, 0.5 );
        glVertex2f( -0.5, 0.5 );
    glEnd( );
    
   // Draw targets
    glColor3f( 0.8, 0.0, 0.0 );
   int i;
   for( i = 0; i < tSize; ++i ){
       
       if( targs[i].toughness > 0 ){
           x = targs[i].position[0];
           y = targs[i].position[1];

           glLoadIdentity();
           glTranslatef( x, y, 0.0 );

           glBegin( GL_QUADS );
                glVertex2f( 0, 0 );
                glVertex2f( 0.5, 0);
                glVertex2f( 0.5, 0.5 );
                glVertex2f( 0, 0.5 );
           glEnd( );
       }
      // else
         //  printf("toughness < 0\n");
   }
   
   // Draw UAVs
   //double heading_x, heading_y, angle;
   glColor3f( 0.0, 0.3, 0.9 );
   for( i = 0; i < pSize; ++i ){
       
       if( planes[i].position[0] == 50 && 
               planes[i].position[1] == 50 ){
           //printf("launching...\n");
       }
       if( planes[i].state == UAV_STATE_ACTIVE ){
       
           x = planes[i].position[0];
           y = planes[i].position[0];
           glLoadIdentity();
           glTranslatef( planes[i].position[0], planes[i].position[1], 0.0 );
           glRotatef( ( planes[i].orientation ) * 57.2957795, 0, 0, 1.0);

           glBegin( GL_TRIANGLES );
                glVertex2f( -0.3125, -0.625 );
                glVertex2f( 0, 0.625 );
                glVertex2f( 0.3125, -0.625 );
           glEnd( );
        
       }
       
   }
   
   SDL_GL_SwapBuffers();
   
   
}


void initDisplay( int width, int height ){
    
   // Create our window and initialize opengl...
   //SDL_Surface* buffer = init_sdl(width*8, height*8);
   init_sdl(width*8, height*8);
   init_opengl(width, height );
}

  


// Initialize opengl...
void init_opengl(int width, int height)
{
   glViewport(0, 0, width*8, height*8);
   glMatrixMode(GL_PROJECTION);
   gluOrtho2D(0, height, 0, width );

   glMatrixMode(GL_MODELVIEW);
   glClearColor( 1.0, 1.0, 1.0, 0.0 );
}

// Initialize an SDL_Surface as the window surface
// Most of this should explain itself...
SDL_Surface* init_sdl(int width, int height)
{
   SDL_Surface* surface;
   if(SDL_Init(SDL_INIT_VIDEO)< 0)
   {
      return NULL;
   }
   const SDL_VideoInfo* info = SDL_GetVideoInfo();
   if(!info)
   {
      return NULL;
   }
   int flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;
	if (info->hw_available)
   {
      flags |= SDL_HWSURFACE;
   }
	else
   {
      flags |= SDL_SWSURFACE;
   }
   surface=SDL_SetVideoMode(width, height, info->vfmt->BitsPerPixel, flags);
   if(surface==0)
   {
      return NULL;
   }
   return surface;
}



