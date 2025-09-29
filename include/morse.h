/**
 * @file morse.h
 *
 * @brief Morse code routines declaration using a binary search tree
 *
 * @author J. A. Corbal (<jacorbal@gmail.com>)
 */
/* Morse code basics
 *
 * International table:
 *
 *    A: .-    N: -.    0: -----    The length of the dot is one unit
 *    B: -...  O: ---   1: .----    A dash is three units
 *    C: -.-.  P: .--.  2: ..---    The space between parts of the
 *    D: -..   Q: --.-  3: ...--        same letter is one unit
 *    E: .     R: .-.   4: ....-    The space between two letters is
 *    F: ..-.  S: ...   5: .....        three units
 *    G: --.   T: -     6: -....    The space between two words is
 *    H: ....  U: ..-   7: --...        seven units
 *    I: ..    V: ...-  8: ---..
 *    J: .---  W: .--   9: ----.
 *    K: -.-   X: -..- [.: .-.-.-]
 *    L: .-..  Y: -.-- [,; --..--]
 *    M: --    Z: --.. [?: ..--..]
 *
 * Basic tree:
 * @code
 *
 *                      dit <- ROOT -> dah
 *                ┌─────────────┴┴─────────────┐
 *                E                            T
 *          ┌─────┴──────┐              ┌──────┴─────┐
 *          I            A              N            M
 *       ┌──┴──┐      ┌──┴───┐       ┌──┴──┐     ┌───┴──┐
 *       S     U      R      W       D     K     G      O
 *     ┌─┴─┐ ┌─┴─┐  ┌─┴─┐  ┌─┴─┐   ┌─┴─┐  ┌┴┐  ┌─┴─┐  ┌─┴─┐
 *     H   V F  (*) L  (*) P   J   B   X  C Y  Z   Q (*) (*)
 *    ┌┴┐  └┐    └┐    ┌┘      └┐ ┌┴┐ ┌┘      ┌┘     ┌┘  ┌┴┐
 *    5 4   3     2    +        1 6 = /       7      8   9 0
 *
 * @endcode
 * where '(*)' denotes an unused node.
 *
 * Extended tree:
 *
 * Some symbols were added with time (latest was the commat, '@',
 * approved in 2002 with the name "arobase", and added in 2004).
 * @code
 *
 *                         dit <- ROOT -> dah
 *                 ┌───────────────┴┴───────────────┐
 *                 E                                T
 *         ┌───────┴───────┐               ┌────────┴────────┐
 *         I               A               N                 M
 *     ┌───┴───┐       ┌───┴───┐       ┌───┴──┐        ┌─────┴─────┐
 *     S       U       R       W       D      K        G           O
 *   ┌─┴─┐   ┌─┴─┐   ┌─┴─┐   ┌─┴─┐   ┌─┴─┐ ┌──┴──┐  ┌──┴───┐     ┌─┴─┐
 *   H   V   F  (§)  L  (§)  P   J   B   X C     Y  Z      Q    (§) (§)
 *  ┌┴┐  └┐ ┌┘ ┌─┴─┐ └┐ ┌┘   └┐  └┐ ┌┴┐ ┌┘ └┐   ┌┘┌─┴─┐  ┌─┴─┐  ┌┘  ┌┴┐
 *  5 4   3 É (§)  2 (*)+    (§)  1 6 = /  (*)  ( 7  (*)(§) (§) 8   9 0
 *           ┌─┴─┐   ┌┘ └┐   ┌┘  ┌┘ └┐    ┌─┴─┐ └┐    └┐       ┌┘
 *           ?  (§)  "   .   @   '   -    ;  (§) )     ,       :
 *
 * @endcode
 *
 * where '(§)' denotes a regional symbol not contemplated in the current
 * recommendation of International Morse code (ITU-R M.1677-1), but
 * usually they vary depending on the country.  The accented 'É',
 * nonetheless, is accepted in the international regulations.
 */

#ifndef MORSE_H
#define MORSE_H

/* Data type includes */
#include <stdbool.h>
#include <stdint.h>

/* ADT includes */
#include <adt/bistree.h>


/* Macros */
#define MORSE_MAX_NODES (45)

/* Flags */
#define MORSE_NO_FLAGS (0)
#define MORSE_USE_SEPARATORS (1 << 0)
#define MORSE_USE_PROSIGNS   (1 << 1)

/* Message representation */
#define MORSE_MESSAGE_MAX_LENGTH (500)  /* Max. characters of transmission */
#define MORSE_DIT "."                   /* A dot, or "dit" (.) */
#define MORSE_DAH "-"                   /* A dash, or "dah" (-)  */
#define MORSE_SEP " "                   /* Separator between transmissions */
#define MORSE_CHAR_SEPARATOR "  "       /* Space between letters is 3 (-1) */
#define MORSE_WORD_SEPARATOR "      "   /* Space between words is 7 (-1) */

/* Morse priority order (low to high) according to Morse code as AVL-tree */
/* NOTE.  Punctuation symbols {'[', ']', '(', ')', '{', '}',...} (except
 * {'+', '=', '/'}, which are part of the original International Morse
 * table) will be ignored, but needed to establish a priority on nodes
 * that do nothing, so those dependant on them will be properly
 * inserted.  The AVL tree do not accept repetitions, only unique
 * characters, so there's a need to insert some dummy filler symbols to
 * occupy those positions.  The root node is also not used, and it's
 * represented by '~' */
#define MORSE_WEIGHTED_NODES "5H4SV3IFU[2ELR+]APWJ1~6B=D/XNCKYT7ZGQM8(O9)0"

/* Prosigns are sent without the normal inter-character spacing */
#define MORSE_PROSIGN_CT "CT"   /* <CT>=<KA>: Start of transmission */
#define MORSE_PROSIGN_AR "AR"   /* <AR>=<RN>: End of transmission */
#define MORSE_PROSIGN_SK "SK"   /* <SK>=<VA>: End of work */


/**
 * @brief Declare a Morse tree as a binary search tree with AVL nodes
 */
typedef bistree_td morse_tree_td;


/* Public interface */
/**
 * @brief Initialize the Morse tree
 *
 * @return New allocated Morse binary search tree, or @c NULL otherwise
 */
morse_tree_td *morse_init(void);

/**
 * @brief Encode a entire string until the @c NULL character is found
 *
 * @param morse Morse tree
 * @param dst   Pointer to the string in Morse code
 * @param src   String to be encoded into Morse
 * @param flags Parsing flags:
 *      - @e MORSE_NO_FLAGS: do not use any flag
 *      - @e MORSE_USE_SEPARATORS: use word and character separators
 *      - @e MORSE_USE_PROSIGNS: add prosigns to start and end transmission
 *
 * @return 0 on success, or -1 on invalid parameters
 *
 * @note The string @e encoded points to the encoded string upon return
 *
 * @todo The final string @e dst has to be trimmed (no trailing spaces)
 */
int morse_encode(const morse_tree_td *morse,
        char *dst, const char *src, uint8_t flags);

/**
 * @brief Decode a full Morse transmission string into plain text
 *
 * @param morse Morse tree
 * @param dst   Output buffer for decoded text
 * @param src   Input Morse string (may contain separators as defined)
 * @param flags Parsing flags:
 *      - @e MORSE_NO_FLAGS: do not use any flag
 *      - @e MORSE_USE_SEPARATORS: use word and character separators
 *      - @e MORSE_USE_PROSIGNS: add prosigns to start and end transmission
 *
 * @return 0 on success, or -1 on invalid parameters
 *
 * @note Output buffer must be at least @e MORSE_MESSAGE_MAX_LENGTH + 1
 * @note Decoded string is trimmed of leading and trailing whitespaces
 * @note If @e MORSE_USE_SEPARATORS is set, this function treats the
 *       multi-space strings @e MORSE_CHAR_SEPARATOR and
 *       @e MORSE_WORD_SEPARATOR as delimiters.  If not set, single
 *       space ' ' separates characters and a run of two or more spaces
 *       is treated as a word separator.
 */
int morse_decode(const morse_tree_td *morse,
        char *dst, const char *src, uint8_t flags);

/**
 * @brief Destroy the Morse binary tree
 *
 * @see bistree_destroy
 */
#define morse_destroy bistree_destroy


#endif  /* ! MORSE_H */
