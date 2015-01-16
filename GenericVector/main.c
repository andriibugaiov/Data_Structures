
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "string.h"

#include "GVector.h"

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

int stringElementComparePredicate(void *aLeftElement, void *aRightElement)
{
	char **leftElement = aLeftElement;
	char **rightElement = aRightElement;
	
	return 0;
}

int main(int argc, const char *argv[])
{
	char *array[] = {"a", "b", "c", "d", "e"}; // , "d", "f", "g", "h", "j", "k", "l"};
	int size = sizeof(array)/sizeof(array[0]);
	
	GVector vector;
	GVectorNew(&vector, sizeof(char *), stringElementCopier, stringElementDisposer);
	
	for (int i = 0; i < size; ++i)
	{
		char *element = array[i];
		GVectorElementInsert(&vector, 0, &element);
	}
	
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < vector.length; ++j)
		{
			printf("%s, ", *(char **)GVectorElementAtIndex(&vector, j));
		}
		printf("\n");
		
		char *element = NULL;
		GVectorElementRemove(&vector, 0, &element);
		printf("Removed: %s \n", element);
		
		stringElementDisposer((void *)&element);
	}
	
	GVectorDispose(&vector);
	
	return 0;
}
