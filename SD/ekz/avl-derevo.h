#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef struct Node 
{
    int value;
    int key;
    int height = 0;
    struct Node* left;
    struct Node* right;
}Node;

typedef struct
{
    Node* root;
}avl_tree;

static int get_height(Node* node)
{
    return node ? node->height : -1;
}

static void update_height(Node* node)
{
    node->height = max(get_height(node->left), get_height(node->right)) + 1;
}

static int get_balance(Node* node)
{
    return node ? get_height(node->right) - get_height(node->left) : 0;
}

static void swap(Node* a, Node* b)
{
    int a_key = a->key;
    a->key = b->key;
    b->key = a_key;
    int a_value = a->value;
    a->value = b->value;
    b->value = a_value;
}

static void right_rotate(Node* node)
{
    swap(node, node->left);
    Node* buffer = node->right;
    node->right = node->left;
    node->left = node->right->left;
    node->right->left = node->right->right;
    node->right->right = buffer;
    update_height(node->right);
    update_height(node);
}

static void left_rotate(Node* node)
{
    swap(node, node->right);
    Node* buffer = node->left;
    node->left = node->right;
    node->right = node->left->right;
    node->left->right = node->left->left;
    node->left->left = buffer;
    update_height(node->left);
    update_height(node);
}

static void balance(Node* node)
{
    int balance = get_balance(node);
    if(balance == -2)
    {
        if(get_balance(node->left) == 1) left_rotate(node->left);
        right_rotate(node);
    }
    else if(balance == 2)
    {        
        if(get_balance(node->right) == 1) right_rotate(node->right);
        left_rotate(node);
        
    }
}

void init_avl_tree(avl_tree* avl)
{
    avl->root = NULL;
}

static void insert(Node* node, int key, int value)
{
    if(key < node->key)
    {
        if(node->left) insert(node->left, key, value);
        else 
        {
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->key = key;
            new_node->value = value;
            node->left = new_node; 
        }
    }
    else 
    {
        if(node->right) insert(node->right, key, value);
        else 
        {
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->key = key;
            new_node->value = value;
            node->right = new_node; 
        } 
    }

    update_height(node);
    balance(node);
}

void insert_avl_tree(avl_tree* avl, int key, int value)
{ 
    if(!avl->root)
    {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->value = value;
        new_node->key = key;
        avl->root = new_node;
    }
    else insert(avl->root, key, value);
}
