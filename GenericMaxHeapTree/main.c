
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "string.h"

#include "GMaxHeapTree.h"

void stringElementCopier(void *aDestination, void *aSource)
{
	char **elementDestination = aDestination;
	char **elementSource = aSource;
	
	if (*elementSource != NULL)
	{
		unsigned long length = strlen((const char *)(*elementSource));
		*elementDestination = malloc(sizeof(char) * (length + 1));
		assert(NULL != *elementDestination);
		strcpy(*elementDestination, *elementSource);
	}
}

void stringElementDisposer(void *anElement)
{
	char **element = anElement;
	if (NULL != *element)
 	{
		free(*element);
		*element = NULL;
	}
}

ComparisonResult stringElementComparePredicate(void *aLeftElement, void *aRightElement)
{
	char **leftElement = aLeftElement;
	char **rightElement = aRightElement;
	int result = strcmp(*leftElement, *rightElement);
	if (result < 0)
	{
		return Less;
	}
	else if (result > 0)
	{
		return More;
	}
	return Equal;
}

int main(int argc, const char *argv[])
{
	char *array[] = {"f", "r", "a", "t", "l", "n", "b", "x", "y", "x", "o", "j", "b", "w", "x"};
	int size = sizeof(array) / sizeof(array[0]);
	
	GMaxHeapTree tree;
	GMaxHeapTreeNew(&tree, sizeof(char *), stringElementCopier, stringElementDisposer,stringElementComparePredicate);
	
	for (int index = 0; index < size; ++index)
	{
		char *string = array[index];
		GMaxHeapTreeDataAdd(&tree, &string);
	}
	
	for (int index = 0; index < size; ++index)
	{
		char *string = NULL;
		GMaxHeapTreeDataMaxRemove(&tree, &string);
		printf("Removed: %s \n", string);

		stringElementDisposer(&string);
	}
	
	GMaxHeapTreeDispose(&tree);
	return 0;
}
