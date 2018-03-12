//ksghoman@ucdavis.edu,Ghoman,Kirtanpal
//clcomrie@ucdavis.edu,Comrie,Cazamere 

#ifndef LeafNodeH
#define LeafNodeH

#include "BTreeNode.h"

class LeafNode:public BTreeNode
{
  int *values;
public:
  LeafNode(int LSize, InternalNode *p, BTreeNode *left,
    BTreeNode *right);
  void addToLeft(int last);
  void addToRight(int value, int last);
  void addToThis(int value);
  void addValue(int value, int &last);
  int getMaximum() const;
  int getMinimum() const;
  LeafNode* remove(int value); // NULL == no merge
  void deleteThis(int valuei, int &deleted);  // delete the value passed in and tells if the value was deleted
  LeafNode* insert(int value);
  //void insert(BTreeNode* newNode);
  void print(Queue <BTreeNode*> &queue);
  LeafNode* split(int value, int last);

}; //LeafNode class

#endif
