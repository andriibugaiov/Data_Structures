
#include "Constants.h"

typedef enum _Color{
	Red,
	Black,
	ColorNone
}Color;

typedef enum _NodePosition{
	Left,
	Right,
	Root,
	NodePositionNone
} NodePosition;

// Private - Begin
struct _GNode{
	struct _GNode *parent;
	struct _GNode *left;
	struct _GNode *right;
	
	Color color;
	
	void *data;
};
// Private - End

typedef struct _GNode GNode;

// Public - Begin
// designated initializer/constructor
void GNodeNew(GNode *aNode, void *aData, int aNodeDataSize, void (* aNodeDataCopier)(void *aDestinationData, void *aSourceData));
// designated disposer/destructor
void GNodeDispose(GNode *aNode, void (* aNodeDataDisposer)(void *));

// accessors
// getters
GNode *GNodeGetParent(GNode *aNode);
GNode *GNodeGetLeft(GNode *aNode);
GNode *GNodeGetRight(GNode *aNode);
Color GNodeGetColor(GNode *aNode);
void *GNodeGetData(GNode *aNode);
// setters
void GNodeSetParent(GNode *aNode, GNode *aParent);
void GNodeSetLeft(GNode *aNode, GNode *aLeft);
void GNodeSetRight(GNode *aNode, GNode *aRight);
void GNodeSetColor(GNode *aNode, Color aColor);

// performs analysis of the node's references and returns the position of the node in the tree's structure
NodePosition GNodePosition(GNode *aNode);

// retrieves a child node for the parent node 'aNode' using its relative to parent position - 'aNodePosition'
GNode *GNodeGetWithNodePosition(GNode *aNode, NodePosition aNodePosition);

// sets 'aNodeToSet' as a child of 'aNode' using its relative to parent position - 'aNodePosition'
void GNodeSetWithNodePosition(GNode *aNode, NodePosition aNodePosition, GNode *aNodeToSet);

// returns the deepest left descendant
GNode *GNodeLeftDescendant(GNode *aNode);

// tests if 'aNode's references are correctly set
void GNodeTestStructure(GNode *aNode, int aBlackHeigth);

void GNodeDescription(GNode *aNode, void (* aNodeDataDescriptor)(void *));

void GNodeResetBlackHeightCounter();
// Public - End
