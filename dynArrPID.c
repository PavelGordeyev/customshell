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
#include "dynArrPID.h"


/********************************************************************* 
**        Name: initDynArrPID
**    Synopsis: Initializes dynamic array.
** Description: Initializes and allocates data array for a dynamic
**				array.
*********************************************************************/
void initDynArrPID(struct DynArrPID *arr, int cap){

	assert(cap > 0);
	assert(arr!= 0);

	// Allocate memory for data
	arr->data = (TYPE *) malloc(sizeof(TYPE) * cap);
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
**        Name: newDynArrPID
**    Synopsis: Creates a new dynamic array.
** Description: Creates a new dynamic array with a specified capacity.
*********************************************************************/
struct DynArrPID* newDynArrPID(int cap){
	
	assert(cap > 0);

	// Allocate memory for the dynamic array
	struct DynArrPID *arr = (struct DynArrPID *)malloc(sizeof(struct DynArrPID));
	assert(arr != 0);

	// Initialize the dynamic array with the specified capacity
	initDynArrPID(arr,cap);

	return arr;
}


/********************************************************************* 
**        Name: _DynArrPIDSetCapacity
**    Synopsis: Resets capacity for dynamic array.
** Description: Resets capacity for the dynamic array to the newly
**				specified capacity.
*********************************************************************/
void _DynArrPIDSetCapacity(struct DynArrPID *arr, int newCap){

    TYPE *temp = (TYPE *) malloc(sizeof(TYPE) * newCap);
    assert(temp != 0);
    
    int size = sizeDynArrPID(arr);
    
    // Copy data into the new array
    for(int i=0;i<sizeDynArrPID(arr);i++){
        temp[i] = arr->data[i];
    }
    
    // Free the old data array
    freeDynArrPID(arr);

    // Reset size, data & capacity
    arr->size = size;
    arr->data = temp;
    arr->capacity = newCap;
}


/********************************************************************* 
**        Name: addDynArrPID
**    Synopsis: Adds to the end of the dynamic array.
** Description: Adds specified value to the end of the dynamic array.
*********************************************************************/
void addDynArrPID(struct DynArrPID *arr, TYPE val){
    
    // Resize dynamic array if needed
    if(sizeDynArrPID(arr) == arr->capacity){
        _DynArrPIDSetCapacity(arr,arr->capacity * 2);
    }
    
    arr->data[sizeDynArrPID(arr)] = val;
    
    arr->size++;

}

/********************************************************************* 
**        Name: putDynArrPID
**    Synopsis: Inserts a value into the specified position in the
**				dynamic array.
** Description: Insert value at the specified location in the dynamic
**				array, if it is a valid index.
*********************************************************************/
void putDynArrPID(struct DynArrPID *arr, int idx, TYPE val)
{

	// Checks for a valid index
    assert(arr != 0 && idx >= 0 && idx < sizeDynArrPID(arr));
    
    arr->data[idx] = val;
    
}

/********************************************************************* 
**        Name: removeAtDynArrPID
**    Synopsis: Removes value from an index in the dynamic array.
** Description: Removes value from a specified index in the dynamic
**				array. Shifts elements over by one to fill the gap.
*********************************************************************/
void removeAtDynArrPID(struct DynArrPID *arr, int idx){

	// Check for a valid index
    assert(arr != 0 && idx >= 0 && idx < sizeDynArrPID(arr));
    
    for(int i = idx; i < sizeDynArrPID(arr) - 1;i++){
        putDynArrPID(arr,i,arr->data[i+1]);
    }
    
    arr->size--;
}

/********************************************************************* 
**        Name: sizeDynArrPID
**    Synopsis: Returns the size of the dynamic array.
** Description: Returns the size of the dynamic array.
*********************************************************************/
int sizeDynArrPID(struct DynArrPID *arr)
{
	return arr->size;
}

/********************************************************************* 
**        Name: freeDynArrPID
**    Synopsis: Frees memory allocated for dynamic array's data.
** Description: Frees memory allocated for the dynamic array data and
**				sets the size and capacity to 0.
*********************************************************************/
void freeDynArrPID(struct DynArrPID *arr){

	if(arr->data != 0){

		// Free space on the heap
		free(arr->data);
		arr->data = 0;
	}

	arr->size = 0;
	arr->capacity = 0;
}

/********************************************************************* 
**        Name: deleteDynArrPID
**    Synopsis: Frees memory allocated for dynamic array and it's data.
** Description: Frees memory allocated for dynamic array and it's data.
*********************************************************************/
void deleteDynArrPID(struct DynArrPID *arr)
{
	freeDynArrPID(arr);
	free(arr);
}