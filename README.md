# ObsCureFileParser
This project aims to provide a way to view and modify the files of the first
ObsCure game.

# Progress
The final goal is to have a CLI and a GUI that lets the user modify the files
of the game. As of today, we can:

- Parse and print a big part of each of these file formats: SAV, IT, TM and HOE.
- Use a GUI to open, modify and save an IT file.

We don't know yet the whole format of SAV, TM and HOE files, therefore more
research is needed.

There are also other possible actions, check the end of the "Usage" section.

# Usage
To parse and view the contents of a file, here's the regex of the command:

`.\ObsCureFileParser --(hoe|sav|it|tm)-parse <file_path>`

For example, to parse and print a save file, type:

`.\ObsCureFileParser --sav-parse <file_path>`

The `<file_path>` must be the absolute or relative path to a file of the
specified format, in this case, a `.sav` file.

If you want to check all the possible options and you are comfortable with the C
syntax, read the `src/commands.c` file to know which options perform which
actions.