/**
 * @file main.c
 *
 * @brief Example of Morse encoding implementation
 */

/* Data type includes */
#include <stdbool.h>

/* System includes */
#include <stdio.h>
#include <stdlib.h>

/* ADT includes */
#include <adt/bistree.h>

/* Project includes */
#include <morse.h>


#ifdef DEBUG
static void s_morse_print(const bitree_node_td *node, const int level)
{
    char c;

    if (!bitree_is_eob(node)) {
        if (!bitree_is_eob(bitree_right(node))) {
            s_morse_print(bitree_right(node), level + 4);
        }

        c = *(const char *) bistree_data(node);
        if (c == '[' || c == ']' || c == '(' || c == ')') {
            c = ' ';
        }
        printf("%*s %c%c%c\n", level, "->",
                (bistree_is_hidden(node)) ? '[' : '{', c,
                (bistree_is_hidden(node)) ? ']' : '}');

        if (!bitree_is_eob(bitree_left(node))) {
            s_morse_print(bitree_left(node), level + 4);
        }
    }
}
#endif  /* ! DEBUG */


/* Main entry */
int main(void)
{
    morse_tree_td *morse;
    char encoded[MORSE_MESSAGE_MAX_LENGTH] = {'\0'};

    morse = morse_init();
    if (morse == NULL) {
        return 1;
    }

#ifdef DEBUG
    printf("Morse tree:\n");
    s_morse_print(bitree_root(morse), 2);
    printf("Size: %lu\n", bistree_size(morse));
    //printf("Root: %c\n", *(char *) bistree_data(bitree_root(morse)));
    //printf("Fctr: %d\n", bistree_factor(bitree_root(morse)));
#endif  /* ! DEBUG */

    morse_encode(morse, encoded, "What hath God wrought",
            MORSE_USE_SEPARATORS | MORSE_USE_PROSIGNS);
    printf("%s\n", encoded);

    morse_destroy(morse);

    return 0;
}
