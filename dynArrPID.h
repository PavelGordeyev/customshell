/********************************************************************* 
** CS344 - Operating Systems
** Program name: Dynamic Array structure
** Author: Pavel Gordeyev
** Date: 5/15/20
** Description:  Dynamic array specification file. Creates the dynamic 
**				 array structure and functions.  Using prior
**				 implementation created in CS261.
*********************************************************************/
#ifndef DYN_ARR_PID
#define DYN_ARR_PID

#include <sys/types.h>

#ifndef __TYPE
#define __TYPE
# define TYPE      pid_t
# define TYPE_SIZE sizeof(pid_t)
# endif

struct DynArrPID{
	TYPE *data;		
	int size;		
	int capacity;
};

// Initialize functions
void initDynArrPID(struct DynArrPID*, int);	
struct DynArrPID *newDynArrPID(int);

// Memory freeing functions
void freeDynArrPID(struct DynArrPID*);
void deleteDynArrPID(struct DynArrPID*);

// Helper functions
int sizeDynArrPID(struct DynArrPID*);
void addDynArrPID(struct DynArrPID*, TYPE);
TYPE getDynArrPID(struct DynArrPID*, int);
void putDynArrPID(struct DynArrPID*, int, TYPE);
void removeAtDynArrPID(struct DynArrPID*, int);


#endif
