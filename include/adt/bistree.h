/**
 * @file bistree.h
 *
 * @brief Binary search tree data structure as AVL (Adelson-Velskii
 *        & Landis) tree declaration as a binary tree
 *
 * @ingroup ADT
 */

#ifndef BISTREE_H
#define BISTREE_H


/* Data type includes */
#include <stdbool.h>

/* ADT includes */
#include <adt/bitree.h> /* Binary tree */


/* Macros to define balance factors for AVL trees */
#define AVL_LEFT_HEAVY   (1)    /**< Left-heavy tree*/
#define AVL_BALANCED     (0)    /**< Balanced tree */
#define AVL_RIGHT_HEAVY (-1)    /**< Right-heavy tree */


/**
 * @brief Define a structure for nodes in AVL trees
 */
typedef struct {
    void *data;     /**< Pointer to the data contained in the node */
    bool is_hidden; /**< Inquiry if the node is hidden or not */
    int factor;     /**< Balance factor */
} avl_node_td;


/**
 * @brief Binary search tree implementation as a binary tree
 */
typedef bitree_td bistree_td;


/* Public interface */
/**
 * @brief Initialize a new binary search tree
 *
 * @param compare Pointer to a function to compare nodes
 * @param destroy Pointer to a function to free the memory
 *
 * @return New allocated binary search tree, or @c NULL otherwise
 *
 * @note This operation must be called for a binary search tree before
 *       the tree can be used with any other operation
 * @note Complexity: @e O(1)
 */
bistree_td *bistree_init(int (*compare)(const void *key1,
            const void *key2), void (*destroy)(void *data));

/**
 * @brief Destroy the binary search tree
 *
 * @param tree Tree to destroy
 *
 * @note No other operations are permitted after calling this function,
 *       unless @e bistree_init is called again
 * @note Complexity: @e O(n), where @e n is the number of nodes in the
 *       binary search tree
 */
void bistree_destroy(bistree_td *tree);

/**
 * @brief Insert a node in the binary search tree
 *
 * @param tree Tree to insert the new node into
 * @param data Data of the node to insert into the tree
 *
 * @return Status of the operation
 * @retval 0 Successfully inserted the node
 *
 * @note Complexity: @e O(log n), where @e n is the number of nodes in
 *       the binary search tree
 */
int bistree_insert(bistree_td *tree, const void *data);

/**
 * @brief Remove the node matching the specified data
 *
 * This operation only performs a lazy removal, in which the node is
 * simply marked as hidden, thus, no pointer is returned to the data
 * matching the specified data pointer.  The data in the tree must
 * remain valid even after it has been removed, consequently the size of
 * the binary tree, does not decrease after removing a node.
 *
 * @param tree Tree to remove the node from
 * @param data Data contained in the node to be removed
 *
 * @return Status of the operation
 * @retval 0 Successfully removed the node
 *
 * @note Once the node is hidden, there's no need to rebalance the tree
 *       because its structure remains intact
 * @note If the same node is inserted again, the node is became visible
 *       again by setting its @e is_hidden member back to @c false
 * @note Complexity: @e O(log n), where @e n is the number of nodes in
 *       the binary search tree
 */
int bistree_remove(bistree_td *tree, const void *data);

/**
 * @brief Determine whether a node matches the specified data
 *
 * @param tree Tree to look up in
 * @param data Pointer to the data to look for
 *
 * @return Status of the operation
 * @retval 0 The data is found in the binary search tree
 *
 * @note If the data is found, @e data points to the matching data in
 *       the binary search tree upon return
 * @note Complexity: @e O(log n), where @e n is the number of nodes in
 *       the binary search tree
 */
int bistree_lookup(bistree_td *tree, void **data);

/**
 * @brief Macro that evaluates to the data of a node
 *
 * @note Complexity: @e O(1)
 *
 * @see bitree_data
 */
#define bistree_data(node) (((avl_node_td *) bitree_data(node))->data)

/**
 * @brief Macro that evaluates to the balancing factor of an AVL node
 *
 * @note Complexity: @e O(1)
 */
#define bistree_factor(node) \
    (((avl_node_td *) bitree_data(node))->factor)

/**
 * @brief Macro that evaluates to the hidden state of an AVL node
 *
 * @note Complexity: @e O(1)
 */
#define bistree_is_hidden(node) \
    (((avl_node_td *) bitree_data(node))->is_hidden)

/**
 * @brief Macro that evaluates to the number of nodes in the tree
 *
 * @see bitree_size
 */
#define bistree_size bitree_size


#endif /* ! BISTREE_H */
