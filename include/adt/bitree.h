/**
 * @file bitree.h
 *
 * @brief Binary tree data structure declaration
 *
 * @ingroup ADT
 */

#ifndef BITREE_H
#define BITREE_H


/* System includes */
#include <stdlib.h>     /* NULL */


/**
 * @brief Define a structure for binary tree nodes
 */
typedef struct bitree_node_st {
    void *data;                     /**< Pointer to the data of this node */
    struct bitree_node_st *left;    /**< Binary tree left branch */
    struct bitree_node_st *right;   /**< Binary tree right branch */
} bitree_node_td;


/**
 * @brief Define a structure for binary trees
 */
typedef struct {
    size_t size;            /**< Size of the tree */
    bitree_node_td *root;   /**< Pointer to the root node */

    /**
     * @brief Pointer to a function to compare two keys
     *
     * @param key1 First key to compare
     * @param key2 Second key to compare
     *
     * @return Status of the operation
     */
    int (*compare)(const void *key1, const void *key2);

    /**
     * @brief Pointer to a function to free the memory of each node
     *
     * @param data Pointer to deallocate
     */
    void (*destroy)(void *data);
} bitree_td;


/* Public interface */
/**
 * @brief Initialize a new binary tree
 *
 * @param destroy Pointer to a function to free the memory
 *
 * @return New initialized binary tree, or @c NULL otherwise

 * @note This operation must be called for a search tree before the tree
 *       can be used with any other operation
 * @note Complexity: @e O(1)
 */
bitree_td *bitree_init(void (*destroy)(void *data));

/**
 * @brief Destroy the binary tree
 *
 * @param tree Tree to destroy
 *
 * @note No other operations are permitted after calling this function,
 *       unless @e bitree_init is called again
 * @note Complexity: @e O(n), where @e n is the number of nodes in the
 *       binary search tree
 */
void bitree_destroy(bitree_td *tree);

/**
 * @brief Insert a node as the left child of the specified node
 *
 * @param tree Tree to insert the node into as a left child
 * @param node Node to the right of the desired new node
 * @param data Data to insert in the left node
 *
 * @return
 * @retval  0 Successfully inserted the new node at the left
 * @retval -1 The tree already has a left child at that position
 *
 * @note Complexity: @e O(1)
 */
int bitree_ins_left(bitree_td *tree, bitree_node_td *node,
        const void *data);

/**
 * @brief Insert a node as the right child of the specified node
 *
 * @param tree Tree to insert the node into as a right child
 * @param node Node to the left of the desired new node
 * @param data Data to insert in the right node
 *
 * @return Status of the operation
 * @retval  0 Successfully inserted the new node at the right
 * @retval -1 The tree already has a right child at that position
 *
 * @note Complexity: @e O(1)
 */
int bitree_ins_right(bitree_td *tree, bitree_node_td *node,
        const void *data);

/**
 * @brief Remove the subtree rooted at the left child of the given node
 *
 * @param tree Tree to remove the subtree at the left of the node
 * @param node Node to which left subtree is going to be removed
 *
 * @note If the tree is @c NULL, all nodes in the tree are removed
 * @note Complexity: @e O(n), where @e n is the number of nodes in the
 *       binary search subtree
 */
void bitree_rem_left(bitree_td *tree, bitree_node_td *node);

/**
 * @brief Remove the subtree rooted at the right child of the given node
 *
 * @param tree Tree to remove the subtree at the right of the node
 * @param node Node to which right subtree is going to be removed
 *
 * @note If the tree is @c NULL, all nodes in the tree are removed
 * @note Complexity: @e O(n), where @e n is the number of nodes in the
 *       binary search subtree
 */
void bitree_rem_right(bitree_td *tree, bitree_node_td *node);

/**
 * @brief Merge two binary trees into a single binary tree
 *
 * @param merge Merged binary tree
 * @param left  Left subtree to merge
 * @param right Right subtree to merge
 * @param data  Root node of the merged tree
 *
 * @return Status of the operation
 * @retval 0 Merging was successful
 *
 * @note After the merge is complete, the merged tree contains @e data
 *       as the root node
 * @note Complexity: @e O(1)
 */
int bitree_merge(bitree_td *merge, bitree_td *left, bitree_td *right,
        const void *data);

/**
 * @brief Macro that evaluates to the size of the tree
 *
 * @note Complexity: @e O(1)
 */
#define bitree_size(self) ((self)->size)

/**
 * @brief Macro that evaluates to the root node of the tree
 *
 * @note Complexity: @e O(1)
 */
#define bitree_root(self) ((self)->root)

/**
 * @brief Macro that inquiries about a node being the end of a branch
 *
 * @note Complexity: @e O(1)
 */
#define bitree_is_eob(node) ((node) == NULL)

/**
 * @brief Macro that inquiries about a node being a leaf node
 *
 * @note Complexity: @e O(1)
 */
#define bitree_is_leaf(node) \
    (((node)->left == NULL) && ((node)->right == NULL))

/**
 * @brief Macro that evaluates to the data of the node
 *
 * @note Complexity: @e O(1)
 */
#define bitree_data(node) ((node)->data)

/**
 * @brief Macro that evaluates to the left node
 *
 * @note Complexity: @e O(1)
 */
#define bitree_left(node) ((node)->left)

/**
 * @brief Macro that evaluates to the right node
 *
 * @note Complexity: @e O(1)
 */
#define bitree_right(node) ((node)->right)


#endif /* ! BITREE_H */
