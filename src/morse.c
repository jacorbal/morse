/**
 * @file morse.c
 *
 * @brief Morse code implementation using a binary search tree
 *
 * @author J. A. Corbal (<jacorbal@gmail.com>)
 */

/* Data type includes */
#include <ctype.h>
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
static int s_compare(const void *key1, const void *key2)
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


/**
 * @brief Trim leading and trailing whitespace characters from
 *        a C string in-place
 *
 * @param s  Pointer to a null-terminated C string to trim
 *
 * @note The function only removes ASCII space ' ' as used by
 *       @e MORSE_SEP/...
 */
void s_trim(char *s)
{
    char *start;
    char *end;

    if (s == NULL) {
        return;
    }

    /* Pointer to the first non-space character (leading) */
    start = s;
    while (*start && isspace((unsigned char) *start)) {
        start++;
    }

    /* If the string contains only spaces, make it an empty string */
    if (*start == '\0') {
        s[0] = '\0';
        return;
    }

    /* Pointer to the end of the useful portion */
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char) *end)) {
        end--;
    }
    /* 'end' points to the last non-space character; place terminator */
    *(end + 1) = '\0';

    /* If 'start' is not at the beginning, shift the substring to the
     * start of the buffer */
    if (start != s) {
        char *dst = s;
        while ((*dst++ = *start++) != '\0') {
            /* Copy including terminator */
        }
    }
}


/* Fill the Morse tree with the alphabet */
static void s_morse_generate_nodes(morse_tree_td *morse)
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
static int s_morse_encode_char(const morse_tree_td *morse,
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
        retval = s_morse_encode_char(morse, bitree_left(node), data, dst,
                use_separators);
    } else if (cmpval > 0) {
        /* Move to the right after adding a 'dah' (dash) */
        strcat(dst, MORSE_DAH);
        if (use_separators) {
            strcat(dst, MORSE_SEP);
        }
        retval = s_morse_encode_char(morse, bitree_right(node), data, dst,
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


/**
 * @brief Decode a single Morse string into its character
 *
 * @param morse     Morse tree
 * @param morse_str String with the Morse code for one character
 * @param dst       Pointer to a char where the decoded character will
 *                  be stored
 *
 * @return Status of the lookup operation
 * @retval  0 The character was decoded successfully and stored in @p dst
 * @retval -1 The code is invalid (path does not exist or node is hidden)
 *
 * @note This function walks the same binary tree used for encoding:
 *       each 'dit' (@e MORSE_DIT[0]) moves to the left child and each
 *       'dah' (@e MORSE_DAH[0]) moves to the right child.
 */
static int s_morse_decode_char(const morse_tree_td *morse,
        const char *morse_str, char *dst)
{
    const bitree_node_td *node;

    if (morse == NULL || morse_str == NULL || dst == NULL) {
        return -1;
    }

    node = bitree_root(morse);
    if (bitree_is_eob(node)) {
        return -1;
    }

    for (size_t i = 0; morse_str[i] != '\0'; ++i) {
        char c = morse_str[i];

        /* Ignore separators characters if they appear inside the token */
        if (c == MORSE_SEP[0]) {
            continue;
        } else if (c == MORSE_DIT[0]) {
            node = bitree_left(node);
        } else if (c == MORSE_DAH[0]) {
            node = bitree_right(node);
        } else {
            /* Unexpected character in morse_str */
            return -1;
        }

        if (bitree_is_eob(node)) {
            return -1;
        }
    }

    if (bistree_is_hidden(node)) {
        return -1;
    }

    *dst = *(char *) bistree_data(node);
    return 0;
}


/* Initialize a new Morse tree */
morse_tree_td *morse_init(void)
{
    morse_tree_td *morse;

    morse = bistree_init(s_compare, free);
    if (morse == NULL) {
        return NULL;
    }

    s_morse_generate_nodes(morse);

    return morse;
}


/* Encode a entire string until the 'NULL' character is found */
// TODO: Consider using 'strtok' to separate words
int morse_encode(const morse_tree_td *morse,
        char *dst, const char *src, uint8_t flags)
{
    char *data;
    char *tmp;
    size_t src_len;

    if (morse == NULL || dst == NULL || src == NULL) {
        return -1;
    }

    tmp = malloc(sizeof(char));
    if (tmp == NULL) {
        return -1;
    }

    src_len = strlen(src);
    dst[0] = '\0';

    /* Start the transmission: add prosign <CT> */
    if (flags & MORSE_USE_PROSIGNS) {
        for (size_t i = 0; i < strlen(MORSE_PROSIGN_CT); ++i) {
            *tmp = MORSE_PROSIGN_CT[i];
            data = tmp;
            if (s_morse_encode_char(morse, bitree_root(morse), data, dst,
                    (flags & MORSE_USE_SEPARATORS) > 0) != 0) {
                return -1;
            }
        }

        if (flags & MORSE_USE_SEPARATORS) {
            strcat(dst, MORSE_WORD_SEPARATOR);
        }
    }

    /* Send the transmission */
    for (size_t i = 0; i < src_len; ++i) {
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

            if (s_morse_encode_char(morse, bitree_root(morse), data, dst,
                    (flags & MORSE_USE_SEPARATORS) > 0) != 0) {
                return -1;
            }
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
            if (s_morse_encode_char(morse, bitree_root(morse), data, dst,
                    (flags & MORSE_USE_SEPARATORS) > 0) != 0){
                return -1;
            }
        }
    }

    free(tmp);
    return 0;
}


/* Decode a full Morse message */
int morse_decode(const morse_tree_td *morse,
        char *dst, const char *src, uint8_t flags)
{
    char token[MORSE_MESSAGE_MAX_LENGTH + 1];
    size_t tok_pos = 0;
    size_t out_pos = 0;
    size_t i = 0;
    size_t src_len;

    if (morse == NULL || dst == NULL || src == NULL) {
        return -1;
    }

    src_len = strlen(src);
    dst[0] = '\0';

    while (i < src_len && out_pos < MORSE_MESSAGE_MAX_LENGTH) {
        size_t run;

        /* If separators mode is enabled, check for word separator first */
        if ((flags & MORSE_USE_SEPARATORS) &&
                i + strlen(MORSE_WORD_SEPARATOR) <= src_len &&
                strncmp(&src[i], MORSE_WORD_SEPARATOR,
                    strlen(MORSE_WORD_SEPARATOR)) == 0) {
            /* finalize current token */
            if (tok_pos > 0) {
                char decoded;
                token[tok_pos] = '\0';
                if (s_morse_decode_char(morse, token, &decoded) == 0) {
                    dst[out_pos++] = decoded;
                }
                tok_pos = 0;
            }
            /* append space as word separator */
            dst[out_pos++] = ' ';
            i += strlen(MORSE_WORD_SEPARATOR);
            continue;
        }

        /* If separators mode is enabled, check for character separator */
        if ((flags & MORSE_USE_SEPARATORS) &&
                i + strlen(MORSE_CHAR_SEPARATOR) <= src_len &&
                strncmp(&src[i], MORSE_CHAR_SEPARATOR,
                    strlen(MORSE_CHAR_SEPARATOR)) == 0) {
            if (tok_pos > 0) {
                char decoded;
                token[tok_pos] = '\0';
                if (s_morse_decode_char(morse, token, &decoded) == 0) {
                    dst[out_pos++] = decoded;
                }
                tok_pos = 0;
            }
            i += strlen(MORSE_CHAR_SEPARATOR);
            continue;
        }

        /* Non-separators mode: single space separates characters; two
         * or more spaces separate words */
        if (!(flags & MORSE_USE_SEPARATORS) && src[i] == ' ') {
            if (tok_pos > 0) {
                char decoded;
                token[tok_pos] = '\0';
                if (s_morse_decode_char(morse, token, &decoded) == 0) {
                    dst[out_pos++] = decoded;
                }
                tok_pos = 0;
            }
            /* Count consecutive spaces to detect word boundary */
            run = 1;
            while (i + run < src_len && src[i + run] == ' ') {
                ++run;
            }
            if (run >= 2) {
                /* treat as word separator */
                dst[out_pos++] = ' ';
            }
            i += run;
            continue;
        }

        /* Accept only 'MORSE_DIT', 'MORSE_DAH' or single-space
         * separators inside token */
        if (src[i] == MORSE_DIT[0] || src[i] == MORSE_DAH[0] ||
                src[i] == MORSE_SEP[0]) {
            if (tok_pos < sizeof(token) - 1) {
                token[tok_pos++] = src[i];
            } else {
                /* token too long: drop it (robustness) */
                tok_pos = 0;
            }
        }
        /* Ignore any other characters */
        ++i;
    }

    /* Process final token if any */
    if (tok_pos > 0 && out_pos < MORSE_MESSAGE_MAX_LENGTH) {
        char decoded;
        token[tok_pos] = '\0';
        if (s_morse_decode_char(morse, token, &decoded) == 0) {
            dst[out_pos++] = decoded;
        }
    }

    dst[out_pos] = '\0';
    s_trim(dst);

    return 0;
}
