
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "GVector.h"

#define INITIAL_LOGICAL_LENGTH 4
#define NTH_ELEMENT(ORIGIN, ELEMENT_SIZE, I)({(char *)(ORIGIN) + (ELEMENT_SIZE) * (I);})

void GVectorSetLength(GVector *aVector, int aLength);
void GVectorSetElementsSize(GVector *aVector, int aSize);

#pragma mark -

int GVectorGetLength(GVector *aVector)
{
	return aVector -> length;
}

void GVectorSetLength(GVector *aVector, int aLength)
{
	aVector -> length = aLength;
}

int GVectorGetElementsSize(GVector *aVector)
{
	return aVector -> elementsSize;
}

void GVectorSetElementsSize(GVector *aVector, int anElementsSize)
{
	aVector -> elementsSize = anElementsSize;
}

#pragma mark -

void GVectorNew(GVector *aVector, int anElementsSize, void (*anElementCopier)(void *aDestination, void *aSource), void (*anElementDisposer)(void *))
{
	assert(anElementsSize > 0);
	
	GVectorSetLength(aVector, 0);
	GVectorSetElementsSize(aVector, anElementsSize);
	
	aVector -> logicalLength = INITIAL_LOGICAL_LENGTH;
	aVector -> origin = malloc(anElementsSize * INITIAL_LOGICAL_LENGTH);
	assert(NULL != aVector -> origin);
	
	aVector -> elementCopier = anElementCopier;
	aVector -> elementDisposer = anElementDisposer;
}

void GVectorDispose(GVector *aVector)
{
	if (NULL != aVector -> elementDisposer)
	{
		for (int elemntIndex = 0; elemntIndex < GVectorGetLength(aVector); ++elemntIndex)
		{
			void *element = NTH_ELEMENT(aVector -> origin, GVectorGetElementsSize(aVector), elemntIndex);
			aVector -> elementDisposer(element);
		}
	}
	free(aVector -> origin);
	
	GVectorSetLength(aVector, 0);
	GVectorSetElementsSize(aVector, 0);
	
	aVector -> logicalLength = 0;
	aVector -> origin = NULL;
	
	aVector -> elementCopier = NULL;
	aVector -> elementDisposer = NULL;

}

void *GVectorElementAtIndex(GVector *aVector, int anIndex)
{
	if (anIndex < 0 || anIndex >= GVectorGetLength(aVector)) return NULL;
	return NTH_ELEMENT(aVector -> origin, GVectorGetElementsSize(aVector), anIndex);
}

#pragma mark -

void GVectorElementInsert(GVector *aVector, int anIndex, void *anInsertElement)
{
	if (anIndex < 0 || anIndex > GVectorGetLength(aVector)) return;
	if (NULL != anInsertElement)
	{
		if (aVector -> logicalLength == GVectorGetLength(aVector))
		{
			aVector -> logicalLength *= 2;
			aVector -> origin = realloc(aVector -> origin, GVectorGetElementsSize(aVector) * aVector -> logicalLength);
			assert(NULL != aVector -> origin);
		}
		
		void *insertElement = NTH_ELEMENT(aVector -> origin, GVectorGetElementsSize(aVector), anIndex);
		if (anIndex < GVectorGetLength(aVector))
		{
			int count = GVectorGetLength(aVector) - anIndex;
			void *destinationElement = NTH_ELEMENT(aVector -> origin, GVectorGetElementsSize(aVector), anIndex + 1);
			memmove(destinationElement, insertElement, GVectorGetElementsSize(aVector) * count);
		}
		
		if (NULL != aVector -> elementCopier)
		{
			aVector -> elementCopier(insertElement, anInsertElement);
		}
		else
		{
			memcpy(insertElement, anInsertElement, GVectorGetElementsSize(aVector));
		}
		
		GVectorSetLength(aVector, GVectorGetLength(aVector) + 1);
	}
}

void GVectorElementPushBack(GVector *aVector, void *anAppendElement)
{
	GVectorElementInsert(aVector, GVectorGetLength(aVector), anAppendElement);
}

#pragma mark -

void GVectorElementRemove(GVector *aVector, int anIndex, void *anElement)
{
	if (NULL == anElement || anIndex < 0 || anIndex >= GVectorGetLength(aVector)) return;
	
	void *removeElement = NTH_ELEMENT(aVector -> origin, GVectorGetElementsSize(aVector), anIndex);
	if (NULL == anElement)
 	{
		if (NULL != aVector -> elementDisposer)
		{
			aVector -> elementDisposer(removeElement);
		}
	}
	else
	{
		memcpy(anElement, removeElement, GVectorGetElementsSize(aVector));
	}
	
	if (anIndex < GVectorGetLength(aVector) - 1)
	{
		int count = GVectorGetLength(aVector) - 1 - anIndex;
		void *sourceElement = NTH_ELEMENT(aVector -> origin, GVectorGetElementsSize(aVector), anIndex + 1);
		memmove(removeElement, sourceElement, GVectorGetElementsSize(aVector) * count);
	}
	GVectorSetLength(aVector, GVectorGetLength(aVector) - 1);
}

void GVectorElementPop(GVector *aVector, void *anElement)
{
	GVectorElementRemove(aVector, GVectorGetLength(aVector) - 1, anElement);
}
