
#include "assert.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "GMaxHeapTree.h"

int GMaxHeapTreeParentNodeForNodeAtIndex(GMaxHeapTree *aTree, int aNodeIndex);
int GMaxHeapTreeLeftChildForNodeAtIndex(GMaxHeapTree *aTree, int aNodeIndex);
int GMaxHeapTreeRightChildForNodeAtIndex(GMaxHeapTree *aTree, int aNodeIndex);

#pragma mark -

void GMaxHeapTreePercolateUp(GMaxHeapTree *aTree, int aNodeIndex, void *aData);
void GMaxHeapTreePercolateDown(GMaxHeapTree *aTree, int aNodeIndex, void *aData);

#pragma mark -

GVector *GMaxHeapTreeGetVector(GMaxHeapTree *aTree)
{
	return aTree -> vector;
}

void GMaxHeapTreeSetVector(GMaxHeapTree *aTree, GVector *aVector)
{
	aTree -> vector = aVector;
}

int GMaxHeapTreeGetSize(GMaxHeapTree *aTree)
{
	return GVectorGetLength(GMaxHeapTreeGetVector(aTree));
}

void GMaxHeapTreeNew(GMaxHeapTree *aTree, int anDataSize, void (*anDataCopier)(void *aDestination, void *aSource), void (*anDataDisposer)(void *), ComparisonResult (*aComparePredicate)(void *leftOperand, void *rightOperand))
{
	aTree -> comparePredicate = aComparePredicate;
	
	GVector *vector = malloc(sizeof(GVector));
	assert(NULL != vector);
	
	GMaxHeapTreeSetVector(aTree, vector);
	GVectorNew(GMaxHeapTreeGetVector(aTree), anDataSize, anDataCopier, anDataDisposer);
	
	void *hole = calloc(1, anDataSize);
	assert(NULL != hole);
	GVectorElementPushBack(GMaxHeapTreeGetVector(aTree), hole);
	free(hole);
	hole = NULL;
}

void GMaxHeapTreeDispose(GMaxHeapTree *aTree)
{
	aTree -> comparePredicate = NULL;
	
	GVectorDispose(GMaxHeapTreeGetVector(aTree));
	free(GMaxHeapTreeGetVector(aTree));
	GMaxHeapTreeSetVector(aTree, NULL);
}

#pragma mark -

void GMaxHeapTreeDataAdd(GMaxHeapTree *aTree, void *aData)
{
	if (NULL == aData) return;

	void *hole = calloc(1, GMaxHeapTreeGetSize(aTree));
	assert(NULL != hole);
	
	GVectorElementPushBack(GMaxHeapTreeGetVector(aTree), hole);
	
	free(hole);
	hole = NULL;

	GMaxHeapTreePercolateUp(aTree, GMaxHeapTreeGetSize(aTree) - 1, aData);
}

void GMaxHeapTreeDataMaxRemove(GMaxHeapTree *aTree, void *aMaxData)
{
	if (GMaxHeapTreeGetSize(aTree) > 1)
	{
		if (NULL != aMaxData)
		{
			memcpy(aMaxData, GVectorElementAtIndex(GMaxHeapTreeGetVector(aTree), 1), GVectorGetElementsSize(GMaxHeapTreeGetVector(aTree)));
		}
	}
	else
	{
		printf("The Heap is empty!\n");
		return;
	}
	
	void *data = malloc(GVectorGetElementsSize(GMaxHeapTreeGetVector(aTree)));
	assert(NULL != data);
	
	GVectorElementPop(GMaxHeapTreeGetVector(aTree), data);
	if (GMaxHeapTreeGetSize(aTree) > 1)
 	{
		GMaxHeapTreePercolateDown(aTree, 1, data);
	}
	
	free(data);
	data = NULL;
}

#pragma mark -

int GMaxHeapTreeParentNodeForNodeAtIndex(GMaxHeapTree *aTree, int aNodeIndex)
{
	if (aNodeIndex >= GMaxHeapTreeGetSize(aTree))
		return -1;
	assert(aNodeIndex > 0);
	return aNodeIndex / 2;
}

int GMaxHeapTreeLeftChildForNodeAtIndex(GMaxHeapTree *aTree, int aNodeIndex)
{
	if (aNodeIndex >= GMaxHeapTreeGetSize(aTree))
		return -1;
	assert(aNodeIndex > 0);
	int leftChildIndex = 2 * aNodeIndex;
	if (leftChildIndex < GMaxHeapTreeGetSize(aTree))
	{
		return leftChildIndex;
	}
	return -1;
}

int GMaxHeapTreeRightChildForNodeAtIndex(GMaxHeapTree *aTree, int aNodeIndex)
{
	int leftChildIndex = GMaxHeapTreeLeftChildForNodeAtIndex(aTree, aNodeIndex);
	if (leftChildIndex > 0 && leftChildIndex < GMaxHeapTreeGetSize(aTree) - 1)
	{
		return leftChildIndex + 1;
	}
	return -1;
}

#pragma mark - 

void GMaxHeapTreePercolateUp(GMaxHeapTree *aTree, int aNodeIndex, void *aData)
{
	void *parentData = NULL;
	void *hole = GVectorElementAtIndex(GMaxHeapTreeGetVector(aTree), aNodeIndex);
	
	int parentIndex = GMaxHeapTreeParentNodeForNodeAtIndex(aTree, aNodeIndex);
	while (parentIndex > 0)
	{
		parentData = GVectorElementAtIndex(GMaxHeapTreeGetVector(aTree), parentIndex);
		ComparisonResult compareResult = aTree -> comparePredicate(parentData, aData);
		if (compareResult == Less)
		{
			memcpy(hole, parentData, GVectorGetElementsSize(GMaxHeapTreeGetVector(aTree)));
			
			hole = parentData;
			parentIndex = GMaxHeapTreeParentNodeForNodeAtIndex(aTree, parentIndex);
		}
		else
		{
			parentIndex = 0;
		}
	}
	if (NULL != GMaxHeapTreeGetVector(aTree) -> elementCopier)
	{
		GMaxHeapTreeGetVector(aTree) -> elementCopier(hole, aData);
	}
	else
	{
		memcpy(hole, aData, GVectorGetElementsSize(GMaxHeapTreeGetVector(aTree)));
	}
}

void GMaxHeapTreePercolateDown(GMaxHeapTree *aTree, int aNodeIndex, void *aData)
{
	void *rightChildData = NULL;
	void *leftChildData = NULL;
	void *hole = GVectorElementAtIndex(GMaxHeapTreeGetVector(aTree), aNodeIndex);

	int rightChildIndex = GMaxHeapTreeRightChildForNodeAtIndex(aTree, aNodeIndex);
	int leftChildIndex = GMaxHeapTreeLeftChildForNodeAtIndex(aTree, aNodeIndex);
	while (leftChildIndex > 0)
 	{
		leftChildData = GVectorElementAtIndex(GMaxHeapTreeGetVector(aTree), leftChildIndex);
		void *childData = leftChildData;
		int childIndex = leftChildIndex;
		
		if (rightChildIndex > 0)
		{
			rightChildData = GVectorElementAtIndex(GMaxHeapTreeGetVector(aTree), rightChildIndex);
			ComparisonResult compareResult = aTree -> comparePredicate(leftChildData, rightChildData);
			if (compareResult == Less)
			{
				childData = rightChildData;
				childIndex = rightChildIndex;
			}
		}
		
		ComparisonResult compareResult = aTree -> comparePredicate(childData, aData);
		if (compareResult == More)
		{
			memcpy(hole, childData, GVectorGetElementsSize(GMaxHeapTreeGetVector(aTree)));
			
			hole = childData;
			leftChildIndex = GMaxHeapTreeLeftChildForNodeAtIndex(aTree, childIndex);
			rightChildIndex = GMaxHeapTreeRightChildForNodeAtIndex(aTree, childIndex);
		}
		else
		{
			leftChildIndex = -1;
		}
	}
	memcpy(hole, aData, GVectorGetElementsSize(GMaxHeapTreeGetVector(aTree)));
}
