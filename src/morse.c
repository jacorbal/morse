/**
 * @file morse.c
 *
 * @brief Morse code implementation using a binary search tree
 *
 * @author J. A. Corbal (<jacorbal@gmail.com>)
 */

/* Data type includes */
#include <stdbool.h>
#include <stdint.h>

/* System includes */
#include <ctype.h>  /* toupper */
#include <stdlib.h> /* malloc, free, NULL */
#include <string.h> /* strchr */

/* ADT includes */
#include <adt/bistree.h>

/* Local includes */
#include <morse.h>


/* Compare two characters to get their order according to the Morse code */
static int _compare(const void *key1, const void *key2)
{
    int64_t pos1 = strchr(MORSE_WEIGHTED_NODES,
            toupper(*(char *) key1)) - MORSE_WEIGHTED_NODES;
    int64_t pos2 = strchr(MORSE_WEIGHTED_NODES,
            toupper(*(char *) key2)) - MORSE_WEIGHTED_NODES;

    if (pos1 > pos2) {
        return 1;
    } else if (pos1 < pos2) {
        return -1;
    }

    return 0;
}


/* Fill the Morse tree with the alphabet */
static void _morse_generate_nodes(morse_tree_td *morse)
{
    char *data[MORSE_MAX_NODES];
    char morse_nodes[MORSE_MAX_NODES] =
        "~ETIAMNSURWDKGOHVFLPJBXYCZQ()543[2]+16=/7890";
       /* NOTE.  There are several AVL trees that comply with the
        * balancing and order of the tree for the same data, so its
        * outcome is not unique and for that reason the inserting order
        * is important: it has to be in this way in order to achieve
        * a replica of the original Morse code tree */

    for (int_fast32_t i = 0; i < MORSE_MAX_NODES; ++i) {
        if (morse_nodes[i] == '\0') {
            continue;
        }

        data[i] = malloc(sizeof(char));
        if (data[i] == NULL) {
            break;
        }

        *data[i] = morse_nodes[i];
        if (bistree_insert(morse, data[i]) != 0) {
            free(data[i]);
        }
    }
}


/* Lookup in the Morse tree and get a string with the Morse code */
/**
 * @brief Encode a single character into its Morse string
 *
 * @param morse Morse tree
 * @param node  Node of the tree to start to look for the character in
 * @param data  Pointer to a data node containing character to look for
 * @param dst   Pointer to the string with the Morse coded character
 *
 * @return Status of the lookup operation
 * @retval 0 The character is on the Morse tree
 *
 * @note If the character is found, @e dst points to the matching string
 * @note If the character is not found, @e dst points to an empty string
 * @note Since this is a lookup operation, the complexity of this
 *       function is @e O(log n), where @e n is the number of nodes in
 *       the binary tree specified by @e MORSE_MAX_NODES
 */
static int _morse_encode_char(const morse_tree_td *morse,
        const bitree_node_td *node, const char *data, char *dst,
        bool use_separators)
{
    int cmpval, retval;

    if (bitree_is_eob(node)) {
        /* Return that the data was not found */
        return -1;
    }

    cmpval = morse->compare(data, bistree_data(node));
    if (cmpval < 0) {
        /* Move to the left after adding a 'dit' (dot) */
        strcat(dst, MORSE_DIT);
        if (use_separators) {
            strcat(dst, MORSE_SEP);
        }
        retval = _morse_encode_char(morse, bitree_left(node), data, dst,
                use_separators);
    } else if (cmpval > 0) {
        /* Move to the right after adding a 'dah' (dash) */
        strcat(dst, MORSE_DAH);
        if (use_separators) {
            strcat(dst, MORSE_SEP);
        }
        retval = _morse_encode_char(morse, bitree_right(node), data, dst,
                use_separators);
    } else {
        if (!(bistree_is_hidden(node))) {
            retval = 0;
        } else {
            /* Return that the data was not found */
            return -1;
        }
    }

    return retval;
}


/* Initialize a new Morse tree */
morse_tree_td *morse_init(void)
{
    morse_tree_td *morse;

    morse = bistree_init(_compare, free);
    if (morse == NULL) {
        return NULL;
    }

    _morse_generate_nodes(morse);

    return morse;
}


/* Encode a entire string until the 'NULL' character is found */
// TODO: Consider using 'strtok' to separate words
void morse_encode(const morse_tree_td *morse,
        char *dst, const char *src, uint8_t flags)
{
    char *data;
    char *tmp;

    tmp = malloc(sizeof(char));
    if (tmp == NULL) {
        return;
    }

    /* Start the transmission: add prosign <CT> */
    if (flags & MORSE_USE_PROSIGNS) {
        for (size_t i = 0; i < strlen(MORSE_PROSIGN_CT); ++i) {
            *tmp = MORSE_PROSIGN_CT[i];
            data = tmp;
            _morse_encode_char(morse, bitree_root(morse), data, dst,
                    (flags & MORSE_USE_SEPARATORS) > 0);
        }

        if (flags & MORSE_USE_SEPARATORS) {
            strcat(dst, MORSE_WORD_SEPARATOR);
        }
    }

    /* Send the transmission */
    for (size_t i = 0; i < strlen(src); ++i) {
        /* Ignore filler characters */
        if (src[i] == '~' ||
                src[i] == '(' || src[i] == ')' ||
                src[i] == '[' || src[i] == ']') {
            continue;
        }

        /* Use the word separator if needed */
        if (flags & MORSE_USE_SEPARATORS) {
            if (src[i] == ' ') {
                strcat(dst, MORSE_WORD_SEPARATOR);
                continue;
            }
        }

        /* Encode only if it's a valid character */
        if (strchr(MORSE_WEIGHTED_NODES, toupper(src[i])) != NULL) {
            /* Start encoding the string */
            *tmp = src[i];
            data = tmp;

            _morse_encode_char(morse, bitree_root(morse), data, dst,
                    (flags & MORSE_USE_SEPARATORS) > 0);
            if ((flags & MORSE_USE_SEPARATORS) &&
                    src[i + 1] != ' ' && src[i + 1] != '\0') {
                strcat(dst, MORSE_CHAR_SEPARATOR);
            }
        }
    }

    /* End the transmission: add prosign <SK> */
    if (flags & MORSE_USE_PROSIGNS) {
        if (flags & MORSE_USE_SEPARATORS) {
            strcat(dst, MORSE_WORD_SEPARATOR);
        }

        for (size_t i = 0; i < strlen(MORSE_PROSIGN_SK); ++i) {
            *tmp = MORSE_PROSIGN_SK[i];
            data = tmp;
            _morse_encode_char(morse, bitree_root(morse), data, dst,
                    (flags & MORSE_USE_SEPARATORS) > 0);
        }
    }

    free(tmp);
}
