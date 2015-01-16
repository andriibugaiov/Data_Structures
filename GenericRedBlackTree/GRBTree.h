
#include "GNode.h"

/* REMARKS:
 - the tree owns all it's nodes (that is the interface which operates on the tree
 is responsible for allocating the memory for each node and initializing it, as
 well as for freeing the memory up after the particular node as it is no longer needed);
 
 - the user passes the data (integral types (int, double, etc.) or custom types
 (key - value pair, etc.)) which are immediately copied and stored under the
 data component/property of the newly created and initialized node;
 
 - the tree knows nothing about the data its nodes store;
 */

// Private - Begin
struct _GRBTree{
	GNode *root;
	int size;
	
	// stores the number of bits needed to accomodate the data
	int nodeDataSize;
	// copies the internal data of the structure stored
	// under node's 'data' component
	void (* nodeDataCopier)(void *aDestinationData, void *aSourceData);
	// serves as a predicate for the data comparison
	ComparisonResult (* nodeDataComparePredicate)(void *, void *);
	// frees up the internal data of the structure stored
	// under node's 'data' component
	void (* nodeDataDisposer)(void *);
};
// Private - End

typedef struct _GRBTree GRBTree;

// Public - Begin
//accessors
//getters
int GRBTreeGetSize(GRBTree *aTree);

MyBool GRBTreeIsEmpty(GRBTree *aTree);

// designated initializer/constructor
void GRBTreeNew(GRBTree *aTree, int aNodeDataSize, ComparisonResult (*aNodeDataComparePredicate)(void *, void *), void (* aNodeDataDisposer)(void *), void (* aNodeDataCopier)(void *aDestinationData, void *aSourceData));
// designated disposer/destructor
void GRBTreeDispose(GRBTree *aTree);

// searches for a node wich has its data component equal to 'aData' and returns a pointer to that data, if a node is found, otherwise returns NULL
void *GRBTreeDataSearch(GRBTree *aTree, void *aData);

// addes a node to 'aTree' and stores 'aData' under its data components, if the node with its data component equal to 'aData' is located, replaces it with a new one
MyBool GRBTreeDataAdd(GRBTree *aTree, void *aData);

// removes a node from 'aTree'
void GRBTreeDataRemove(GRBTree *aTree, void *aData);

// performs full test of the tree's structure
void GRBTreeTestStructure(GRBTree *aTree);

void GRBTreeDescription(GRBTree *aTree, void (* aNodeDataDescriptor)(void *));
// Public - End
