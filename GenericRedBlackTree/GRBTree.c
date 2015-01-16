
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GRBTree.h"

// Protected - Begin
//accessors
//getters
GNode *GRBTreeGetRoot(GRBTree *aTree);

// setters
void GRBTreeSetRoot(GRBTree *aTree, GNode *aNode);
void GRBTreeSetSize(GRBTree *aTree, int aSize);

void GRBTreeSizeIncrement(GRBTree *aTree);
void GRBTreeSizeDecrement(GRBTree *aTree);

// replaces 'aNodeToBeReplaced' with 'aNode' in 'aTree', resetting all the necessary references for both
void GRBTreeReplace(GRBTree *aTree, GNode *aNodeToBeReplaced, GNode *aNode);

// returns a node address which data is equal to the aNode's data (if such a node appears present in the tree), otherwise, writes the address of the suggested parent for the aNode into the aParent
GNode *GRBTreeNodeSearch(GNode *aNode, GNode *aNodeToSearch, GNode **aParent, ComparisonResult (*aNodeDataComparePredicate)(void *, void *));

void GRBTreeNodeDispose(GNode *aNode, void (* aNodeDataDisposer)(void *));

// analyses the position and the color of 'aChildNode' and its ancestors, balances the tree by means of 'trinode recoloring' or 'restructuring'
void GRBTreeBalance(GRBTree *aTree, GNode *aChildNode);
void GRBTreeRotateLeft(GRBTree *aTree, GNode *aNode);
void GRBTreeRotateRight(GRBTree *aTree, GNode *aNode);

void GRBTreeNodeRemove(GRBTree *aTree, GNode *aNode);
void GRBTreeNodeRemoveFixUp(GRBTree *aTree, GNode *aParent, GNode *aNode, NodePosition aPosition);

#if 0 // {
// performs the actual restructuring of 'aTree' using
void GRBTreeRotate(GRBTree *aTree, GNode *aGrandParent, GNode *aParent, GNode *aChild, NodePosition aRotationDirection, MyBool isChildParent);
#endif
// Protected - End

// IMPLEMENTATION - GENERIC RED-BLACK TREE ENTITY

GNode *GRBTreeGetRoot(GRBTree *aTree)
{
    return aTree -> root;
}

int GRBTreeGetSize(GRBTree *aTree)
{
    return aTree -> size;   
}

void GRBTreeSetRoot(GRBTree *aTree, GNode *aNode)
{
	aTree -> root = aNode;
}

void GRBTreeSetSize(GRBTree *aTree, int aSize)
{
    aTree -> size = aSize;
}

void GRBTreeSizeIncrement(GRBTree *aTree)
{
   int size = GRBTreeGetSize(aTree);
   GRBTreeSetSize(aTree, size + 1); 
}

void GRBTreeSizeDecrement(GRBTree *aTree)
{
   int size = GRBTreeGetSize(aTree);
   if (size > 0)
   {
        GRBTreeSetSize(aTree, size - 1); 
   }
}

void GRBTreeReplace(GRBTree *aTree, GNode *aNodeToBeReplaced, GNode *aNode)
{
	// color
	GNodeSetColor(aNode, GNodeGetColor(aNodeToBeReplaced));
	
	// parent
	NodePosition nodePosition = GNodePosition(aNodeToBeReplaced);
	if (Root == nodePosition)
	{
		GNodeSetParent(aNode, NULL);
		GRBTreeSetRoot(aTree, aNode);
	}
	else
	{
		GNode *parent = GNodeGetParent(aNodeToBeReplaced);
		GNodeSetWithNodePosition(parent, nodePosition, aNode);
	}
	
	// left
	GNode *left = GNodeGetLeft(aNodeToBeReplaced);
	GNodeSetWithNodePosition(aNode, Left, left);
	
	// right
	GNode *right = GNodeGetRight(aNodeToBeReplaced);
	GNodeSetWithNodePosition(aNode, Right, right);
}

MyBool GRBTreeIsEmpty(GRBTree *aTree)
{
    return (GRBTreeGetRoot(aTree) == NULL) ? YES : NO;
}

void GRBTreeNew(GRBTree *aTree, int aNodeDataSize, ComparisonResult (*aNodeDataComparePredicate)(void *, void *), void (* aNodeDataDisposer)(void *), void (* aNodeDataCopier)(void *aDestinationData, void *aSourceData))
{
    aTree -> nodeDataSize = aNodeDataSize;
    aTree -> nodeDataComparePredicate = aNodeDataComparePredicate;
    aTree -> nodeDataDisposer = aNodeDataDisposer;
    aTree -> nodeDataCopier = aNodeDataCopier;

    GRBTreeSetRoot(aTree, NULL);    
    GRBTreeSetSize(aTree, 0);
}

void GRBTreeDispose(GRBTree *aTree)
{  
    if (GRBTreeIsEmpty(aTree) == NO)
    {
        GNode *root = GRBTreeGetRoot(aTree);
        GRBTreeNodeDispose(root, aTree -> nodeDataDisposer);
        GRBTreeSetRoot(aTree, NULL);
        GRBTreeSetSize(aTree, 0);
    }
}

void GRBTreeNodeDispose(GNode *aNode, void (* aNodeDataDisposer)(void *))
{
    GNode *left = GNodeGetLeft(aNode);
    GNode *right = GNodeGetRight(aNode);
    if (NULL != left)
    {
        GRBTreeNodeDispose(left, aNodeDataDisposer);   
    }
    if (NULL != right)
    {
        GRBTreeNodeDispose(right, aNodeDataDisposer);
    }
    GNodeDispose(aNode, aNodeDataDisposer);
    free(aNode);
}

#pragma mark - Search

void *GRBTreeDataSearch(GRBTree *aTree, void *aData)
{
    if (GRBTreeIsEmpty(aTree) == YES) return NULL;
 
    GNode *nodeToSearch = malloc(sizeof(GNode));
    GNodeNew(nodeToSearch, aData, aTree -> nodeDataSize, aTree -> nodeDataCopier);

    GNode *parent = NULL;
    GNode *root = GRBTreeGetRoot(aTree);
    GNode *searchResult = GRBTreeNodeSearch(root, nodeToSearch, &parent, aTree -> nodeDataComparePredicate);

    GNodeDispose(nodeToSearch, aTree -> nodeDataDisposer);
    free(nodeToSearch);
    nodeToSearch = NULL;

    if (searchResult == NULL) return NULL;
    return GNodeGetData(searchResult);
}

GNode *GRBTreeNodeSearch(GNode *aNode, GNode *aNodeToSearch, GNode **aParent, ComparisonResult (*aNodeDataComparePredicate)(void *, void *))
{
	void *data = GNodeGetData(aNode);
	void *dataToSearch = GNodeGetData(aNodeToSearch);
	ComparisonResult compResult = (*aNodeDataComparePredicate)(data, dataToSearch);
	if (compResult == Equal)
	{
		*aParent = NULL;
		return aNode;
	}
	else if (compResult == More)
	{
		GNode *left = GNodeGetLeft(aNode);
		if (NULL == left)
		{
			*aParent = aNode;
			return NULL;
		}
		else
		{
			return GRBTreeNodeSearch(left, aNodeToSearch, aParent, aNodeDataComparePredicate);
		}
	}
	else if (compResult == Less)
	{
		GNode *right = GNodeGetRight(aNode);
		if (NULL == right)
		{
			*aParent = aNode;
			return NULL;
		}
		else
		{
			return GRBTreeNodeSearch(right, aNodeToSearch, aParent, aNodeDataComparePredicate);
		}
	}
	assert(!"Fatal error! Undefined comparison result!");
	*aParent = NULL;
	return NULL;
}

#pragma mark - Add

MyBool GRBTreeDataAdd(GRBTree *aTree, void *aData)
{
    GNode *nodeToAdd = malloc(sizeof(GNode));
    GNodeNew(nodeToAdd, aData, aTree -> nodeDataSize, aTree -> nodeDataCopier);
    if (GRBTreeIsEmpty(aTree) == YES)
    {
        GNodeSetColor(nodeToAdd, Black);
		GNodeSetParent(nodeToAdd, NULL);
        GRBTreeSetRoot(aTree, nodeToAdd);
        GRBTreeSizeIncrement(aTree); 
        return NO; // the node was NOT replaced
    }

    GNode *parent = NULL;
    GNode *root = GRBTreeGetRoot(aTree);
    GNode *searchResult = GRBTreeNodeSearch(root, nodeToAdd, &parent, aTree -> nodeDataComparePredicate);
    
    if (searchResult == NULL)
    {       
        void *newData = GNodeGetData(nodeToAdd);
        void *parentData = GNodeGetData(parent);
        ComparisonResult compResult = (* aTree -> nodeDataComparePredicate)(parentData, newData);
        if (compResult == More)
        {
            GNodeSetWithNodePosition(parent, Left, nodeToAdd); 
        } 
        else if (compResult == Less)
        {
            GNodeSetWithNodePosition(parent, Right, nodeToAdd); 
        } 
        GRBTreeBalance(aTree, nodeToAdd);
        GRBTreeSizeIncrement(aTree); 
        return NO; // the node was NOT replaced
    }

    GRBTreeReplace(aTree, searchResult, nodeToAdd);
    GNodeDispose(searchResult, aTree -> nodeDataDisposer);
    free(searchResult);
    searchResult = NULL;
    return YES; // the node was replaced
}

void GRBTreeBalance(GRBTree *aTree, GNode *aChildNode)
{
    GNode *parent = GNodeGetParent(aChildNode);
    Color parentColor = GNodeGetColor(parent);
    if (Black == parentColor) return;

    // here a parent node has Red color, which means that it has its grandparent
    NodePosition parentPosition = GNodePosition(parent);
    GNode *grandParent = GNodeGetParent(parent); // not NULL for sure
    GNode *parentSibling = NULL; // caution! can be NULL
    if (Left == parentPosition)
    {
        parentSibling = GNodeGetRight(grandParent);
    }
    else if (Right == parentPosition)
    {
        parentSibling = GNodeGetLeft(grandParent);
    }
    Color parentSiblingColor = Black; 
    if (NULL != parentSibling)
    {
        parentSiblingColor = GNodeGetColor(parentSibling);
    }

    NodePosition childPosition = GNodePosition(aChildNode);
    if (Black == parentSiblingColor)
    {
		if (parentPosition == Left && childPosition == Right)
		{
			GNodeSetColor(aChildNode, Black);
			GRBTreeRotateLeft(aTree, parent);
		}
		else if (parentPosition == Right && childPosition == Left)
		{
			GNodeSetColor(aChildNode, Black);
			GRBTreeRotateRight(aTree, parent);
		}
		else
		{
			GNodeSetColor(parent, Black);
		}
		
		GNodeSetColor(grandParent, Red);
		if (parentPosition == Left)
		{
			GRBTreeRotateRight(aTree, grandParent);
		}
		else if (parentPosition == Right)
		{
			GRBTreeRotateLeft(aTree, grandParent);
		}
		
#if 0 // {
        NodePosition rotationDirection = (parentPosition == Left) ? Right : Left;
        MyBool doesChildBecomeParent =
        ((rotationDirection == Left && childPosition == Left) ||
         (rotationDirection == Right && childPosition == Right)) ? YES : NO;

        GRBTreeRotate(aTree, grandParent, parent, aChildNode, rotationDirection, doesChildBecomeParent);
#endif
    }
    else if (Red == parentSiblingColor)
    {
        GNodeSetColor(grandParent, Red);
        GNodeSetColor(parent, Black);
        if (NULL != parentSibling)
        {
            GNodeSetColor(parentSibling, Black);
        }

        GNode *grandGrandParent = GNodeGetParent(grandParent);
        if (NULL == grandGrandParent) // which means that grandparent is actually a root of the tree
        {
           GNodeSetColor(grandParent, Black);
        }
        else
        {
            GRBTreeBalance(aTree, grandParent); 
        }
    }      
}

void GRBTreeRotateLeft(GRBTree *aTree, GNode *aNode)
{
	GNode *rightChild = GNodeGetRight(aNode);
	if (NULL == rightChild) return;
	
	GNode *parent = GNodeGetParent(aNode);
	NodePosition position = GNodePosition(aNode);
	
	GNode *leftGrandChild = GNodeGetLeft(rightChild);
	
	// 1
	GNodeSetWithNodePosition(rightChild, Left, aNode);
	
	// 2
	GNodeSetWithNodePosition(aNode, Right, leftGrandChild);
	
	// 3
	if (position == Root)
	{
		GNodeSetParent(rightChild, NULL);
		GRBTreeSetRoot(aTree, rightChild);
	}
	else if (position == Left || position == Right)
	{
		GNodeSetWithNodePosition(parent, position, rightChild);
	}
}

void GRBTreeRotateRight(GRBTree *aTree, GNode *aNode)
{
	GNode *leftChild = GNodeGetLeft(aNode);
	if (NULL == leftChild) return;
	
	GNode *parent = GNodeGetParent(aNode);
	NodePosition position = GNodePosition(aNode);
	
	GNode *rightGrandChild = GNodeGetRight(leftChild);
	
	// 1
	GNodeSetWithNodePosition(leftChild, Right, aNode);
	
	// 2
	GNodeSetWithNodePosition(aNode, Left, rightGrandChild);
	
 	// 3
	if (position == Root)
	{
		GNodeSetParent(leftChild, NULL);
		GRBTreeSetRoot(aTree, leftChild);
	}
	else if (position == Left || position == Right)
	{
		GNodeSetWithNodePosition(parent, position, leftChild);		
	}
}

#if 0 // {
void GRBTreeRotate(GRBTree *aTree, GNode *aGrandParent, GNode *aParent, GNode *aChild, NodePosition aRotationDirection, MyBool isChildParent)
{
   NodePosition grandParentPosition = GNodePosition(aGrandParent);
   GNode *grandGrandParent = GNodeGetParent(aGrandParent); 
   if (YES == isChildParent)
   {     
       if (Right == aRotationDirection)
       {
            GNode *left = GNodeGetLeft(aChild);
		    GNodeSetWithNodePosition(aParent, Right, left);

            GNode *right = GNodeGetRight(aChild);
		    GNodeSetWithNodePosition(aGrandParent, Left, right);

            GNodeSetWithNodePosition(aChild, Left, aParent);
            GNodeSetWithNodePosition(aChild, Right, aGrandParent);
       }
       else if (Left == aRotationDirection)
       {
			GNode *left = GNodeGetLeft(aChild);
			GNodeSetWithNodePosition(aGrandParent, Right,  left);

			GNode *right = GNodeGetRight(aChild);
			GNodeSetWithNodePosition(aParent, Left, right);

			GNodeSetWithNodePosition(aChild, Right, aParent);
			GNodeSetWithNodePosition(aChild, Left, aGrandParent);
       }

       if (NULL != grandGrandParent)
       { 
            GNodeSetWithNodePosition(grandGrandParent, grandParentPosition, aChild);
       }
       else // that means the that grandparent is a root of the tree
       {
           GNodeSetParent(aChild, NULL);
           GRBTreeSetRoot(aTree, aChild);
       }
       GNodeSetColor(aChild, Black);
       GNodeSetColor(aGrandParent, Red);
   }
   else if (NO == isChildParent)
   {
        GNode *childSibling = NULL;
        NodePosition childPosition = GNodePosition(aChild);
        if (Left == childPosition)
        {
           childSibling = GNodeGetRight(aParent);
        }
        else if (Right == childPosition)
        {
           childSibling = GNodeGetLeft(aParent);
        } 

        if (Right == aRotationDirection)
        {
			GNodeSetWithNodePosition(aGrandParent, Left, childSibling);
            GNodeSetWithNodePosition(aParent, Right, aGrandParent);
        }  
        else if (Left == aRotationDirection)
        {
			GNodeSetWithNodePosition(aGrandParent, Right, childSibling);
            GNodeSetWithNodePosition(aParent, Left, aGrandParent);
        }

        if (NULL != grandGrandParent)
        {
            GNodeSetWithNodePosition(grandGrandParent, grandParentPosition, aParent);
        }
        else // that means that the grandparent is a root of the tree
        {
            GNodeSetParent(aParent, NULL);
            GRBTreeSetRoot(aTree, aParent);
        }
        GNodeSetColor(aParent, Black); 
        GNodeSetColor(aGrandParent, Red);     
    }        
}
#endif

#pragma mark - Remove

void GRBTreeDataRemove(GRBTree *aTree, void *aData)
{
	if (GRBTreeIsEmpty(aTree) == YES) return;
 
	GNode *nodeToSearch = malloc(sizeof(GNode));
	GNodeNew(nodeToSearch, aData, aTree -> nodeDataSize, aTree -> nodeDataCopier);
	
	GNode *parent = NULL;
	GNode *root = GRBTreeGetRoot(aTree);
	GNode *searchResult = GRBTreeNodeSearch(root, nodeToSearch, &parent, aTree -> nodeDataComparePredicate);
	
	GNodeDispose(nodeToSearch, aTree -> nodeDataDisposer);
	free(nodeToSearch);
	nodeToSearch = NULL;
	
	if (searchResult == NULL) return;
	GRBTreeNodeRemove(aTree, searchResult);
}

void GRBTreeNodeRemove(GRBTree *aTree, GNode *aNode)
{
	GNode *nodeToRemove = NULL;
	if (NULL != GNodeGetRight(aNode) && NULL != GNodeGetLeft(aNode))
	{
		nodeToRemove = GNodeLeftDescendant(aNode);
	}
	else
	{
		nodeToRemove = aNode;
	}
	
	// saves node's info
	Color color = GNodeGetColor(nodeToRemove);
	
	NodePosition position = GNodePosition(nodeToRemove);
	GNode *parent = GNodeGetParent(nodeToRemove);
	
	GNode *child = NULL;
	if (NULL != GNodeGetLeft(nodeToRemove))
 	{
		child = GNodeGetLeft(nodeToRemove);
	}
	else
	{
		child = GNodeGetRight(nodeToRemove);
	}
	
	// replaces 'aNode' with its descendant
	if (aNode != nodeToRemove)
 	{
		if (aNode == parent)
		{
			GNodeSetParent(nodeToRemove, NULL);
			GNodeSetWithNodePosition(parent, position, NULL);

			parent = nodeToRemove;
		}
		GRBTreeReplace(aTree, aNode, nodeToRemove);
	}
	// disposes of 'aNode'
	GRBTreeSizeDecrement(aTree);
	GNodeDispose(aNode, aTree -> nodeDataDisposer);
	free(aNode);
	aNode = NULL;
	
	// reconnects the links
	if (Root == position)
	{
		GRBTreeSetRoot(aTree, child);
		if (NULL != child)
		{
			GNodeSetParent(child, NULL);
		}
	}
	else
	{
		GNodeSetWithNodePosition(parent, position, child);
	}
	
	if (Black == color)
	{
		GRBTreeNodeRemoveFixUp(aTree, parent, child, position);
	}
}

void GRBTreeNodeRemoveFixUp(GRBTree *aTree, GNode *aParent, GNode *aNode, NodePosition aPosition)
{
	GNode *node = aNode;
	
	GNode *parent = (node != NULL) ? GNodeGetParent(node) : aParent;
	NodePosition position = (node != NULL) ? GNodePosition(node) : aPosition;
	Color color = (node != NULL) ? GNodeGetColor(node) : Black;
	
	while (position != Root && color == Black)
 	{
		assert(parent != NULL && position != NodePositionNone);
		if (position == Left)
		{
			GNode *sibling = GNodeGetRight(parent);
			Color siblingColor = Black;
			if (NULL != sibling)
			{
				siblingColor = GNodeGetColor(sibling);
			}
			
			if (siblingColor == Red)
			{
				// sibling is not NULL
				GNodeSetColor(parent, Red);
				GNodeSetColor(sibling, Black);
				GRBTreeRotateLeft(aTree, parent);
				
				// sibling can be NULL
				sibling = GNodeGetRight(parent);
				siblingColor = (sibling == NULL) ? Black : GNodeGetColor(sibling);
				assert(siblingColor == Black && GNodeGetColor(parent) == Red);
			}
			assert(siblingColor == Black);
			
			if (NULL == sibling)
			{
				node = parent;
			}
			else
			{
				Color siblingLeftChildColor = Black;
				if (GNodeGetLeft(sibling) != NULL)
				{
					siblingLeftChildColor = GNodeGetColor(GNodeGetLeft(sibling));
				}
				Color siblingRightChildColor = Black;
				if (GNodeGetRight(sibling) != NULL)
				{
					siblingRightChildColor = GNodeGetColor(GNodeGetRight(sibling));
				}
				
				if (siblingLeftChildColor == Black && siblingRightChildColor == Black)
				{
					GNodeSetColor(sibling, Red);
					node = parent;
				}
				else
				{
					if (siblingRightChildColor == Black)
					{
						// sibling's left child is not NULL
						GNodeSetColor(GNodeGetLeft(sibling), Black);
						GNodeSetColor(sibling, Red);
						GRBTreeRotateRight(aTree, sibling);
						
						// sibling is not NULL
						sibling = GNodeGetRight(parent);
						siblingColor = GNodeGetColor(sibling);
						
						// sibling's right child is not NULL
						siblingRightChildColor = GNodeGetColor(GNodeGetRight(sibling));
						siblingLeftChildColor = Black;
						if (GNodeGetLeft(sibling) != NULL)
						{
							siblingLeftChildColor = GNodeGetColor(GNodeGetLeft(sibling));
						}
					}
					assert((sibling != NULL && siblingColor == Black) &&
						   (GNodeGetRight(sibling) != NULL && siblingRightChildColor == Red));
					
					
					GNodeSetColor(sibling, GNodeGetColor(parent));
					GNodeSetColor(GNodeGetRight(sibling), Black);
					GNodeSetColor(parent, Black);
					GRBTreeRotateLeft(aTree, parent);
					node = GRBTreeGetRoot(aTree);
				}
			}
		}
		else if (position == Right)
		{
			GNode *sibling = GNodeGetLeft(parent);
			Color siblingColor = Black;
			if (NULL != sibling)
			{
				siblingColor = GNodeGetColor(sibling);
			}
			
			if (siblingColor == Red)
			{
				// sibling is not NULL
				GNodeSetColor(parent, Red);
				GNodeSetColor(sibling, Black);
				GRBTreeRotateRight(aTree, parent);
				
				// sibling can be NULL
				sibling = GNodeGetLeft(parent);
				siblingColor = (sibling == NULL) ? Black : GNodeGetColor(sibling);
				assert(siblingColor == Black && GNodeGetColor(parent) == Red);
			}
			assert(siblingColor == Black);
			
			if (NULL == sibling)
			{
				node = parent;
			}
			else
			{
				Color siblingRightChildColor = Black;
				if (GNodeGetRight(sibling) != NULL)
				{
					siblingRightChildColor = GNodeGetColor(GNodeGetRight(sibling));
				}
				Color siblingLeftChildColor = Black;
				if (GNodeGetLeft(sibling) != NULL)
				{
					siblingLeftChildColor = GNodeGetColor(GNodeGetLeft(sibling));
				}
				
				if (siblingLeftChildColor == Black && siblingRightChildColor == Black)
				{
					GNodeSetColor(sibling, Red);
					node = parent;
				}
				else
				{
					if (siblingLeftChildColor == Black)
					{
						// sibling's right child is not NULL
						GNodeSetColor(GNodeGetRight(sibling), Black);
						GNodeSetColor(sibling, Red);
						GRBTreeRotateLeft(aTree, sibling);
						
						assert(sibling == GNodeGetLeft(GNodeGetLeft(parent)));
						
						// sibling is not NULL
						sibling = GNodeGetLeft(parent);
						siblingColor = GNodeGetColor(sibling);
						
						// sibling's left child is not NULL
						siblingLeftChildColor = GNodeGetColor(GNodeGetLeft(sibling));
						siblingRightChildColor = Black;
						if (GNodeGetRight(sibling) != NULL)
						{
							siblingRightChildColor = GNodeGetColor(GNodeGetRight(sibling));
						}
						assert((sibling != NULL && siblingColor == Black) &&
							   (GNodeGetLeft(sibling) != NULL && siblingLeftChildColor == Red));

					}
					assert((sibling != NULL && siblingColor == Black) &&
						   (GNodeGetLeft(sibling) != NULL && siblingLeftChildColor == Red));
					
					GNodeSetColor(sibling, GNodeGetColor(parent));
					GNodeSetColor(GNodeGetLeft(sibling), Black);
					GNodeSetColor(parent, Black);
					GRBTreeRotateRight(aTree, parent);
					node = GRBTreeGetRoot(aTree);
				}
			}
		}
		
		parent = (node != NULL) ? GNodeGetParent(node) : NULL;
		position = (node != NULL) ? GNodePosition(node) : NodePositionNone;
		color = (node != NULL) ? GNodeGetColor(node) : ColorNone;
	}
	if (node != NULL)
 	{
		GNodeSetColor(node, Black);
	}
}

#pragma mark -

void GRBTreeTestStructure(GRBTree *aTree)
{    
    if (GRBTreeIsEmpty(aTree) == YES)
    {
		printf("Structure test - The tree is empty!\n");
        assert(GRBTreeGetRoot(aTree) == NULL);
    }
    else
    {
		GNodeResetBlackHeightCounter();
        GNodeTestStructure(GRBTreeGetRoot(aTree), 0);
    }
}

void GRBTreeDescription(GRBTree *aTree, void (* aNodeDataDescriptor)(void *))
{
    if (GRBTreeIsEmpty(aTree) == YES)
    {
        printf("Description - The tree is empty!\n");
    }
    else
    {
        GNodeDescription(GRBTreeGetRoot(aTree), aNodeDataDescriptor);    
    }
}

