
#include "btree.h"
#include <stdlib.h>
#include <stdio.h>

void CreateBinaryTree( TreeNode **root, int keys[], int keys_array_size){

    TreeNode* parent = NULL;
    TreeNode* new_node = NULL;

    for(int i = 0; i < keys_array_size; i++){

        new_node = (TreeNode*)malloc(sizeof(TreeNode));

        new_node->key = keys[i];
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = NULL;

        if(parent == NULL){
            parent = new_node;
            (*root) = parent;
            continue;
        }

        while(parent != NULL){
            if(new_node->key <= parent->key){
                if(parent->left == NULL){
                    parent->left = new_node;
                    new_node->parent = parent;
                    break;
                }else{
                    parent = parent->left;
                }
            }else{
                if(parent->right == NULL){
                    parent->right = new_node;
                    new_node->parent = parent;
                    break;
                }else{
                    parent = parent->right;
                }
            }
        }
        parent = (*root) ;
    }
}

void OrderTree(TreeNode *root){

    TreeNode* current_node = root;

    if(current_node != NULL){
        OrderTree(current_node->left);
        printf("%d",current_node->key);
        OrderTree(current_node->right);
    }
}

TreeNode *SearchNodeRecursive(TreeNode *root, int key){
    TreeNode* current_node = root;

    if(current_node->key == key || current_node == NULL){
        return current_node;
    }else{
        if(current_node->key > key ){
            return SearchNodeRecursive(current_node->left, key);
        }else{
            return SearchNodeRecursive(current_node->right, key);
        }
    }
}




TreeNode *SearchNode(TreeNode *root, int key)
{
    TreeNode* current_node = root;

    while(current_node != NULL && current_node->key != key){
        if(current_node->key > key){
            current_node = current_node->left;
        }else{
            current_node = current_node->right;
        }
    }
    return current_node;
}




TreeNode *SearchMinKey(TreeNode *root)
{
    TreeNode* current_node = root;
    TreeNode* previous_node = NULL;

    while (current_node != NULL) {
        previous_node = current_node;
        current_node = current_node->left;
    }
    return previous_node;
}

TreeNode *SearchMaxKey(TreeNode *root)
{
    TreeNode* current_node = root;
    TreeNode* previous_node = NULL;

    while(current_node != NULL){
        previous_node = current_node;
        current_node = current_node->right;
    }
    return previous_node;
}

int FindDepthTree(TreeNode *root)
{
    int depth_right = 0;
    int depth_left = 0;
    int depth = 0;

    TreeNode* current_node = root;

    if(root != NULL){
        return -1;
    }else{
        depth_left = FindDepthTree(current_node->left);
        depth_right = FindDepthTree(current_node->right);
        depth = (depth_left > depth_right ? depth_left:depth_right)+1;
    }

    return depth;
}
