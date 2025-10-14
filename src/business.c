#include "business.h"

#include <string.h>

#include "it_file.h"
#include "tm_file.h"
#include "utils.h"

int parse_it_file_b(char **argv, size_t i)
{
    it_file *it = parse_it_file(argv[i + 1]);
    if (!it)
    {
        return 1;
    }

    print_it_file(it);
    free_it_file(it);
    return 0;
}

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

int parse_tm_file_b(char **argv, size_t i)
{
    tm_file *tm = parse_tm_file(argv[i + 1]);
    if (!tm)
    {
        return 1;
    }

    print_tm_file(tm);
    free_tm_file(tm);
    return 0;
}

int add_item_to_tm_b(char **argv, size_t i)
{
    tm_file *tm = parse_tm_file(argv[i + 1]);
    if (!tm)
    {
        return 1;
    }

    uint8_t info_diff[] = { 0x31, 0x2F, 0x31, 0x2F, 0x37, 0 };
    tm_item_section item =
    {
        0, //
        0, //
        my_atoi_base(argv[i + 2], 16),
        my_atoi_base(argv[i + 3], 16),
        my_atoi_base(argv[i + 4], 16),
        my_atoi_base(argv[i + 5], 16),
        my_atoi_base(argv[i + 6], 16),
        my_atoi_base(argv[i + 7], 16),
        my_atoi_base(argv[i + 8], 16),
        my_atoi_base(argv[i + 9], 16),
        { 0 },
        strlen(argv[i + 10]),
        argv[i + 10],
    };

    if (add_item_to_tm(tm, &item))
    {
        fprintf(stderr, "ERROR: Couldn't add item to tm\n");
        return 1;
    }

    if (serialize_tm_file(tm, argv[1 + 11]))
    {
        fprintf(stderr, "ERROR: Couldn't add item to tm\n");
        return 1;
    }

    print_tm_file(tm);
    free_tm_file(tm);
    return 0;
}

int remove_item_from_tm_b(char **argv, size_t i)
{
    tm_file *tm = parse_tm_file(argv[i + 1]);
    if (!tm)
    {
        return 1;
    }

    if (remove_item_from_tm(tm, my_atoi_base(argv[i + 2], 16)))
    {
        fprintf(stderr, "ERROR: Couldn't add item to tm\n");
        return 1;
    }

    if (serialize_tm_file(tm, argv[1 + 3]))
    {
        fprintf(stderr, "ERROR: Couldn't add item to tm\n");
        return 1;
    }

    print_tm_file(tm);
    free_tm_file(tm);
    return 0;
}