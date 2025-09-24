/**
 * @file bistree.c
 *
 * @brief Binary search tree data structure as AVL (Adelson-Velskii
 *        & Landis) tree implementation
 */

/* Data type includes */
#include <stdbool.h>

/* System includes */
#include <stdlib.h>     /* malloc, free, NULL*/

/* ADT includes */
#include <adt/bitree.h> /* Binary tree */

/* Local includes*/
#include <adt/bistree.h>


static void _destroy_right(bitree_td *tree, bitree_node_td *node);


/* Rebalances the tree by performing a left rotation: LL or LR */
static void _rotate_left(bitree_node_td **node)
{
    bitree_node_td *left, *grandchild;

    left = bitree_left(*node);

    if (((avl_node_td *) bitree_data(left))->factor == AVL_LEFT_HEAVY) {
        /* Perform an LL rotation */
        bitree_left(*node) = bitree_right(left);
        bitree_right(left) = *node;
        ((avl_node_td *) bitree_data(*node))->factor = AVL_BALANCED;
        ((avl_node_td *) bitree_data(left))->factor = AVL_BALANCED;
        *node = left;
    } else {
        /* Perform an LR rotation */
        grandchild = bitree_right(left);
        bitree_right(left) = bitree_left(grandchild);
        bitree_left(grandchild) = left;
        bitree_left(*node) = bitree_right(grandchild);
        bitree_right(grandchild) = *node;

        switch (((avl_node_td *) bitree_data(grandchild))->factor) {
            case AVL_LEFT_HEAVY:
                ((avl_node_td *) bitree_data(*node))->factor =
                    AVL_RIGHT_HEAVY;
                ((avl_node_td *) bitree_data(left))->factor =
                    AVL_BALANCED;
                break;

            case AVL_BALANCED:
                ((avl_node_td *) bitree_data(*node))->factor =
                    AVL_BALANCED;
                ((avl_node_td *) bitree_data(left))->factor =
                    AVL_BALANCED;
                break;

            case AVL_RIGHT_HEAVY:
                ((avl_node_td *) bitree_data(*node))->factor =
                    AVL_BALANCED;
                ((avl_node_td *) bitree_data(left))->factor =
                    AVL_LEFT_HEAVY;
                break;

            default:
                break;

        }

        ((avl_node_td *) bitree_data(grandchild))->factor = AVL_BALANCED;
        *node = grandchild;
    }
}


/* Rebalances the tree by performing a right rotation: RL or RR */
static void _rotate_right(bitree_node_td **node)
{
    bitree_node_td *right, *grandchild;

    right = bitree_right(*node);

    if (((avl_node_td *) bitree_data(right))->factor == AVL_RIGHT_HEAVY) {
        /* Perform an RR rotation */
        bitree_right(*node) = bitree_left(right);
        bitree_left(right) = *node;
        ((avl_node_td *) bitree_data(*node))->factor = AVL_BALANCED;
        ((avl_node_td *) bitree_data(right))->factor = AVL_BALANCED;
        *node = right;
    } else {
        /* Perform an RL rotation */
        grandchild = bitree_left(right);
        bitree_left(right) = bitree_right(grandchild);
        bitree_right(grandchild) = right;
        bitree_right(*node) = bitree_left(grandchild);
        bitree_left(grandchild) = *node;

        switch (((avl_node_td *) bitree_data(grandchild))->factor) {
            case AVL_LEFT_HEAVY:
                ((avl_node_td *) bitree_data(*node))->factor =
                    AVL_BALANCED;
                ((avl_node_td *) bitree_data(right))->factor =
                    AVL_RIGHT_HEAVY;
                break;

            case AVL_BALANCED:
                ((avl_node_td *) bitree_data(*node))->factor =
                    AVL_BALANCED;
                ((avl_node_td *) bitree_data(right))->factor =
                    AVL_BALANCED;
                break;

            case AVL_RIGHT_HEAVY:
                ((avl_node_td *) bitree_data(*node))->factor =
                    AVL_LEFT_HEAVY;
                ((avl_node_td *) bitree_data(right))->factor =
                    AVL_BALANCED;
                break;

            default:
                break;
        }

        ((avl_node_td *) bitree_data(grandchild))->factor = AVL_BALANCED;
        *node = grandchild;
    }
}


/* Destroy the left subtree */
static void _destroy_left(bitree_td *tree, bitree_node_td *node)
{
    bitree_node_td **position;

    /* Do not allow destruction of an empty tree */
    if (bitree_size(tree) == 0) {
        return;
    }

    /* Determine where to destroy nodes */
    if (node == NULL) {
        position = &tree->root;
    } else {
        position = &node->left;
    }

    /* Destroy the nodes */
    if (*position != NULL) {
        _destroy_left(tree, *position);
        _destroy_right(tree, *position);

        if (tree->destroy != NULL) {
            /* Call a user-defined function to free dynamically
             * allocated data */
            tree->destroy(((avl_node_td *) (*position)->data)->data);
        }

        /* Free the AVL data in the node, then free the node itself */
        free((*position)->data);
        free(*position);
        *position = NULL;

        /* Adjust the size of the tree to account for the destroyed node */
        tree->size--;
    }
}


/* Destroy the right subtree */
static void _destroy_right(bitree_td *tree, bitree_node_td *node)
{
    bitree_node_td **position;

    /* Do not allow destruction of an empty tree */
    if (bitree_size(tree) == 0) {
        return;
    }

    /* Determine where to destroy nodes */
    if (node == NULL) {
        position = &tree->root;
    } else {
        position = &node->right;
    }

    /* Destroy the nodes */
    if (*position != NULL) {
        _destroy_left(tree, *position);
        _destroy_right(tree, *position);

        if (tree->destroy != NULL) {
            /* Call a user-defined function to free dynamically
             * allocated data */
            tree->destroy(((avl_node_td *) (*position)->data)->data);
        }

        /* Free the AVL data in the node, then free the node itself */
        free((*position)->data);
        free(*position);
        *position = NULL;

        /* Adjust the size of the tree to account for the destroyed node */
        tree->size--;
    }
}

/* Perform an insertion while mantaining the tree balanced */
static int _insert(bitree_td *tree, bitree_node_td **node,
        const void *data, bool *balanced)
{
    avl_node_td *avl_data;
    int cmpval, retval;

    /* Insert the data into the tree */
    if (bitree_is_eob(*node)) {
        /* Handle insertion into an empty tree */
        if ((avl_data = malloc(sizeof(avl_node_td))) == NULL) {
            return -1;
        }

        avl_data->factor = AVL_BALANCED;
        avl_data->is_hidden = false;
        avl_data->data = (void *) data;

        return bitree_ins_left(tree, *node, avl_data);
    } else { /* !bistree_is_eob(*node) */
        /* Handle insertion into a tree that is not empty */
        cmpval = tree->compare(data,
                ((avl_node_td *) bitree_data(*node))->data);

        if (cmpval < 0) {
            /* Move to the left */
            if (bitree_is_eob(bitree_left(*node))) {
                if ((avl_data = malloc(sizeof(avl_node_td))) == NULL) {
                    return -1;
                }

                avl_data->factor = AVL_BALANCED;
                avl_data->is_hidden = false;
                avl_data->data = (void *) data;

                if (bitree_ins_left(tree, *node, avl_data) != 0) {
                    return -1;
                }

                *balanced = false;
            } else {
                if ((retval = _insert(tree, &bitree_left(*node),
                                data, balanced)) != 0) {
                    return retval;
                }
            }

            /* Ensure that the tree remains balanced */
            if (!(*balanced)) {
                switch (((avl_node_td *) bitree_data(*node))->factor) {
                    case AVL_LEFT_HEAVY:
                        _rotate_left(node);
                        *balanced = true;
                        break;

                    case AVL_BALANCED:
                        ((avl_node_td *) bitree_data(*node))->factor =
                            AVL_LEFT_HEAVY;
                        break;

                    case AVL_RIGHT_HEAVY:
                        ((avl_node_td *) bitree_data(*node))->factor =
                            AVL_BALANCED;
                        *balanced = true;

                    default:
                        break;
                }
            }
        } else if (cmpval > 0) {
            /* Move to the right */
            if (bitree_is_eob(bitree_right(*node))) {
                if ((avl_data = malloc(sizeof(avl_node_td))) == NULL) {
                    return -1;
                }

                avl_data->factor = AVL_BALANCED;
                avl_data->is_hidden = false;
                avl_data->data = (void *) data;

                if (bitree_ins_right(tree, *node, avl_data) != 0) {
                    return -1;
                }

                *balanced = false;
            } else {
                if ((retval = _insert(tree, &bitree_right(*node),
                                data, balanced)) != 0) {
                    return retval;
                }
            }

            /* Ensure that the tree remains balanced */
            if (!(*balanced)) {
                switch (((avl_node_td *) bitree_data(*node))->factor) {
                    case AVL_LEFT_HEAVY:
                        ((avl_node_td *) bitree_data(*node))->factor =
                            AVL_BALANCED;
                        *balanced = true;
                        break;

                    case AVL_BALANCED:
                        ((avl_node_td *) bitree_data(*node))->factor =
                            AVL_RIGHT_HEAVY;
                        break;

                    case AVL_RIGHT_HEAVY:
                        _rotate_right(node);
                        *balanced = true;

                    default:
                        break;
                }
            }
        } else { /* cmpval == 0 */
            /* Handle finding a copy of the data */
            if (!((avl_node_td *) bitree_data(*node))->is_hidden) {
                /* Do nothing since the data is in the tree and not
                 * hidden */
                return 1;
            } else {
                /* Insert the new data and mark it as not hidden */
                if (tree->destroy != NULL) {
                    /* Destroy the hidden data since it is being
                     * replaced */
                    tree->destroy(((avl_node_td *)
                                bitree_data(*node))->data);
                }

                ((avl_node_td *) bitree_data(*node))->data = (void *) data;
                ((avl_node_td *) bitree_data(*node))->is_hidden = false;

                /* Do not rebalance because the tree structure is
                 * unchanged */
                *balanced = true;
            }
        }
    }

    return 0;
}


/* Mark a node as hidden */
static int _hide(bitree_td *tree, bitree_node_td *node,
        const void *data)
{
    int cmpval, retval;

    if (bitree_is_eob(node)) {
        /* Return that the data was not found */
        return -1;
    }

    cmpval = tree->compare(data,
            ((avl_node_td *) bitree_data(node))->data);

    if (cmpval < 0) {
        /* Move to the left */
        retval = _hide(tree, bitree_left(node), data);
    } else if (cmpval > 0) {
        /* Move to the right */
        retval = _hide(tree, bitree_right(node), data);
    } else {
        /* Mark the node as hidden */
        ((avl_node_td *) bitree_data(node))->is_hidden = true;
        retval = 0;
    }

    return retval;
}


/* Look up in the tree and get the data if the node exists */
static int _lookup(bitree_td *tree, bitree_node_td *node, void **data)
{
    int cmpval, retval;

    if (bitree_is_eob(node)) {
        /* Return that the data was not found */
        return -1;
    }

    cmpval = tree->compare(*data,
            ((avl_node_td *) bitree_data(node))->data);

    if (cmpval < 0) {
        /* Move to the left */
        retval = _lookup(tree, bitree_left(node), data);
    } else if (cmpval > 0) {
        /* Move to the right */
        retval = _lookup(tree, bitree_right(node), data);
    } else {
        if (!((avl_node_td *) bitree_data(node))->is_hidden) {
            /* Pass back the data from the tree */
            *data = ((avl_node_td *) bitree_data(node))->data;
            retval = 0;
        } else {
            /* Return that the data was not found */
            return -1;
        }
    }

    return retval;
}


/* Initialize a new binary search tree */
bistree_td *bistree_init(int (*compare)(const void *key1,
            const void *key2), void (*destroy)(void *data))
{
    bistree_td *tree;

    /* Initialize the tree */
    tree = bitree_init(destroy);
    if (tree == NULL) {
        return NULL;
    }

    tree->compare = compare;

    return tree;
}


/* Destroy the binary search tree */
void bistree_destroy(bistree_td *tree)
{
    /* Destroy all nodes in the tree */
    _destroy_left(tree, NULL);
    free(tree);
}


/* Insert a node in the binary search tree */
int bistree_insert(bistree_td *tree, const void *data)
{
    bool balanced = false;

    return _insert(tree, &bitree_root(tree), data, &balanced);
}


/* Remove the node matching the specified data */
int bistree_remove(bistree_td *tree, const void *data)
{
    return _hide(tree, bitree_root(tree), data);
}


/* Determine whether a node matches the specified data */
int bistree_lookup(bistree_td *tree, void **data)
{
    return _lookup(tree, bitree_root(tree), data);
}
