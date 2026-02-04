#include <stdlib.h>
#include <stdio.h>

typedef struct Node 
{
    int value;
    int key;
    struct Node* left;
    struct Node* right;
}Node;

typedef struct
{
    Node* root;
}bi_tree;

void init_bi_tree(bi_tree* bi)
{
    bi->root = NULL;
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
}

void insert_bi_tree(bi_tree* bi, int key, int value)
{ 
    if(!bi->root)
    {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->value = value;
        new_node->key = key;
        bi->root = new_node;
    }
    else insert(bi->root, key, value);
}

static int search(Node* node, int key)
{
    if(key == node->key) return node->value;
    if(key < node->key) 
    {
        if(!node->left) return 0;
        return search(node->left, key);
    }
    else 
    {
        if(!node->right) return 0;
        return search(node->right, key); 
    }
}

int search_bi_tree(bi_tree* bi, int key)
{
    return search(bi->root, key);
}

//Симетричный обход дерева
static void print(Node* node)
{
    if(!node) return;
    print(node->left);
    printf("%d %d\n", node->key, node->value);
    print(node->right);
}

void print_bi_tree(bi_tree* bi)
{
    print(bi->root);
}

//Обратный обход дерева
static void delete_node(Node* node)
{
    if(!node) return;
    delete_node(node->left);
    delete_node(node->right);
    printf("%d %d\n", node->key, node->value);
}

void delete_bi_tree(bi_tree* bi)
{
    delete_node(bi->root);
}

static void copy_tree(Node* node)
{
    if(!node) return;
    printf("%d %d\n", node->key, node->value);
    copy_tree(node->left);
    copy_tree(node->right);
}

void copy_bi_tree(bi_tree* bi)
{
    copy_tree(bi->root);
}
