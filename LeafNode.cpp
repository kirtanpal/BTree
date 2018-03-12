//ksghoman@ucdavis.edu,Ghoman,Kirtanpal
//clcomrie@ucdavis.edu,Comrie,Cazamere 

#include <iostream>
#include <climits>
#include "LeafNode.h"
#include "InternalNode.h"
#include "QueueAr.h"

using namespace std;


LeafNode::LeafNode(int LSize, InternalNode *p,
  BTreeNode *left, BTreeNode *right) : BTreeNode(LSize, p, left, right)
  {
    values = new int[LSize];
  }  // LeafNode()


  void LeafNode::addToLeft(int last)
  {
    leftSibling->insert(values[0]);

    for(int i = 0; i < count - 1; i++)
    values[i] = values[i + 1];

    values[count - 1] = last;
    if(parent)
    parent->resetMinimum(this);
  } // LeafNode::ToLeft()


  void LeafNode::addToRight(int value, int last)
  {
    rightSibling->insert(last);

    if(value == values[0] && parent)
    parent->resetMinimum(this);
  } // LeafNode::addToRight()


  void LeafNode::addToThis(int value)
  {
    int i;

    for(i = count - 1; i >= 0 && values[i] > value; i--)
    values[i + 1] = values[i];

    values[i + 1] = value;
    count++;

    if(value == values[0] && parent)
    parent->resetMinimum(this);
  } // LeafNode::addToThis()


  void LeafNode::addValue(int value, int &last) // this function finds the largest value from the leaf and the value to be added and adjust the leaf and return the largest value to be taken care
  {
    int i;

    if(value > values[count - 1])
    last = value;
    else
    {
      last = values[count - 1];

      for(i = count - 2; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];
      // i may end up at -1
      values[i + 1] = value;
    }
  } // LeafNode:: addValue()


  int LeafNode::getMaximum()const
  {
    if(count > 0)  // should always be the case
    return values[count - 1];
    else
    return INT_MAX;
  } // getMaximum()


  int LeafNode::getMinimum()const
  {
    if(count > 0)  // should always be the case
    return values[0];
    else
    return 0;

  } // LeafNode::getMinimum()


  LeafNode* LeafNode::insert(int value)
  {
    int last;

    if(count < leafSize)
    {
      addToThis(value);
      return NULL;
    } // if room for value

    addValue(value, last);

    if(leftSibling && leftSibling->getCount() < leafSize)
    {
      addToLeft(last);
      return NULL;
    }  // if can insert into leftSibling
    else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < leafSize)
    {
      addToRight(value, last);
      return NULL;
    }  // if can insert into rightSibling
    else // both siblings full or non-existent
    return split(value, last);
  }  // LeafNode::insert()

  void LeafNode::print(Queue <BTreeNode*> &queue)
  {
    cout << "Leaf: ";
    for (int i = 0; i < count; i++)
    cout << values[i] << ' ';
    cout << endl;
  } // LeafNode::print()


  LeafNode* LeafNode::remove(int value)
  {   // To be written by students
    int deleted;
    deleteThis(value,deleted); // delete the value if it present and then we will take care of rest of the issues if arrise because of deletion
    // count is already decremented by deleteThis
    if(deleted) // checks for the case if value was not preset (error case)
    {

      if( (count >= (leafSize+1)/2) ) // if leaf is at least half full then return null and reset parent's minimum
      {
        if( parent)
        {
          parent->resetMinimum(this);
        }
        return NULL;
      }

      else
      {
        // if the left sibling is 1 more than the half then borrow
        if(leftSibling && leftSibling->getCount() > (leafSize+1)/2)
        {
          insert(leftSibling->getMaximum()); // borrow maximum value
          leftSibling->remove(values[0]); // remove borrowed value from its sibling
          return NULL;
        }

        else if(leftSibling)
        { // if cannot borrow then merge with it
          for(int i=0; i<count; i++)
          {	//merge leaf with the left sibling
            leftSibling->insert(values[i]);
          }
          if(rightSibling)
          {	//set the pointer of the left siblings rightsibling to deleting leaf's right sibling
          leftSibling->setRightSibling(rightSibling);
        }
        else leftSibling->setRightSibling(NULL);
        return this; // return pointer so that it can be deleted
      }

      // else check right if the right sibling is 1 more than the half then borrow
      else if(rightSibling && rightSibling->getCount() > (leafSize+1)/2)
      { // if cannot borrow or merge with leftSibling then borrow from rightsibling
        insert(rightSibling->getMinimum());  // get its minimum value
        rightSibling->remove(values[count-1]);
        return NULL;
      }

      else if(rightSibling)
      { // if cannot borrow from rightsibling then merge with it
        for(int i=0; i<count; i++)
        {       //merge leaf with the right sibling
          rightSibling->insert(values[i]);
        }
        if(leftSibling)
        {       //set the pointer of the left siblings rightsibling to deletin    g leaf's right sibling
        rightSibling->setLeftSibling(leftSibling);
      }
      else rightSibling->setLeftSibling(NULL);

      return this;
    }

  }//else
} //if deleted
return NULL;  // filler for stub
}  // LeafNode::remove()


//************************
// deleteThis finds the value passed in and delete it and set deleted =1
// else it deleted = 0
// deleted variable keeps tack if the value passed in is not found and not deleted
// ***********************
//
void LeafNode::deleteThis(int value, int &deleted)
{
  int j;
  for(j = count - 1; j >= 0 && values[j]!=value; j--){}  // find the location of given value in the leaf
  if (value == values[j])
  {
    for(int k = j; k<count-1; k++)
    {
      values[k] = values[k+1];  // delete the value by replacing
    }
    count--;
    deleted = 1;
  }//if ==
  else deleted = 0;
}

LeafNode* LeafNode::split(int value, int last)
{
  LeafNode *ptr = new LeafNode(leafSize, parent, this, rightSibling);


  if(rightSibling)
  rightSibling->setLeftSibling(ptr);

  rightSibling = ptr;

  for(int i = (leafSize + 1) / 2; i < leafSize; i++)
  ptr->values[ptr->count++] = values[i];

  ptr->values[ptr->count++] = last;
  count = (leafSize + 1) / 2;

  if(value == values[0] && parent)
  parent->resetMinimum(this);
  return ptr;
} // LeafNode::split()
