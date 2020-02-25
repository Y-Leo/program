#pragma once

#include <iostream>
#include <queue>
#include <vector>

template<class W>
struct HuffManTreeNode
{
    //节点构造
    HuffManTreeNode(const W& weight = W())
        :_pLeft(nullptr)
         ,_pRight(nullptr)
         ,_pParent(nullptr)
         ,_weight(weight)
    {}

   HuffManTreeNode<W>* _pLeft;
   HuffManTreeNode<W>* _pRight;
   HuffManTreeNode<W>* _pParent;
   W _weight; //节点的权值
};

template<class W>
class Less
{
   typedef HuffManTreeNode<W> Node;
public:
   bool operator()(const Node* pLeft,const Node* pRight)
   {
       return pLeft->_weight > pRight->_weight;
   }
};

template<class W>
class HuffManTree
{
   typedef HuffManTreeNode<W> Node;
public:
   //无参构造
   HuffManTree()
       :_pRoot(nullptr)
   {}
   //带参构造
   HuffManTree(const std::vector<W> vWeight,const W& invalid)
   {
       CreateHuffManTree(vWeight,invalid);
   }

   //析构
   ~HuffManTree()
   {
       _DestroyTree(_pRoot);
   }

   Node* GetRoot()
   {
       return _pRoot;
   }

   void CreateHuffManTree(const std::vector<W>& vWeight, const W& invalid)
   {
       //1、构建森林
       std::priority_queue<Node*, std::vector<Node*>, Less<W>> q;
       for(auto e : vWeight)
       {
        if(e == invalid)
            continue;
         q.push(new Node(e));
       }

       //2、
       while(q.size() > 1)
       {
           Node* pLeft = q.top();
           q.pop();

           Node* pRight = q.top();
           q.pop();

           Node* pParent = new Node(pLeft->_weight + pRight->_weight);
           pParent->_pLeft = pLeft;
           pParent->_pRight = pRight;

           pLeft->_pParent = pParent;
           pRight->_pParent = pParent;

           q.push(pParent);
       }
       _pRoot = q.top();
   }

private:
   void _DestroyTree(Node*& pRoot)
   {
      if(pRoot)
      {
          //如果树存在，采用后序遍历递归方式销毁树
          _DestroyTree(pRoot->_pLeft);
          _DestroyTree(pRoot->_pRight);
          delete pRoot;
          pRoot = nullptr;
      }
   }
private:
       Node* _pRoot;
};
