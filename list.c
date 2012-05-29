
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
 * File:   list.c
 * Author: bradley
 *
 * Created on September 15, 2007, 10:04 PM
 */

#include "simtypes.h"

// ************************************************************************** 
// Function name: push
// Purpose: pushes a node onto the front of the lsit
// Explicitly returns: 0  
// Returns using pass by reference: the resulting list
// **************************************************************************
void addFront(node **n, void * data)
{
	node *temp;
	temp = malloc( sizeof(node) );
	temp->data = data;
	temp->next = *n;
	*n = temp;
}

// ************************************************************************** 
// Function name: append
// Purpose: appends a node to the end of a list
// Explicitly returns: 0  
// Returns using pass by reference: the resulting list
// **************************************************************************
void addBack(node **n, void * data)
{
	node *temp = NULL;
	if(*n==NULL)
		addFront(n, data);
	else
	{
		temp = *n;
		while(temp->next!=NULL)
			temp = temp->next;
		addFront(&(temp->next), data);
	}	
}

// ************************************************************************** 
// Function name: length
// Purpose: Gives the length of a singly linked list
// Explicitly returns: the length 
// Returns using pass by reference: Nothing
// **************************************************************************
int length( const node *n )
{
	int l = 0;
	while(n!=NULL)
	{
		n = n->next;
		l++;
	}	
	return l;
}

// ************************************************************************** 
// Function name: pop
// Purpose: Removes the head node from a singly linked list
// Explicitly returns: A pointer to the stored data  
// Returns using pass by reference: the resulting list
// **************************************************************************
void * removeFront(node **n)
{
	void * d = NULL;
 	node * temp = NULL;
	node * temp2 = NULL;
	
	assert(n!=NULL);
 	temp = *n;
	d = temp->data;
	temp2 = temp->next;
	free(temp);
	*n = temp2;
	
	return d;
}

void copy( node **cpy, node * list ){
    
    int len = length( list );
    
    int i;
    for( i = 0; i < len; ++i )
        addFront( cpy, list->data );
}

void clear( node *list ){
    
    int n = length( list );
    
    int i;
    for( i = 0; i < n; ++i)
        removeFront( &list );
    
    list = NULL;
}

int hasData( const node * list, const void * d ){
    
    
    int n = length( list );
    
    void * d2;
    int i;
    for( i = 0; i < n; ++i ){
        
        d2 = list->data;
        if( d2 == d )
            return 1;
        
        list = list->next;
    }
    
    return 0;
}

void mergeUnique( node ** list1, const node * list2 ){
    
    int n = length( list2 );
    
    int i;
    for( i = 0; i < n; ++i ){
        
        if( 0 == hasData( (*list1), list2->data ) )
            addFront( list1, list2->data );
        
        list2 = list2->next;
    }
    
}


