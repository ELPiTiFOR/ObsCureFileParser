#include "business.h"

#include "it_file.h"
#include "utils.h"

int add_item_to_it_b(char **argv, size_t i)
{
    it_file *it = parse_it_file(argv[i + 1]);
    if (!it)
    {
        return 1;
    }

    uint32_t item_id = my_atoi_base(argv[i + 2], 16);
    uint32_t item_loc = my_atoi_base(argv[i + 3], 16);
    uint32_t extra_info = my_atoi_base(argv[i + 4], 16);
    uint32_t multiplier = my_atoi_base(argv[i + 5], 16);
    uint32_t diff_mode = my_atoi_base(argv[i + 6], 16);
    it_item item =
    {
        item_id,
        item_loc,
        extra_info,
        multiplier,
        diff_mode
    };

    if (add_item_to_it(it, &item))
    {
        return 1;
    }

    if (serialize_it_file(it, argv[i + 7]))
    {
        return 1;
    }

    return 0;
}

int remove_item_from_it_b(char **argv, size_t i)
{
    it_file *it = parse_it_file(argv[i + 1]);
    if (!it)
    {
        return 1;
    }

    uint32_t item_loc = my_atoi_base(argv[i + 2], 16);

    if (remove_item_from_it(it, item_loc))
    {
        return 1;
    }

    if (serialize_it_file(it, argv[i + 3]))
    {
        return 1;
    }

    return 0;
}