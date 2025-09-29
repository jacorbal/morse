# `morse` -- Morse encoder and decoder C library

Encode text messages into Morse code using a binary search tree (AVL
tree) structure.  It supports customizable encoding options, including
separators and prosigns, to facilitate Morse code transmission and
decoding.

## Features

  - Initialize and destroy Morse code binary trees.
  - Encode strings into Morse code with configurable options.
  - Support for Morse separators (spaces between characters and words).
  - Support for prosigns such as "SK", "AR", and "CT" for special
    transmission signals.
  - Uses an AVL tree for efficient character lookup and encoding based
    on Morse priority.

## Data structure

  - **`morse_tree_td`.**  An AVL binary search tree representing Morse
    code characters and their encoding.

## Usage

### Initialization

Create a new Morse code tree:

        morse_tree_td *morse_tree = morse_init();
        if (morse_tree == NULL) {
            /* Handle error */
        }

### Encoding a message

Encode an ASCII string into Morse code:

        char encoded[MORSE_MESSAGE_MAX_LENGTH];
        const char *inpupt = "Hello, World";

        if (morse_encode(morse_tree, morse_message, input,
                     MORSE_USE_SEPARATORS | MORSE_USE_PROSIGNS) != 0) {
            fprintf(stderr, "Encoding failed\n");
            morse_destroy(morse);
            return -1;
        }
        printf("Morse code: %s\n", encoded);

### Decoding a message

Decode a Morse code string into ASCII:

    char decoded[MORSE_MESSAGE_MAX_LENGTH + 1] = {'\0'};
    const char *input = "... --- ... / .- .-."; /* "SOS AR" (SOS AR) */

    if (morse_decode(morse, decoded, input, MORSE_USE_SEPARATORS) != 0) {
        fprintf(stderr, "Decoding failed\n");
        morse_destroy(morse);
        return -1;
    }
    printf("Decoded text: '%s'\n", decoded);

### Flags

  - **`MORSE_NO_FLAGS`**.  No special features.
  - **`MORSE_USE_SEPARATORS`**.  Use spaces to separate characters and
    words.
  - **`MORSE_USE_PROSIGNS`**.  Include prosigns like "AR", "SK", "CT" at
    start and end.

### Cleanup

Destroy the Morse tree when done:

        morse_destroy(morse_tree);

## Constants and macros

  - **`MORSE_MAX_NODES`.**  Maximum number of nodes in the Morse tree.
  - **`MORSE_MESSAGE_MAX_LENGTH`.**  Maximum length of the Morse
    message.
  - **`MORSE_DIT` / `MORSE_DAH`.**  Representations of dot ("dit") and
    dash ("dah").
  - **Separator constants.**  Define spacing between characters and
    words.
  - **Prosigns.** Special Morse signals such as "AR", "SK", "CT".

## Notes

  - The Morse tree is organized according to Morse code priority, stored
    as an AVL tree for efficient encoding.
  - Supports encoding of alphabetic characters, digits, and some
    punctuation.
  - Punctuation characters are ignored in tree construction but
    influence insertion order.

## License

  - ISC License
  - Copyright (c) 2025, J. A. Corbal
  - Read the file [`LICENSE`](LICENSE) for more details
