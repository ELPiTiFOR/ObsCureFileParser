# ObsCureFileParser
This project aims to provide a way to modify the files of ObsCure.

# Usage
## IT Format
All the item IDs, item locations, extra_infos, multipliers and diff_modes must
be provided in their hexadecimal representation without spaces.

### Adding an item
Add an item to an `.it` file:
```
ObsCureFileParser --it-add <path_src> <item_id> <item_loc> <extra_info> \
    <multiplier> <diff_mode> <path_dst>
```
For example:
```
ObsCureFileParser --it-add "C:\allitems.it" 0264 020407 0 \
    1 7 "C:\allitems2.it"
```
### Removing an item
Remove an item from an `.it` file:
```
ObsCureFileParser --it-remove <path_src> <item_loc> <path_dst>
```