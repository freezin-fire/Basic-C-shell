# GLSH - Generating a Lazy Shell

## Overview

GLSH is a basic terminal shell implementation written in C, and it is the subject of a
tutorial on [Stephen Brennan's website][1]

It demonstrates the basics of how a shell works. That is: read, parse, fork, exec, and wait. Since its purpose is demonstration (not feature completeness or even fitness for casual use), it has many limitations, including:

-   Commands must be on a single line.
-   Arguments must be separated by whitespace.
-   No quoting arguments or escaping whitespace.
-   No piping or redirection.
-   Only builtins are:  `cd`,  `help`,  `exit`.
-   Runs on Linux and Mac OS X systems only

## Installation
**Compile the source code**:
   ```sh
   gcc -o glsh main.c
   ```
   **Running the shell**:
   ```sh
   ./glsh
   ```

## License

This project is released under the MIT License. See `LICENSE` for more details.

Feel free to modify and expand this README section according to your project's specific details and requirements.

[1]: http://brennan.io/2015/01/16/write-a-shell-in-c/
