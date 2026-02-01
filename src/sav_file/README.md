# sav_file

Here are all the functions to parse, serialize, print, etc, `.sav` files, in
other words, the save files of ObsCure.

The structures and the main functions prototypes are in the `sav_file.h` header
file. These functions call other functions that dore more specific stuff on the
inventory, the sections of the playable characters (PCs), the weapons
inventories, etc. These "other functions" are in the other files, like
`sav_inv.h` or `sva_pc.h`.

These compilation units use functions and variables from `src/it_file` (the
folder where the parsing functions for `.it` files can be found) and from
`src/obscure_ids` (a folder with some useful information about ObsCure in
general, like Item IDs, Document IDs, etc).

# config
You can create a `SAV.config` file in the same directory as the executable and
include these values:
`verbose-print=true` (or `false`)