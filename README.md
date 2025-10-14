# ObsCureFileParser
This project aims to provide a way to modify the files of ObsCure.

# Usage
## IT Format
All the item IDs, item locations, extra_infos, multipliers and diff_modes must
be provided in their hexadecimal representation without spaces.

### Viewing an `.it` file
To view the contents of an `.it` file:
```
ObsCureFileParser.exe --it-parse <path_src>
```

### Adding an item
Add an item to an `.it` file:
```
ObsCureFileParser.exe --it-add <path_src> <item_id> <item_loc> <extra_info> \
    <multiplier> <diff_mode> <path_dst>
```
For example:
```
ObsCureFileParser.exe --it-add "C:\allitems.it" 0264 020407 0 \
    1 7 "C:\allitems2.it"
```
### Removing an item
Remove an item from an `.it` file:
```
ObsCureFileParser.exe --it-remove <path_src> <item_loc> <path_dst>
```

## TM Format
The item IDs, item locations, coordinates and rotations must be provided in
their hexadecimal representation without spaces. The info_diff must be provided
as a string.

### Viewing an `.tm` file
To view the contents of an `.tm` file:
```
ObsCureFileParser.exe --tm-parse <path_src>
```

### Adding an item
Add an item to an `.tm` file:
```
ObsCureFileParser.exe --tm-add <path_src> <item_id> <item_loc> \
    <x_pos> <y_pos> <z_pos> \
    <x_rot> <y_rot> <z_rot> \
    <info_diff> \
    <path_dst>
```
For example:
```
ObsCureFileParser.exe --tm-add "C:\xyyy_n.tm" 0264 020407 \
    0 0 0 \
    0 0 0 \
    "1/1/7" \
    "C:\xyyy_n2.tm"
```
### Removing an item
Remove an item from an `.tm` file:
```
ObsCureFileParser.exe --tm-remove <path_src> <item_loc> <path_dst>
```