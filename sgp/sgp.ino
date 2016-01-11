// Simple graph processor that executes a graph in the Get,Process,Set&Transition style.
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define NUM_PROC_NODES 7
#define NUM_DATA_NODES 12

// Value union
typedef union Value {
  int i;
  float f;
  char str[32];
} dn_value;

// Data Node struct
typedef struct DN
{
  dn_value key;
  dn_value value;
} dn;
// pointer to a nodes process function
typedef node_data (*node_proc)(node_data nd);

// all process nodes have a pointer to a function or null
int p[3][4] = {  
   {0, 1, 2, 3} ,   /*  initializers for row indexed by 0 */
   {4, 5, 6, 7} ,   /*  initializers for row indexed by 1 */
   {8, 9, 10, 11}   /*  initializers for row indexed by 2 */
};

