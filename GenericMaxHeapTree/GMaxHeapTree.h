
#include "Constants.h"
#include "GVector.h"

struct _GMaxHeapTree{
	GVector *vector;
	ComparisonResult (*comparePredicate)(void *leftOperand, void *rightOperand);
};
typedef struct _GMaxHeapTree GMaxHeapTree;

GVector *GMaxHeapTreeGetVector(GMaxHeapTree *aTree);
void GMaxHeapTreeSetVector(GMaxHeapTree *aTree, GVector *aVector);

int GMaxHeapTreeGetSize(GMaxHeapTree *aTree);

void GMaxHeapTreeNew(GMaxHeapTree *aTree, int anDataSize, void (*anDataCopier)(void *aDestination, void *aSource), void (*anDataDisposer)(void *), ComparisonResult (*aComparePredicate)(void *leftOperand, void *rightOperand));
void GMaxHeapTreeDispose(GMaxHeapTree *aTree);

void GMaxHeapTreeDataAdd(GMaxHeapTree *aTree, void *aData);
void GMaxHeapTreeDataMaxRemove(GMaxHeapTree *aTree, void *aMaxData);
