
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GNode.h"

// Protected - Begin
void GNodeSetData(GNode *aNode, void *aData);
// Protected- End

// IMPLEMENTATAION - NODE ENTITY (wrapper for data, used within the tree only)
void GNodeSetData(GNode *aNode, void *aData)
{
	aNode -> data = aData;
}

void GNodeNew(GNode *aNode, void *aData, int aNodeDataSize, void (* aNodeDataCopier)(void *aDestinationData, void *aSourceData))
{
	GNodeSetParent(aNode, NULL);
	GNodeSetLeft(aNode, NULL);
	GNodeSetRight(aNode, NULL);
	GNodeSetColor(aNode, Red);
	
	void *ownedData = malloc(aNodeDataSize);
	assert(NULL != ownedData);
	if (NULL != aNodeDataCopier)
	{
		(*aNodeDataCopier)(ownedData, aData);
	}
	else
	{
		memcpy(ownedData, aData, aNodeDataSize);
	}
	GNodeSetData(aNode, ownedData);
}

void GNodeDispose(GNode *aNode, void (* aNodeDataDisposer)(void *))
{
	GNodeSetParent(aNode, NULL);
	GNodeSetLeft(aNode, NULL);
	GNodeSetRight(aNode, NULL);
	GNodeSetColor(aNode, ColorNone);
	
	void *ownedData = GNodeGetData(aNode);
	if (NULL != aNodeDataDisposer)
	{
		(*aNodeDataDisposer)(ownedData);
	}
	free(ownedData);
	GNodeSetData(aNode, NULL);
}

// accessors
// getters
GNode *GNodeGetParent(GNode *aNode)
{
	return aNode -> parent;
}

GNode *GNodeGetLeft(GNode *aNode)
{
	return aNode -> left;
}

GNode *GNodeGetRight(GNode *aNode)
{
	return aNode -> right;
}

Color GNodeGetColor(GNode *aNode)
{
	return aNode -> color;
}

void *GNodeGetData(GNode *aNode)
{
	return aNode -> data;
}

// setters
void GNodeSetParent(GNode *aNode, GNode *aParent)
{
	aNode -> parent = aParent;
}

void GNodeSetLeft(GNode *aNode, GNode *aLeft)
{
	aNode -> left = aLeft;
}

void GNodeSetRight(GNode *aNode, GNode *aRight)
{
	aNode -> right = aRight;
}

void GNodeSetColor(GNode *aNode, Color aColor)
{
	aNode -> color = aColor;
}

NodePosition GNodePosition(GNode *aNode)
{
	NodePosition nodePosition = NodePositionNone;
	GNode *parent = GNodeGetParent(aNode);
	if (parent == NULL)
	{
		nodePosition = Root;
	}
	else if (aNode == GNodeGetLeft(parent))
	{
		nodePosition = Left;
	}
	else if (aNode == GNodeGetRight(parent))
	{
		nodePosition = Right;
	}
	assert(nodePosition != NodePositionNone && "Fatal error! The parent of the node doesn't have it as a child!");
	return nodePosition;
}

GNode *GNodeGetWithNodePosition(GNode *aNode, NodePosition aNodePosition)
{
	if (aNodePosition == Left) return GNodeGetLeft(aNode);
	if (aNodePosition == Right) return GNodeGetRight(aNode);
	assert(!"Fatal error! Wrong node position to GET child!");
	return NULL;
}

void GNodeSetWithNodePosition(GNode *aNode, NodePosition aNodePosition, GNode *aNodeToSet)
{
	if (aNodePosition == Root) return;
	else if (aNodePosition == Left)
	{
		GNodeSetLeft(aNode, aNodeToSet);
	}
	else if (aNodePosition == Right)
	{
		GNodeSetRight(aNode, aNodeToSet);
	}
	else
	{
		assert(!"Fatal error! Wrong node position to SET child!");
	}
	if (NULL != aNodeToSet)
	{
		GNodeSetParent(aNodeToSet, aNode);
	}
}

// here 'aNode' must have left child
GNode *GNodeLeftDescendant(GNode *aNode)
{
	GNode *leftDescendant = GNodeGetLeft(aNode);
	while (NULL != GNodeGetRight(leftDescendant))
	{
		leftDescendant = GNodeGetRight(leftDescendant);
	}
	return leftDescendant;
}

static int blackHeightCounter = 0;
void GNodeTestStructure(GNode *aNode, int aBlackHeigth)
{
	int blackHeigth = aBlackHeigth;
	GNode *parent = GNodeGetParent(aNode);
	Color color = GNodeGetColor(aNode);
	assert(color != ColorNone);
	NodePosition nodePosition = GNodePosition(aNode);
	assert(nodePosition != NodePositionNone);
	if (Root == nodePosition)
	{
		++blackHeigth;
		assert(Black == color);
		assert(NULL == parent);
	}
	else
	{
		if (Black == color)
		{
			++blackHeigth;
		}
		else if (Red == color)
		{
			assert(Black == GNodeGetColor(parent));
		}
		
		if (Left == nodePosition)
		{
			assert(aNode == GNodeGetLeft(GNodeGetParent(aNode)));
		}
		else if (Right == nodePosition)
		{
			assert(aNode == GNodeGetRight(GNodeGetParent(aNode)));
		}
	}
	
	GNode *left = GNodeGetLeft(aNode);
	if (NULL != left)
	{
		GNodeTestStructure(left, blackHeigth);
	}
	GNode *right = GNodeGetRight(aNode);
	if (NULL != right)
	{
		GNodeTestStructure(right, blackHeigth);
	}
	
	if (NULL == left && NULL == right)
	{
		if (blackHeightCounter == 0)
		{
			blackHeightCounter = blackHeigth;
//			printf("Black Height - %d;\n", blackHeigth);
		}
		assert(blackHeightCounter == blackHeigth);
	}
}

void GNodeResetBlackHeightCounter()
{
	blackHeightCounter = 0;
}

void GNodeDescription(GNode *aNode, void (* aNodeDataDescriptor)(void *))
{
	GNode *left = GNodeGetLeft(aNode);
	if (NULL != left)
	{
		GNodeDescription(left, aNodeDataDescriptor);
	}
	
	//    Color nodeColor = GNodeGetColor(aNode);
	//    const char *colors[] = {"Red", "Black"}
	
	//    NodePosition nodePosition = GNodePosition(aNode);
	//    const char *positions[] = {"Left", "Right", "Root"}
	//    printf("%s   -   %s   -   ", positions[nodePosition], colors[nodeColor]);
	
	if (NULL != aNodeDataDescriptor)
	{
		(* aNodeDataDescriptor)(GNodeGetData(aNode));
	}
	
	GNode *right = GNodeGetRight(aNode);
	if (NULL != right)
	{
		GNodeDescription(right, aNodeDataDescriptor);
	}
}
