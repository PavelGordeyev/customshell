/********************************************************************* 
** CS344 - Operating Systems
** Program name: Dynamic Array structure
** Author: Pavel Gordeyev
** Date: 5/15/20
** Description:  Dynamic array implementation file. Creates the dynamic 
**				 array structure and functions.  Using prior
**				 implementation created in CS261.
*********************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "dynArrChar.h"


/********************************************************************* 
**        Name: initDynArrChar
**    Synopsis: Initializes dynamic array.
** Description: Initializes and allocates data array for a dynamic
**				array.
*********************************************************************/
void initDynArrChar(struct DynArrChar *arr, int cap){

	assert(cap > 0);
	assert(arr!= 0);

	// Allocate memory for data
	arr->data = (TYPEC *) malloc(sizeof(TYPEC) * cap);
	assert(arr->data != 0);

	// Initialize array
	for(int i = 0; i < cap; i++){
		arr->data[i] = 0;
	}

	// Initialize size & capacity
	arr->size = 0;
	arr->capacity = cap;
}

/********************************************************************* 
**        Name: newDynArrChar
**    Synopsis: Creates a new dynamic array.
** Description: Creates a new dynamic array with a specified capacity.
*********************************************************************/
struct DynArrChar* newDynArrChar(int cap){
	
	assert(cap > 0);

	// Allocate memory for the dynamic array
	struct DynArrChar *arr = (struct DynArrChar *)malloc(sizeof(struct DynArrChar));
	assert(arr != 0);

	// Initialize the dynamic array with the specified capacity
	initDynArrChar(arr,cap);

	return arr;
}


/********************************************************************* 
**        Name: _DynArrCharSetCapacity
**    Synopsis: Resets capacity for dynamic array.
** Description: Resets capacity for the dynamic array to the newly
**				specified capacity.
*********************************************************************/
void _DynArrCharSetCapacity(struct DynArrChar *arr, int newCap){

    TYPEC *temp = (TYPEC *) malloc(sizeof(TYPEC) * newCap);
    assert(temp != 0);
    
    int size = sizeDynArrChar(arr);
    
    // Copy data into the new array
    for(int i=0;i<sizeDynArrChar(arr);i++){
        temp[i] = arr->data[i];
    }
    
    // Free the old data array
    freeDynArrChar(arr);

    // Reset size, data & capacity
    arr->size = size;
    arr->data = temp;
    arr->capacity = newCap;
}


/********************************************************************* 
**        Name: addDynArrChar
**    Synopsis: Adds to the end of the dynamic array.
** Description: Adds specified value to the end of the dynamic array.
*********************************************************************/
void addDynArrChar(struct DynArrChar *arr, TYPEC val){

	// Resize dynamic array if needed
    if(sizeDynArrChar(arr) == arr->capacity){
        _DynArrCharSetCapacity(arr,arr->capacity * 2);
    }
    
    arr->data[sizeDynArrChar(arr)] = val;
    
    arr->size++;

}

/********************************************************************* 
**        Name: putDynArrChar
**    Synopsis: Inserts a value into the specified position in the
**				dynamic array.
** Description: Insert value at the specified location in the dynamic
**				array, if it is a valid index.
*********************************************************************/
void putDynArrChar(struct DynArrChar *arr, int idx, TYPEC val)
{

	// Checks for a valid index
    assert(arr != 0 && idx >= 0 && idx < sizeDynArrChar(arr));
    
    arr->data[idx] = val;
    
}

/********************************************************************* 
**        Name: removeAtDynArrChar
**    Synopsis: Removes value from an index in the dynamic array.
** Description: Removes value from a specified index in the dynamic
**				array. Shifts elements over by one to fill the gap.
*********************************************************************/
void removeAtDynArrChar(struct DynArrChar *arr, int idx){

	// Check for a valid index
    assert(arr != 0 && idx >= 0 && idx < sizeDynArrChar(arr));
    
    for(int i = idx; i < sizeDynArrChar(arr) - 1;i++){
        putDynArrChar(arr,i,arr->data[i+1]);
    }
    
    arr->size--;
}

/********************************************************************* 
**        Name: sizeDynArrChar
**    Synopsis: Returns the size of the dynamic array.
** Description: Returns the size of the dynamic array.
*********************************************************************/
int sizeDynArrChar(struct DynArrChar *arr)
{
	return arr->size;
}

/********************************************************************* 
**        Name: freeDynArrChar
**    Synopsis: Frees memory allocated for dynamic array's data.
** Description: Frees memory allocated for the dynamic array data and
**				sets the size and capacity to 0.
*********************************************************************/
void freeDynArrChar(struct DynArrChar *arr){

	if(arr->data != 0){

		// Free space on the heap
		free(arr->data);
		arr->data = 0;
	}

	arr->size = 0;
	arr->capacity = 0;
}

/********************************************************************* 
**        Name: deleteDynArrChar
**    Synopsis: Frees memory allocated for dynamic array and it's data.
** Description: Frees memory allocated for dynamic array and it's data.
*********************************************************************/
void deleteDynArrChar(struct DynArrChar *arr)
{
	freeDynArrChar(arr);
	free(arr);
}