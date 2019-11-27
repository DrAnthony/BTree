#ifndef _BTREE_
#define _BTREE_
#ifndef NULL
#define NULL 0
#endif // !NULL
enum BOOL_VALUE {FALSE,TRUE};
enum _CMP {BIGGER,EQUAL,SMALLER};
typedef short BOOL;

template <typename ElementType>
struct KeyNode;

template <typename ElementType>
struct TreeNode {
  int size=0;
  KeyNode<ElementType>* key = NULL;
  TreeNode<ElementType>* next = NULL;
  TreeNode<ElementType>* child = NULL;
  TreeNode<ElementType>* parent = NULL;
};

template <typename ElementType>
struct KeyNode {
  ElementType value;
  TreeNode<ElementType>* p = NULL;
  KeyNode<ElementType>* next=NULL;
};

template <typename ElementType>
class BTree {
private:
  int m;
  _CMP(*cmp)(const ElementType&, const ElementType&);
  TreeNode<ElementType>* head;
  int len;
  void freeNode(TreeNode<ElementType>*);
  TreeNode<ElementType>* nil;
public:
  BTree(int,_CMP(*_cmp)(const ElementType& e1,const ElementType& e2));
  ~BTree();
  TreeNode<ElementType>* getHead();
  KeyNode<ElementType>* get(const ElementType&);
  KeyNode<ElementType>* put(ElementType);
  void remove(const ElementType);
  int length();
  void destory();
};

template <typename ElementType>
BTree<ElementType>::BTree(int _m, _CMP(*_cmp)(const ElementType& e1, const ElementType& e2)) {
  m = _m;
  cmp = _cmp;
  len = 0;
  nil = NULL;
  head = new TreeNode<ElementType>;
}
template <typename ElementType>
BTree<ElementType>::~BTree() {
  destory();
}
template <typename ElementType>
void BTree<ElementType>::freeNode(TreeNode<ElementType>* node) {
  if (node == NULL) {
    return;
  }
  KeyNode<ElementType>* key = node->key;
  KeyNode<ElementType>* kn;
  TreeNode<ElementType>* child = node->child;
  TreeNode<ElementType>* next = node->next;
  delete node;
  node = NULL;
  while (key != NULL) {
    kn = key->next;
    delete key;
    key = NULL;
    key = kn;
  }
  freeNode(next);
  freeNode(child);
}
template <typename ElementType>
int BTree<ElementType>::length() {
  return len;
}
template <typename ElementType>
void BTree<ElementType>::destory() {
  freeNode(head);
}
template <typename ElementType>
TreeNode<ElementType>* BTree<ElementType>::getHead() {
  return head;
}
template <typename ElementType>
KeyNode<ElementType>* BTree<ElementType>::get(const ElementType& element) {
  TreeNode<ElementType>* h = head->child;
  KeyNode<ElementType>* k = head->key;

  while (1) {
    if (cmp(element, k->value) == EQUAL) {
      return k;
    }
    else if (cmp(element, k->value) == SMALLER) {
      if (h == NULL) {
        return NULL;
      }
      else {
        k = h->key;
        h = h->child;
      }
    }
    else {
      if (k->next == NULL) {
        if (h == NULL) {
          return NULL;
        }
        k = h->next->key;
        h = h->next->child;
      }
      else {
        k = k->next;
        if (h != NULL) {
          h = h->next;
        } 
      }   
    }
  }
}
template <typename ElementType>
KeyNode<ElementType>* BTree<ElementType>::put(ElementType element) {
  len++;
  KeyNode<ElementType>* node = new KeyNode<ElementType>;
  node->value = element;

  TreeNode<ElementType>* h = head->child;
  KeyNode<ElementType>* k = head->key;
  KeyNode<ElementType>* kpre = NULL;
  node->p = head;
  while (1) {
    if (k == NULL) {
      if (h == NULL) {
        if (kpre != NULL) {
          kpre->next = node;
        }
        else {
          node->p->key = node;
        }
        node->next = k;
        node->p->size++;
        break;
      }
      k = h->key;
      kpre = NULL;
      node->p = h;
      h = h->child;
    }
    if (cmp(element, k->value) == EQUAL) {
      k->value = node->value;
      delete node;
      node = NULL;
      len--;
      return k;
    }
    else if (cmp(element, k->value) == SMALLER) {
      if (h == NULL) {
        node->p = k->p;
        if (kpre != NULL) {
          kpre->next = node;
        }
        else {
          k->p->key = node;
        }
        node->next = k;
        node->p->size++;
        break;
      }
      else {
        k = h->key;
        kpre = NULL;
        node->p = h;
        h = h->child;
      }
    }
    else {
      if (h != NULL) {
        h = h->next;
      }
      kpre = k;
      k = k->next;
    }
  }
  TreeNode<ElementType>* s = node->p;//当前结点
  TreeNode<ElementType>* sp;//当前结点父结点
  TreeNode<ElementType>* temp;//分裂后的新节点
  KeyNode<ElementType>* kt;//取出的值结点
  TreeNode<ElementType>* tt;//需要断开的子节点
  while (1) {
    //关键字数小于m
    if (s->size < m) {
      break;
    }
    //关键字数超过m-1,分裂操作
    sp = s->parent;
    temp = new TreeNode<ElementType>;
    temp->parent = sp;
    int mid = s->size / 2;
    kt = s->key;
    tt = s->child;
    for (int i = 1; i < mid; i++) {
      kt = kt->next;
      if (tt != NULL) {
        tt = tt->next;
      }
    }
    if (tt != NULL) {
      tt = tt->next;
      temp->child = tt->next;
      tt->next = NULL;
      TreeNode<ElementType>* t = temp->child;
      while (t != NULL) {
        t->parent = temp;
        t = t->next;
      }
    }
    KeyNode<ElementType>* k = kt;
    kt = k->next;
    k->next = NULL;
    k = kt->next;
    temp->key = k;
    while (k != NULL) {
      k->p = temp;
      k = k->next;
    }
    kt->next = NULL;
    kt->p = sp;
    //将分裂出的新结点插入到父节点的孩子链表
    temp->next = s->next;
    temp->size = m - 1 - mid;
    s->next = temp;
    s->size = mid;
    KeyNode<ElementType>* kpre=NULL;
    

    //如果当前结点为根节点
    if (sp == NULL) {
      TreeNode<ElementType>* newhead = new TreeNode<ElementType>;
      newhead->size = 1;
      newhead->key = kt;
      kt->p = newhead;
      newhead->child = s;
      while (s != NULL) {
        s->parent = newhead;
        s = s->next;
      }
      head = newhead;
      break;
    }
    //将分类出的key插入到父节点的Key链表
    k = sp->key;
    while (1) {
      if (k == NULL) {
        kpre->next = kt;
        break;
      }
      if (cmp(kt->value, k->value) == SMALLER) {
        kt->next = k;
        if (kpre == NULL) {
          sp->key = kt;
        }
        else {
          kpre->next = kt;
        }
        break;
      }
      else {
        kpre = k;
        k = k->next;
      }
    }
    sp->size++;
    s = sp;
  }
  return node;
}
template <typename ElementType>
void BTree<ElementType>::remove(const ElementType element) {
  len--;
  //查找删除结点的位置
  KeyNode<ElementType>* kn = get(element);
  if (kn == NULL) {
    len++;
    return;
  }
  int min;
  if (m % 2 == 0) {
    min = m / 2 - 1;
  }
  else {
    min = m / 2;
  }
  //找到替代节点，直接后继
  TreeNode<ElementType>* rt = kn->p;;
  KeyNode<ElementType>* ktemp = rt->key;
  if (kn->p->child != NULL) {
    rt = rt->child;
    while (ktemp != kn) {
      ktemp = ktemp->next;
      rt = rt->next;
    }
    rt = rt->next;
    while (rt->child != NULL) {
      rt = rt->child;
    }
    ktemp = rt->key;
    kn->value = ktemp->value;
    kn = ktemp;
  }

  if (kn == rt->key) {
    rt->key = kn->next;
  }
  else {
    ktemp = rt->key;
    while (ktemp->next != kn) {
      ktemp = ktemp->next;
    }
    ktemp->next = kn->next;
  }
  rt->size--;
  delete kn;
  kn = NULL;
  
  /*
  lbro:左兄弟 rbro：右兄弟 rtp:父结点
  */
  TreeNode<ElementType>* lbro, * rbro, * rtp;
  TreeNode<ElementType>* lchild, * rchild;
  KeyNode<ElementType>* kpre=NULL;
  TreeNode<ElementType>* tt;
  while (1) {
    //当前结点长度符合要求
    if (rt->size >= min) {
      return;
    } 
    
    //当前结点为头节点
    if (rt == head) {
      //头节点空，替换头节点
      if (rt->size == 0) {
        head = rt->child;
        delete rt;
        rt = NULL;
      }
      return;
    }
    rtp = rt->parent;
    tt = rtp->child;
    rbro = rt->next;
    lbro = rtp->child;
    //找到兄弟结点
    if (lbro == rt) {
      lbro = NULL;
    }
    else {
      while (lbro->next != rt) {
        lbro = lbro->next;
      }
    }
    kpre = NULL;
    tt = rtp->child;
    //右兄弟结点不为空且可以借
    if (rbro != NULL && rbro->size > min) {
      lchild = rt->child;
      rchild = rbro->child;
      if (tt == rt) {
        ktemp = rtp->key;
        rtp->key = rbro->key;
        rtp->key->p = rtp;
        rbro->key = rtp->key->next;
        rtp->key->next = ktemp->next;
        kpre = rt->key;
        if (kpre == NULL) {
          rt->key = ktemp;
        }
        else {
          while (kpre->next != NULL) {
            kpre = kpre->next;
          }
          kpre->next = ktemp;
        }    
        ktemp->p = rt;
      }
      else {
        kpre = rtp->key;
        while (tt->next != rt) {
          tt = tt->next;
          kpre = kpre->next;
        }
        ktemp = kpre->next;
        rbro->key->p = kpre->p;
        kpre->next = rbro->key;
        rbro->key = rbro->key->next;
        kpre->next->next = ktemp->next;
        kpre = rt->key;
        if (kpre == NULL) {
          rt->key = ktemp;
        }
        else {
          while (kpre->next != NULL) {
            kpre = kpre->next;
          }
          kpre->next = ktemp;
        }  
        ktemp->p = rt;
        ktemp->next = NULL;
      }
      if (lchild != NULL) {
        while (lchild->next != NULL)
        {
          lchild = lchild->next;
        }
        lchild->next = rchild;
        rbro->child = rchild->next;
        rchild->next = NULL;
        rchild->parent = rt;
      }
      rt->size++;
      rbro->size--;
      return;
    }
    //左兄弟结点不为空且可以借
    else if (lbro != NULL && lbro->size > min) {
      lchild = lbro->child;
      rchild = rt->child;
      if (tt == lbro) {
        ktemp = lbro->key;
        kpre = NULL;
        while (ktemp->next != NULL) {
          kpre = ktemp;
          ktemp = ktemp->next;
        }
        kpre->next = NULL;
        kpre = rtp->key;
        rtp->key = ktemp;
        ktemp->p = rtp;
        ktemp->next = kpre->next;
        kpre->p = rt;
        kpre->next = rt->key;
        rt->key = kpre;
      }
      else {
        kpre = rtp->key;
        while (tt->next != lbro) {
          tt = tt->next;
          kpre = kpre->next;
        }
        ktemp = kpre->next;
        kpre->next = ktemp->next;
        ktemp->p = rt;
        ktemp->next = rt->key;
        rt->key = ktemp;
        ktemp = lbro->key;
        KeyNode<ElementType>* kkt = NULL;
        while (ktemp->next != NULL) {
          kkt = ktemp;
          ktemp = ktemp->next;
        }
        kkt->next = NULL;
        ktemp->p = rtp;
        ktemp->next = kpre->next;
        kpre->next = ktemp;
      }
      if (lchild != NULL) {
        while (lchild->next->next != NULL)
        {
          lchild = lchild->next;
        }
        lchild->next->next = rchild;
        rt->child = lchild->next;
        lchild->next->parent = rt;
        lchild->next = NULL;
      }
      rt->size++;
      lbro->size--;
      return;
    }
    //不可借，两结点合并，对应的key值加入合并后的结点，父结点删除该key，将父结点作为当前结点继续向上调节
    else {
      TreeNode<ElementType>* l, * r;
      if (rbro != NULL) {
        l = rt; r = rbro;
      }
      else {
        l = lbro; r = rt;
      }
      lchild = l->child;
      rchild = r->child;
      if (tt == l) {
        ktemp = rtp->key;
        rtp->key = ktemp->next;
        l->size=l->size+ 1 + r->size;
        l->next = r->next;
        ktemp->p = l;
        kpre = l->key;
        if (kpre != NULL) {
          while (kpre->next != NULL) {
            kpre = kpre->next;
          }
          kpre->next = ktemp;
          ktemp->next = r->key;
          ktemp = ktemp->next;
        }
        else {
          l->key = ktemp;
          ktemp->next= r->key;
          ktemp = ktemp->next;
        }
        while (ktemp != NULL) {
          ktemp->p = l;
          ktemp = ktemp->next;
        }
        if (lchild != NULL) {
          while (lchild->next!=NULL)
          {
            lchild = lchild->next;
          }
          lchild->next = rchild;
          lchild = lchild->next;
          while (lchild != NULL)
          {
            lchild->parent = l;
            lchild = lchild->next;
          }
        }
      }
      else {
        kpre = rtp->key;
        while (tt->next != l) {
          tt = tt->next;
          kpre = kpre->next;
        }
        ktemp = kpre->next;
        kpre->next = ktemp->next;
        kpre = l->key;
        l->size =l->size+ 1 + r->size;
        l->next = r->next;
        ktemp->p = l;
        if (kpre == NULL) {
          l->key = r->key;
          ktemp = l->key;
        }
        else {
          while (kpre->next != NULL) {
            kpre = kpre->next;
          }
          kpre->next = ktemp;
          ktemp->next = r->key;
          ktemp = ktemp->next;
        }    
        while (ktemp != NULL) {
          ktemp->p = l;
          ktemp = ktemp->next;
        }
        if (lchild != NULL) {
          while (lchild->next != NULL)
          {
            lchild = lchild->next;
          }
          lchild->next = rchild;
          lchild = lchild->next;
          while (lchild != NULL)
          {
            lchild->parent = l;
            lchild = lchild->next;
          }
        }
      }
      rtp->size--;
      delete r;
      r = NULL;
      rt = rtp;
    }
  } 
}
#endif // !_BTREE_

