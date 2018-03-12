//ksghoman@ucdavis.edu,Ghoman,Kirtanpal
//clcomrie@ucdavis.edu,Comrie,Cazamere 

#include <iostream>
#include <climits>
#include "InternalNode.h"

using namespace std;

InternalNode::InternalNode(int ISize, int LSize,
  InternalNode *p, BTreeNode *left, BTreeNode *right) :
  BTreeNode(LSize, p, left, right), internalSize(ISize)
  {
    keys = new int[internalSize]; // keys[i] is the minimum of children[i]
    children = new BTreeNode* [ISize];
  } // InternalNode::InternalNode()

  BTreeNode* InternalNode::addPtr(BTreeNode *ptr, int pos)
  { // called when original was full, pos is where the node belongs.
    if(pos == internalSize)
    return ptr;

    BTreeNode *last = children[count - 1];

    for(int i = count - 2; i >= pos; i--)
    {
      children[i + 1] = children[i];
      keys[i + 1] = keys[i];
    } // shift things to right to make room for ptr, i can be -1!

    children[pos] = ptr;  // i will end up being the position that it is inserted
    keys[pos] = ptr->getMinimum();
    ptr->setParent(this);
    return last;
  } // InternalNode:: addPtr()


  void InternalNode::addToLeft(BTreeNode *last)
  {
    ((InternalNode*)leftSibling)->insert(children[0]);

    for(int i = 0; i < count - 1; i++)
    {
      children[i] = children[i + 1];
      keys[i] = keys[i + 1];
    }

    children[count - 1] = last;
    keys[count - 1] = last->getMinimum();
    last->setParent(this);
    if(parent)
    parent->resetMinimum(this);
  } // InternalNode::ToLeft()


  void InternalNode::addToRight(BTreeNode *ptr, BTreeNode *last)
  {
    ((InternalNode*) rightSibling)->insert(last);
    if(ptr == children[0] && parent)
    parent->resetMinimum(this);
  } // InternalNode::addToRight()



  void InternalNode::addToThis(BTreeNode *ptr, int pos)
  {  // pos is where the ptr should go, guaranteed count < internalSize at start
    int i;

    for(i = count - 1; i >= pos; i--)
    {
      children[i + 1] = children[i];
      keys[i + 1] = keys[i];
    } // shift to the right to make room at pos

    children[pos] = ptr;
    keys[pos] = ptr->getMinimum();
    count++;
    ptr->setParent(this);

    if(pos == 0 && parent)
    parent->resetMinimum(this);
  } // InternalNode::addToThis()



  int InternalNode::getMaximum() const
  {
    if(count > 0) // should always be the case
    return children[count - 1]->getMaximum();
    else
    return INT_MAX;
  }  // getMaximum();


  int InternalNode::getMinimum()const
  {
    if(count > 0)   // should always be the case
    return children[0]->getMinimum();
    else
    return 0;
  } // InternalNode::getMinimum()


  InternalNode* InternalNode::insert(int value)
  {  // count must always be >= 2 for an internal node
    int pos; // will be where value belongs

    for(pos = count - 1; pos > 0 && keys[pos] > value; pos--);

    BTreeNode *last, *ptr = children[pos]->insert(value);
    if(!ptr)  // child had room.
    return NULL;

    if(count < internalSize)
    {
      addToThis(ptr, pos + 1);
      return NULL;
    } // if room for value

    last = addPtr(ptr, pos + 1);

    if(leftSibling && leftSibling->getCount() < internalSize)
    {
      addToLeft(last);
      return NULL;
    }
    else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < internalSize)
    {
      addToRight(ptr, last);
      return NULL;
    }
    else // both siblings full or non-existent
    return split(last);
  } // InternalNode::insert()


  void InternalNode::insert(BTreeNode *oldRoot, BTreeNode *node2)
  { // Node must be the root, and node1
    children[0] = oldRoot;
    children[1] = node2;
    keys[0] = oldRoot->getMinimum();
    keys[1] = node2->getMinimum();
    count = 2;
    children[0]->setLeftSibling(NULL);
    children[0]->setRightSibling(children[1]);
    children[1]->setLeftSibling(children[0]);
    children[1]->setRightSibling(NULL);
    oldRoot->setParent(this);
    node2->setParent(this);
  } // InternalNode::insert()


  void InternalNode::insert(BTreeNode *newNode)
  { // called by sibling so either at beginning or end
    int pos;

    if(newNode->getMinimum() <= keys[0]) // from left sibling
    pos = 0;
    else // from right sibling
    pos = count;

    addToThis(newNode, pos);
  } // InternalNode::insert(BTreeNode *newNode)


  void InternalNode::print(Queue <BTreeNode*> &queue)
  {
    int i;

    cout << "Internal: ";
    for (i = 0; i < count; i++)
    cout << keys[i] << ' ';
    cout << endl;

    for(i = 0; i < count; i++)
    queue.enqueue(children[i]);

  } // InternalNode::print()

  void InternalNode::deleteKey(int position)
  {
    for(int i = position; i<count; i++)
    {
      keys[i]= keys[i+1];
    }
    count--; // decrease the count of internal node values
  }

  BTreeNode* InternalNode::remove(int value)
  {  // to be written by students
    int pos; // find the postion of the value in the internal node
    for(pos = count - 1; pos >= 0 && keys[pos] > value; pos--); // find the location of the calue to be deleted
    BTreeNode *ptr = children[pos]->remove(value); //remove the value in the given child

    if(!ptr) return NULL; // filler for stub

    else // if children merge then we will need to remove the children at the pointer ptr
    {
      BTreeNode **temp = new BTreeNode* [count]; // create a temporary BTreeNode to make a copy of children

      for(int i =count-1; i > pos; i--) // copy all the children upto to the one need to be deleted
      {
        temp[i]= children[i];
      }
      for(int i =pos-1; i >=0; i--) //copy all the rest of the children leaving the deleting one out
      {
        temp[i+1]= children[i];
      }
      delete [] children;     // deallocate children space
      children = new BTreeNode* [count]; // recereate children
      for(int i =0; i < count-1; i++)
      children[i]=temp[i+1];  // copy all the children from the temp copy back to the children BTreeNode

      delete [] temp; // delete temporary

      deleteKey(pos); // delete the key value for the deleted children

      // Now we need to check if after deleting the key, the InternalNode must be half full
      // If not then either borrow form sibling or merge with it

      if(count < (internalSize+1)/2)  // if size is less than the half
      {
        if( ((InternalNode*)leftSibling) && ((InternalNode*)leftSibling)->getCount() > (internalSize+1)/2)
        {   // if there is leftSibling and it has extra child, then borrow from it
          int leftCount = ((InternalNode*)leftSibling)->getCount();   //left sibling count
          insert( ((InternalNode*)leftSibling)->children[leftCount-1]); // copy maximum value child of the left sibling
          ( ((InternalNode*)leftSibling)->count )--; // decrement the counter of the left sibling
          if( (((InternalNode*)rightSibling)->parent) ) // reset siblings parent's minimum
          (((InternalNode*)rightSibling)->parent)->resetMinimum((InternalNode*)rightSibling);

          if(parent)
          parent->resetMinimum(this);
          return NULL;
        }

        else if( ((InternalNode*)leftSibling) )
        { // if leftSibling cannot lend then merge with it
          for(int i=0; i<count; i++)
          {
            ((InternalNode*)leftSibling)->insert(children[i]); // insert all the children to leftSibling
          }
          if( ((InternalNode*)rightSibling) )
          {	//set the pointer of the left siblings rightsibling to deleting leaf's right sibling
          ((InternalNode*)leftSibling)->setRightSibling((InternalNode*)rightSibling);
        }
        else ((InternalNode*)leftSibling)->setRightSibling(NULL);
        return this; // return this pointer so that its parent can delete it
      }

      // else check right if the right sibling is 1 more than the half then borrow
      else if( ((InternalNode*)rightSibling) && ((InternalNode*)rightSibling)->getCount() > (internalSize+1)/2)
      { // if no left sibling then borrow from leftSibling
        int rightCount = ((InternalNode*)rightSibling)->getCount();
        insert( ((InternalNode*)rightSibling)->children[0]);
        for(int i = 0; i< rightCount-1;i++)
        {
          ( ((InternalNode*)rightSibling)->keys[i] ) = ( ((InternalNode*)rightSibling)->keys[i+1] ); // shift rightSibling's keys
          (((InternalNode*)rightSibling)->children[i] ) = ( ((InternalNode*)rightSibling)->children[i+1] ); // shift rightSibling's children
        }
        ( ((InternalNode*)rightSibling)->count )--;
        if( (((InternalNode*)rightSibling)->parent) )  // reset rightSibling parent's minimum
        (((InternalNode*)rightSibling)->parent)->resetMinimum((InternalNode*)rightSibling);

        if(parent)
        parent->resetMinimum(this);
        return NULL;
      }

      else if( ((InternalNode*)rightSibling) )
      { // if cannot borrow from right sibling then merge with it
        for(int i=0; i<count; i++)
        {       //merge leaf with the right sibling
          ((InternalNode*)rightSibling)->insert(children[i]);
        }
        if( ((InternalNode*)leftSibling) )
        {       //set the pointer of the left siblings rightsibling to deletin    g leaf's right sibling
        ((InternalNode*)rightSibling)->setLeftSibling((InternalNode*)leftSibling);
      }
      else{
        ((InternalNode*)rightSibling)->setLeftSibling(NULL);

      }
      return this;
    }
  }

  if(count == 1) //if only one key left in the internal node and there are no siblings then return pointer to the leaf
  {
    return children[0];
  }

  if(parent)  parent->resetMinimum(this);
  return NULL;

}
} // InternalNode::remove(int value)


void InternalNode::resetMinimum(const BTreeNode* child)
{
  for(int i = 0; i < count; i++)
  if(children[i] == child)
  {
    keys[i] = children[i]->getMinimum();
    if(i == 0 && parent)
    parent->resetMinimum(this);
    break;
  }
} // InternalNode::resetMinimum()


InternalNode* InternalNode::split(BTreeNode *last)
{
  InternalNode *newptr = new InternalNode(internalSize, leafSize,
    parent, this, rightSibling);


    if(rightSibling)
    rightSibling->setLeftSibling(newptr);

    rightSibling = newptr;

    for(int i = (internalSize + 1) / 2; i < internalSize; i++)
    {
      newptr->children[newptr->count] = children[i];
      newptr->keys[newptr->count++] = keys[i];
      children[i]->setParent(newptr);
    }

    newptr->children[newptr->count] = last;
    newptr->keys[newptr->count++] = last->getMinimum();
    last->setParent(newptr);
    count = (internalSize + 1) / 2;
    return newptr;
  } // split()
