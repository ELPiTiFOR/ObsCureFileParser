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

## CLI
To parse and view the contents of a file, here's the regex of the command:

`.\ObsCureFileParser --(hoe|sav|it|tm)-parse <file_path>`

For example, to parse and print a save file, type:

`.\ObsCureFileParser --sav-parse <file_path>`

The `<file_path>` must be the absolute or relative path to a file of the
specified format, in this case, a `.sav` file.

If you want to check all the possible options and you are comfortable with the C
syntax, read the `src/commands.c` file to know which options perform which
actions.

## GUI
Run the `ObsCureFileParserGUI.exe` program, it will open a window. Choose the
format you want to work with. Use the "Load ..." button to open a file of
said format. Each format has a different UI, read the following sections to
learn how to use them. Once you made your changes, use the "Save" button in the
upper right corner of the window to apply them.

### IT

After loading the IT file, you'll see a list of items, you can scroll through it
with the "Top", "Up", "Down", and "Bot" ("Bottom") buttons. "Up" and "Down" will
scroll a page, while "Top" and "Bot" will take you to the first and last page
respectively.

For each item, you can click on the image to change the item type. A new window
will appear and you will have to click on the new type you want. You can click
the cross button in the left part to delete the item. You can change the UID of
the item with the leftmost text box. You can modify the multiplier associated
with the item using the rightmost text box. You can check or uncheck the "S"
(Special), "H" (Hard), "N" (Normal) and "E" (Easy) boxes to decide in which
difficulties and modes the item appears. Sometimes the item has another
field: the extra info. To modify it, click on the middle long button, just
above the "Add" button, this will only work if the item is a map, a document,
or a statuette. Finally, you can use one of the "Add" buttons to add a new item
right next to the one for which you clicked the "Add" button. Click on it, this
will open a new window where you can setup the new item to be added. Once you're
done filling the fields, you can add the new item with "Add Up" and "Add Down",
which will insert it before the original item, and after the original item
respectively.

# More info
For more info on ObsCure formats, types, etc, check
[the documentation](https://elpitifor.github.io/ObsCureDocs).