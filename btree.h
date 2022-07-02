#ifndef BTREE_H
#define BTREE_H

typedef struct TreeNode {
  int key;
  struct TreeNode *left;
  struct TreeNode *right;
  struct TreeNode *parent;
} TreeNode;

void CreateBinaryTree(TreeNode **root, int keys[], int keys_array_size);

void OrderTree(TreeNode *root);

TreeNode *SearchNodeRecursive(TreeNode *root, int key);

TreeNode *SearchNode(TreeNode *root, int key);

TreeNode *SearchMinKey(TreeNode *root);

TreeNode *SearchMaxKey(TreeNode *root);

int FindDepthTree(TreeNode *root);

#endif // BTREE_H
