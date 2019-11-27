#ifndef _BTREE_
#define _BTREE_
#ifndef NULL
#define NULL 0
#endif // !NULL
enum _BOOL_VALUE {FALSE,TRUE};
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
  int m_m;
  _CMP(*m_cmp)(const ElementType&, const ElementType&);//�ȽϺ���
  TreeNode<ElementType>* m_head;
  int m_size;
  TreeNode<ElementType>* m_nil;
  void freeNode(TreeNode<ElementType>*);
  BOOL insert(KeyNode<ElementType>*);
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
  m_m = _m;
  m_cmp = _cmp;
  m_size = 0;
  m_nil = NULL;
  m_head = new TreeNode<ElementType>;
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
  return m_size;
}
template <typename ElementType>
void BTree<ElementType>::destory() {
  freeNode(m_head);
}
template <typename ElementType>
TreeNode<ElementType>* BTree<ElementType>::getHead() {
  return m_head;
}
template <typename ElementType>
KeyNode<ElementType>* BTree<ElementType>::get(const ElementType& element) {
  TreeNode<ElementType>* h = m_head->child;
  KeyNode<ElementType>* k = m_head->key;

  while (1) {
    if (m_cmp(element, k->value) == EQUAL) {
      return k;
    }
    else if (m_cmp(element, k->value) == SMALLER) {
      if (h == NULL) {
        return NULL;
      }
      k = h->key;
      h = h->child;
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
        h = h == NULL ? NULL : h->next;
      }   
    }
  }
}
//���½ڵ���뵽Ҷ�ӽڵ㣬���ɹ����뷵��TRUE��node���䣬���Ѵ��ڸýڵ㣬����FALSE��
//node����Ϊ�����Ѵ��ڵĽ���ַ
template <typename ElementType>
BOOL BTree<ElementType>::insert(KeyNode<ElementType>* node) {
  TreeNode<ElementType>* h = m_head->child;
  KeyNode<ElementType>* k = m_head->key;
  KeyNode<ElementType>* kpre = NULL;
  ElementType element = node->value;
  node->p = m_head;
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
    if (m_cmp(element, k->value) == EQUAL) {
      k->value = node->value;
      delete node;
      node = NULL;
      m_size--;
      node=k;
      return FALSE;
    }
    else if (m_cmp(element, k->value) == SMALLER) {
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
  return TRUE;
}
template <typename ElementType>
KeyNode<ElementType>* BTree<ElementType>::put(ElementType element) {
  m_size++;
  KeyNode<ElementType>* node = new KeyNode<ElementType>;
  node->value = element;
  if (!insert(node)) {
    return node;
  }
  TreeNode<ElementType>* s = node->p;//��ǰ���
  TreeNode<ElementType>* sp;//��ǰ��㸸���
  TreeNode<ElementType>* temp;//���Ѻ���½ڵ�
  KeyNode<ElementType>* kt;//ȡ����ֵ���
  TreeNode<ElementType>* tt;//��Ҫ�Ͽ����ӽڵ�
  while (1) {
    //�ؼ�����С��m
    if (s->size < m_m) {
      break;
    }
    //�ؼ���������m-1,���Ѳ���
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
    //�����ѳ����½����뵽���ڵ�ĺ�������
    temp->next = s->next;
    temp->size = m_m - 1 - mid;
    s->next = temp;
    s->size = mid;
    KeyNode<ElementType>* kpre=NULL;
    //�����ǰ���Ϊ���ڵ�
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
      m_head = newhead;
      break;
    }
    //���������key���뵽���ڵ��Key����
    k = sp->key;
    while (1) {
      if (k == NULL) {
        kpre->next = kt;
        break;
      }
      if (m_cmp(kt->value, k->value) == SMALLER) {
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
  m_size--;
  //����ɾ������λ��
  KeyNode<ElementType>* kn = get(element);
  if (kn == NULL) {
    m_size++;
    return;
  }
  int min;
  if (m_m % 2 == 0) {
    min = m_m / 2 - 1;
  }
  else {
    min = m_m / 2;
  }
  //�ҵ�����ڵ㣬ֱ�Ӻ��
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
  lbro:���ֵ� rbro�����ֵ� rtp:�����
  */
  TreeNode<ElementType>* lbro, * rbro, * rtp;
  TreeNode<ElementType>* lchild, * rchild;
  KeyNode<ElementType>* kpre=NULL;
  TreeNode<ElementType>* tt;
  while (1) {
    //��ǰ��㳤�ȷ���Ҫ��
    if (rt->size >= min) {
      return;
    } 
    
    //��ǰ���Ϊͷ�ڵ�
    if (rt == m_head) {
      //ͷ�ڵ�գ��滻ͷ�ڵ�
      if (rt->size == 0) {
        m_head = rt->child;
        delete rt;
        rt = NULL;
      }
      return;
    }
    rtp = rt->parent;
    tt = rtp->child;
    rbro = rt->next;
    lbro = rtp->child;
    //�ҵ��ֵܽ��
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
    //���ֵܽ�㲻Ϊ���ҿ��Խ�
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
    //���ֵܽ�㲻Ϊ���ҿ��Խ�
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
    //���ɽ裬�����ϲ�����Ӧ��keyֵ����ϲ���Ľ�㣬�����ɾ����key�����������Ϊ��ǰ���������ϵ���
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

