
struct _GVector
{
	int length;
	int elementsSize;
	
	int logicalLength;
	void *origin;
	
	void (*elementCopier)(void *aDestination, void *aSource);
	void (*elementDisposer)(void *);
};
typedef struct _GVector GVector;

int GVectorGetLength(GVector *aVector);
int GVectorGetElementsSize(GVector *aVector);

void GVectorNew(GVector *aVector, int anElementsSize, void (*anElementCopier)(void *aDestination, void *aSource), void (*anElementDisposer)(void *));
void GVectorDispose(GVector *aVector);

void *GVectorElementAtIndex(GVector *aVector, int anIndex);

void GVectorElementInsert(GVector *aVector, int anIndex, void *anInsertElement);
void GVectorElementPushBack(GVector *aVector, void *anPushBackElement);

void GVectorElementRemove(GVector *aVector, int anIndex, void *anElement);
void GVectorElementPop(GVector *aVector, void *anElement);
