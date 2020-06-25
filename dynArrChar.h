/********************************************************************* 
** CS344 - Operating Systems
** Program name: Dynamic Array structure
** Author: Pavel Gordeyev
** Date: 5/15/20
** Description:  Dynamic array specification file. Creates the dynamic 
**				 array structure and functions.  Using prior
**				 implementation created in CS261.
*********************************************************************/
#ifndef DYN_ARR_CHAR
#define DYN_ARR_CHAR

#ifndef __TYPEC
#define __TYPEC
# define TYPEC      char*
# define TYPEC_SIZE sizeof(char*)
# endif

struct DynArrChar{
	TYPEC *data;		
	int size;		
	int capacity;
};

// Initialize functions
void initDynArrChar(struct DynArrChar*, int);	
struct DynArrChar *newDynArrChar(int);

// Memory freeing functions
void freeDynArrChar(struct DynArrChar*);
void deleteDynArrChar(struct DynArrChar*);

// Helper functions
int sizeDynArrChar(struct DynArrChar*);
void addDynArrChar(struct DynArrChar*, TYPEC);
TYPEC getDynArrChar(struct DynArrChar*, int);
void putDynArrChar(struct DynArrChar*, int, TYPEC);
void removeAtDynArrChar(struct DynArrChar*, int);


#endif
