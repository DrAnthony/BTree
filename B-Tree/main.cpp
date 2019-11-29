#include <iostream>
#include "_BTree.h"
using namespace std;

_CMP cmp(const int& a,const int& b) {
  if (a < b) {
    return SMALLER;
  }
  else if (a > b) {
    return BIGGER;
  }
  else {
    return EQUAL;
  }
}
void inorder(TreeNode<int>* h,int level) {
  if (h == NULL) {
    return;
  }
  KeyNode<int>* k = h->key;
  TreeNode<int>* child=h->child;
  cout << "level:" << level << " ";
  while (k != NULL) {
    cout << k->value << " ";
    k = k->next;
  }
  cout << endl;
  while (child != NULL) {
    inorder(child,level+1);
    child = child->next;
  }
}

void set(int** a) {
  *a = new int[3];
  (*a)[0] = 1;
  (*a)[1] = 2;
  (*a)[2] = 3;
}
int main() {
  int map[] = {
    5,6,1,4,5,14,52,17,84,100,26,58,95,47,
    1,4,5,3,0,48,78,95,65,14,789,145,14,
    17,87,25,26,111,201,102,458,5555,65,14,
    230,-1,549,777,1024,256,2048,10001,10002,10003,
    10004,10005,10006,10007
  };
  int len = sizeof(map)/sizeof(int);
  BTree<int> tree;
  tree.init(3, &cmp);
  for (int i = 0; i < len; i++) {
    tree.put(map[i]);
  }
  inorder(tree.getHead(),0);
  cout << "+++++++++++++++" << endl;
  tree.remove(6);
  inorder(tree.getHead(), 0);
  cout << "+++++++++++++++" << endl;
  tree.remove(14);
  inorder(tree.getHead(), 0);
  cout << "+++++++++++++++" << endl;
  tree.remove(17);
  inorder(tree.getHead(), 0);
  cout << "+++++++++++++++" << endl;
  tree.remove(789);
  inorder(tree.getHead(), 0);

  return 0;
}