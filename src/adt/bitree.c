/**
 * @file bitree.c
 *
 * @brief Binary search tree implementation
 */

/* System includes */
#include <stdlib.h>     /* malloc, free, NULL*/

/* Local includes*/
#include <adt/bitree.h>


/* Initialize a new binary tree */
bitree_td *bitree_init(void (*destroy)(void *data))
{
    bitree_td *tree;

    tree = malloc(sizeof(bitree_td));
    if (tree == NULL) {
        return NULL;
    }

    /* Initialize the binary tree */
    tree->size = 0;
    tree->destroy = destroy;
    tree->root = NULL;

    return tree;
}

/* Destroy the binary tree */
void bitree_destroy(bitree_td *tree)
{
    /* Remove all the nodes from the tree */
    bitree_rem_left(tree, NULL);

    free(tree);
}


/* Insert a node as the left child of the specified node */
int bitree_ins_left(bitree_td *tree, bitree_node_td *node,
        const void *data)
{
    bitree_node_td *new_node, **position;

    if (tree == NULL) {
        return -1;
    }

    /* Determine where to insert the node */
    if (node == NULL) {
        /* Allow insertion at the root only in an empty tree */
        if (bitree_size(tree) > 0) {
            return -1;
        }
        position = &tree->root;
    } else {
        /* Normally allow insertion only at the end of a branch */
        if (bitree_left(node) != NULL) {
            return -1;
        }
        position = &node->left;
    }

    /* Allocate storage for the node */
    if ((new_node = malloc(sizeof(bitree_node_td))) == NULL) {
        return -1;
    }

    /* Insert the node into the tree */
    new_node->data = (void *) data;
    new_node->left = NULL;
    new_node->right = NULL;
    *position = new_node;

    /* Adjust the size of the tree to account for the inserted node */
    tree->size++;

    return 0;
}


/* Insert a node as the right child of the specified node */
int bitree_ins_right(bitree_td *tree, bitree_node_td *node,
        const void *data)
{
    bitree_node_td *new_node, **position;

    if (tree == NULL) {
        return -1;
    }

    /* Determine where to insert the node */
    if (node == NULL) {
        /* Allow insertion at the root only in an empty tree */
        if (bitree_size(tree) > 0) {
            return -1;
        }
        position = &tree->root;
    } else {
        /* Normally allow insertion only at the end of a branch */
        if (bitree_right(node) != NULL) {
            return -1;
        }
        position = &node->right;
    }

    /* Allocate storage for the node */
    if ((new_node = malloc(sizeof(bitree_node_td))) == NULL) {
        return -1;
    }

    /* Insert the node into the tree */
    new_node->data = (void *) data;
    new_node->left = NULL;
    new_node->right = NULL;
    *position = new_node;

    /* Adjust the size of the tree to account for the inserted node */
    tree->size++;

    return 0;
}

/* Remove the subtree rooted at the left child of the given node */
void bitree_rem_left(bitree_td *tree, bitree_node_td *node)
{
    bitree_node_td **position;

    /* Do not allow removal from an empty tree */
    if (tree == NULL || bitree_size(tree) == 0) {
        return;
    }

    /* Determine where to remove nodes */
    if (node == NULL) {
        position = &tree->root;
    } else {
        position = &node->left;
    }

    /* Remove the nodes */
    if (*position != NULL) {
        bitree_rem_left(tree, *position);
        bitree_rem_right(tree, *position);

        if (tree->destroy != NULL) {
            /* Call a user-defined function to free dynamically
             * allocated data */
            tree->destroy((*position)->data);
        }

        free(*position);
        *position = NULL;

        /* Adjust the size of the tree to account for the removed node */
        tree->size--;
    }
}

/* Remove the subtree rooted at the right child of the given node */
void bitree_rem_right(bitree_td *tree, bitree_node_td *node)
{
    bitree_node_td **position;

    /* Do not allow removal from an empty tree */
    if (tree == NULL || bitree_size(tree) == 0) {
        return;
    }

    /* Determine where to remove nodes */
    if (node == NULL) {
        position = &tree->root;
    } else {
        position = &node->right;
    }

    /* Remove the nodes */
    if (*position != NULL) {
        bitree_rem_left(tree, *position);
        bitree_rem_right(tree, *position);

        if (tree->destroy != NULL) {
            /* Call a user-defined function to free dynamically
             * allocated data */
            tree->destroy((*position)->data);

        }

        free(*position);
        *position = NULL;

        /* Adjust the size of the tree to account for the removed node */
        tree->size--;
    }
}


/* Merge two binary trees into a single binary tree */
int bitree_merge(bitree_td *merge, bitree_td *left, bitree_td *right,
        const void *data)
{
    /* Initialize the merged tree */
    merge = bitree_init(left->destroy);

    /* Insert the data for the root node of the merged tree */
    if (bitree_ins_left(merge, NULL, data) != 0) {
        bitree_destroy(merge);
        return -1;
    }

    /* Merge the two binary trees into a single binary tree */
    bitree_root(merge)->left = bitree_root(left);
    bitree_root(merge)->right = bitree_root(right);

    /* Adjust the size of the new binary tree */
    merge->size = merge->size + bitree_size(left) + bitree_size(right);

    /* Do not let the original trees access the merged nodes */
    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;

    return 0;
}
