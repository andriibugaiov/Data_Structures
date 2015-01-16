
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GRBTree.h"

typedef struct _OrderedPair{
	int index;
	int value;
} OrderedPair;

// designated initializer/constructor
void OPNew(OrderedPair *anOrderedPair, int anIndex, int aValue);

//accessors
//setters
void OPSetIndex(OrderedPair *anOrderedPair, int anIndex);
void OPSetValue(OrderedPair *anOrderedPair, int aValue);

// getters
int OPGetIndex(OrderedPair *anOrderedPair);
int OPGetValue(OrderedPair *anOrderedPair);

void OPNew(OrderedPair *anOrderedPair, int anIndex, int aValue)
{
	OPSetIndex(anOrderedPair, anIndex);
	OPSetValue(anOrderedPair, aValue);
}

void OPSetIndex(OrderedPair *anOrderedPair, int anIndex)
{
	anOrderedPair -> index = anIndex;
}

void OPSetValue(OrderedPair *anOrderedPair, int aValue)
{
	anOrderedPair -> value = aValue;
}

// getters
int OPGetIndex(OrderedPair *anOrderedPair)
{
	return anOrderedPair -> index;
}

int OPGetValue(OrderedPair *anOrderedPair)
{
	return anOrderedPair -> value;
}

/* **************************************************************************** */

ComparisonResult integersComparePredicate(int anIntLeft, int anIntRight);
ComparisonResult integersComparePredicate(int anIntLeft, int anIntRight)
{
	int intLeft = anIntLeft;
	int intRight = anIntRight;
	return intLeft == intRight ? Equal : intLeft < intRight ? Less : More;
}

ComparisonResult valueComparePredicate(void *anOPLeft, void *anOPRight);
ComparisonResult valueComparePredicate(void *anOPLeft, void *anOPRight)
{
	OrderedPair *orderedPairLeft = (OrderedPair *)anOPLeft;
	OrderedPair *orderedPairRight = (OrderedPair *)anOPRight;
	int valueLeft = OPGetValue(orderedPairLeft);
	int valueRight = OPGetValue(orderedPairRight);
	return integersComparePredicate(valueLeft, valueRight);
}

//ComparisonResult indexComparePredicate(void *anOPLeft, void *anOPRight);
//ComparisonResult indexComparePredicate(void *anOPLeft, void *anOPRight)
//{
//	OrderedPair *orderedPairLeft = (OrderedPair *)anOPLeft;
//	OrderedPair *orderedPairRight = (OrderedPair *)anOPRight;
//	int indexLeft = OPGetIndex(orderedPairLeft);
//	int indexRight = OPGetIndex(orderedPairRight);
//	return integersComparePredicate(indexLeft, indexRight);
//}

void defaultDescriptor(void *anOrderedPair);
void defaultDescriptor(void *anOrderedPair)
{
	OrderedPair *orderedPair = (OrderedPair *)anOrderedPair;
	printf("Index - %d; Value - %d\n",  OPGetIndex(orderedPair), OPGetValue(orderedPair));
}

void startTest();
void startTest()
{
	srand((unsigned)time(NULL));
//	int array[] = {16, 282475, 1622650, 984943, 1144108, 470211, 101027, 1457850, 1458777, 2007237};
	
	int arraySize = 100;
	int array[arraySize];
	for (int i = 0; i < arraySize; ++i)
	{
		array[i] = rand() % 10000;
//		printf(" %d,", array[i]);
	}
	
	GRBTree orderedValuesTree;
	GRBTreeNew(&orderedValuesTree, sizeof(OrderedPair), valueComparePredicate, NULL, NULL);

	unsigned long size = sizeof(array)/sizeof(array[0]);
	
	for (int orderedPairIndex = 0; orderedPairIndex < size; ++orderedPairIndex)
	{
		int value = array[orderedPairIndex];

		OrderedPair orderedPair;
		OPNew(&orderedPair, orderedPairIndex, value);
		/*MyBool isReplaced = */GRBTreeDataAdd(&orderedValuesTree, &orderedPair);
		
//		printf("\n");
//		GRBTreeTestStructure(&orderedValuesTree);
	}
	
	printf("\n");
	GRBTreeDescription(&orderedValuesTree, defaultDescriptor);
	
	for (int orderedPairIndex = 0; orderedPairIndex < size; ++orderedPairIndex)
	{
		int value = array[size - 1 - orderedPairIndex];
		
		OrderedPair orderedPair;
		OPNew(&orderedPair, orderedPairIndex, value);
		GRBTreeDataRemove(&orderedValuesTree, &orderedPair);
		
//		printf("\n");
//		GRBTreeTestStructure(&orderedValuesTree);
	}
	
	printf("\n");
	GRBTreeDescription(&orderedValuesTree, defaultDescriptor);
	
	GRBTreeDispose(&orderedValuesTree);
	printf("\n");
}

int main(int argc, const char * argv[]) {
	startTest();
	return 0;
}

