/**
 * @file RBTree.c
 * @author  Jason Elter <jason.elter@mail.huji.ac.il>
 * @version 1.0
 * @date 10 December 2019
 *
 * @brief implementation file for generic red-black tree library.
 */

#ifndef RBTREE_RBTREE_H
#define RBTREE_RBTREE_H

// ------------------------------ includes ------------------------------
#include <stdlib.h>
#include <stdio.h>

// -------------------------- const definitions -------------------------
// Number constants.
#define TRUE 1
#define FALSE 0
#define SUCCESS 1
#define FAILURE 0

// -------------------------------- code --------------------------------

// a color of a Node.
typedef enum Color
{
    RED,
    BLACK
} Color;

/**
 * a function to sort the tree items.
 * @a, @b: two items.
 * @return: equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a.
 */
typedef int (*CompareFunc)(const void *a, const void *b);

/**
 * a function to apply on all tree items.
 * @object: a pointer to an item of the tree.
 * @args: pointer to other arguments for the function.
 * @return: 0 on failure, other on success.
 */
typedef int (*forEachFunc)(const void *object, void *args);

/**
 * a function to free a data item
 * @object: a pointer to an item of the tree.
 */
typedef void (*FreeFunc)(void *data);

/**
 * a node of the tree.
 */
typedef struct Node
{
    struct Node *parent, *left, *right;
    Color color;
    void *data;

} Node;

/**
 * represents the tree
 */
typedef struct RBTree
{
    Node *root;
    CompareFunc compFunc;
    FreeFunc freeFunc;
    int size;
} RBTree;

/**
 * constructs a new RBTree with the given CompareFunc.
 * comp: a function two compare two variables.
 */
RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    if (compFunc == NULL || freeFunc == NULL)
    {
        return NULL;
    }

    RBTree *tree = (RBTree *) malloc(sizeof(RBTree));
    if (tree == NULL)
    {
        return NULL;
    }

    tree->root = NULL;
    tree->compFunc = compFunc;
    tree->freeFunc = freeFunc;
    tree->size = 0;

    return tree;
}

/*
 * Helper function that creates and returns a new node (needs to be freed).
 * (Assumes data is valid, parent can be null and position is only used if parent isn't null)
 */
static Node *createNode(void *data, Node *parent, int position)
{
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL)
    {
        return NULL;
    }

    newNode->left = newNode->right = NULL;
    newNode->parent = parent;
    newNode->color = RED;
    newNode->data = data;

    if (parent != NULL)
    {
        if (position > 0)
        {
            parent->right = newNode;
        }
        else
        {
            parent->left = newNode;
        }
    }

    return newNode;
}

// Rotates the tree to the right around the given node.
static void rotateRight(RBTree *tree, Node *node)
{
    Node *head = node->left;
    Node *left = node->left = head->right;

    if (left != NULL)
    {
        left->parent = node;
    }
    head->parent = node->parent;

    if (head->parent == NULL)
    {
        tree->root = head;
    }
    else
    {
        Node *parent = head->parent;
        if (parent->left == node)
        {
            parent->left = head;
        }
        else
        {
            parent->right = head;
        }
    }

    head->right = node;
    node->parent = head;
}

// Rotates the tree to the left around the given node.
static void rotateLeft(RBTree *tree, Node *node)
{
    Node *head = node->right;
    Node *right = node->right = head->left;

    if (right != NULL)
    {
        right->parent = node;
    }
    head->parent = node->parent;

    if (head->parent == NULL)
    {
        tree->root = head;
    }
    else
    {
        Node *parent = head->parent;
        if (parent->left == node)
        {
            parent->left = head;
        }
        else
        {
            parent->right = head;
        }
    }

    head->left = node;
    node->parent = head;
}

// Balances the given tree after the insertion of the given node. (Assumes valid input)
static void balanceTree(RBTree *tree, Node *newNode)
{
    Node *parent = newNode->parent;
    if (parent == NULL)
    {
        newNode->color = BLACK;
    }
    else if (parent->color == RED)
    {
        Node *grandpa = parent->parent;
        int isRightParent = (parent == grandpa->right);
        Node *uncle = isRightParent ? grandpa->left : grandpa->right;

        if (uncle != NULL && uncle->color == RED)
        {
            parent->color = uncle->color = BLACK;
            grandpa->color = RED;
            balanceTree(tree, grandpa);
        }
        else
        {
            int isRightSon = (newNode == parent->right);
            if (isRightSon && !isRightParent)
            {
                rotateLeft(tree, parent);
                rotateRight(tree, grandpa);
                newNode->color = BLACK;
            }
            else if (!isRightSon && isRightParent)
            {
                rotateRight(tree, parent);
                rotateLeft(tree, grandpa);
                newNode->color = BLACK;
            }
            else if (isRightParent)
            {
                rotateLeft(tree, grandpa);
                parent->color = BLACK;
            }
            else
            {
                rotateRight(tree, grandpa);
                parent->color = BLACK;
            }

            grandpa->color = RED;
        }
    }
}

// Creates and inserts a new node with the given data to the correct place in the given tree. (Assumes valid input)
static int insertNodeToTree(RBTree *tree, void *data)
{
    int compareResult = 0;
    Node *parent = tree->root, *son = tree->root;
    while (son != NULL)
    {
        compareResult = tree->compFunc(data, son->data);
        if (compareResult == 0)
        {
            return FAILURE;
        }
        else
        {
            parent = son;
            son = (compareResult > 0) ? son->right : son->left;
        }
    }

    Node *newNode = createNode(data, parent, compareResult);
    if (newNode == NULL)
    {
        return FAILURE;
    }

    tree->size++;
    balanceTree(tree, newNode);
    return SUCCESS;
}

/**
 * add an item to the tree
 * @param tree: the tree to add an item to.
 * @param data: item to add to the tree.
 * @return: 0 on failure, other on success. (if the item is already in the tree - failure).
 */
int addToRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || data == NULL)
    {
        return FAILURE;
    }

    if (tree->size == 0)
    {
        Node *newNode = createNode(data, NULL, 0);
        if (newNode == NULL)
        {
            return FAILURE;
        }

        tree->root = newNode;
        tree->size++;
        balanceTree(tree, newNode);
        return SUCCESS;
    }

    return insertNodeToTree(tree, data);
}

// Helper recursive function for the contains function.
static int containsHelper(RBTree *tree, void *data, Node *current)
{
    if (current == NULL)
    {
        return FALSE;
    }
    return (tree->compFunc(data, current->data) == 0) ||
           containsHelper(tree, data, current->left) ||
           containsHelper(tree, data, current->right);
}

/**
 * check whether the tree contains this item.
 * @param tree: the tree to add an item to.
 * @param data: item to check.
 * @return: 0 if the item is not in the tree, other if it is.
 */
int containsRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || data == NULL)
    {
        return FALSE;
    }
    return containsHelper(tree, data, tree->root);
}

// Helper recursive function for the forEach function.
static int forEachHelper(forEachFunc func, void *args, Node *current)
{
    if (current == NULL)
    {
        return TRUE;
    }
    return forEachHelper(func, args, current->left) && func(current->data, args) &&
           forEachHelper(func, args, current->right);
}

/**
 * Activate a function on each item of the tree. the order is an ascending order. if one of the activations of the
 * function returns 0, the process stops.
 * @param tree: the tree with all the items.
 * @param func: the function to activate on all items.
 * @param args: more optional arguments to the function (may be null if the given function support it).
 * @return: 0 on failure, other on success.
 */
int forEachRBTree(RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL || func == NULL)
    {
        return FAILURE;
    }
    return forEachHelper(func, args, tree->root);
}

// Helper recursive function for the freeTree function.
static void freeTreeHelper(Node *current, FreeFunc freeFunc)
{
    if (current != NULL)
    {
        freeTreeHelper(current->left, freeFunc);
        freeTreeHelper(current->right, freeFunc);
        freeFunc(current->data);
        free(current);
    }
}

/**
 * free all memory of the data structure.
 * @param tree: the tree to free.
 */
void freeRBTree(RBTree *tree)
{
    freeTreeHelper(tree->root, tree->freeFunc);
    free(tree);
}


#endif //RBTREE_RBTREE_H