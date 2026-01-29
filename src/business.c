#include "business.h"

#include <stdlib.h>
#include <string.h>

#include "it_file.h"
#include "tm_file.h"
#include "sav_file.h"
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

int edit_item_in_it_b(char **argv, size_t i)
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

    if (edit_item_in_it(it, &item))
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

int item_info_from_it_b(char **argv, size_t i)
{
    it_file *it = parse_it_file(argv[i + 1]);
    if (!it)
    {
        return 1;
    }

    uint32_t item_loc = my_atoi_base(argv[i + 2], 16);

    it_item item = {0};
    if (get_item_from_loc(it, item_loc, &item))
    {
        return 1;
    }

    printf("Printing found item...\n");
    print_it_item(&item);

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
        my_atoi_base(argv[i + 2], 16), // item_id
        my_atoi_base(argv[i + 3], 16), // item_loc
        my_atoi_base(argv[i + 4], 16), // x
        my_atoi_base(argv[i + 5], 16), // y
        my_atoi_base(argv[i + 6], 16), // z
        ///*
        {
            {
            str_to_float(argv[i + 7], strlen(argv[i + 7])),  // rot_matrix[0]
            str_to_float(argv[i + 8], strlen(argv[i + 8])),  // rot_matrix[1]
            str_to_float(argv[i + 9], strlen(argv[i + 9])),  // rot_matrix[2]
            },
            {
            str_to_float(argv[i + 10], strlen(argv[i + 10])), // rot_matrix[3]
            str_to_float(argv[i + 11], strlen(argv[i + 11])), // rot_matrix[4]
            str_to_float(argv[i + 12], strlen(argv[i + 12])), // rot_matrix[5]
            },
            {
            str_to_float(argv[i + 13], strlen(argv[i + 13])), // rot_matrix[6]
            str_to_float(argv[i + 14], strlen(argv[i + 14])), // rot_matrix[7]
            str_to_float(argv[i + 15], strlen(argv[i + 15])), // rot_matrix[8]
            },
        },
        //*/
        /*
        {
            {
            strtof(argv[i + 7], NULL),  // rot_matrix[0]
            strtof(argv[i + 8], NULL),  // rot_matrix[1]
            strtof(argv[i + 9], NULL),  // rot_matrix[2]
            },
            {
            strtof(argv[i + 10], NULL),  // rot_matrix[3]
            strtof(argv[i + 11], NULL),  // rot_matrix[4]
            strtof(argv[i + 12], NULL),  // rot_matrix[5]
            },
            {
            strtof(argv[i + 13], NULL),  // rot_matrix[6]
            strtof(argv[i + 14], NULL),  // rot_matrix[7]
            strtof(argv[i + 15], NULL),  // rot_matrix[8]
            },
        },
        */
        //{ 0 },
        strlen(argv[i + 16]),
        argv[i + 16],
    };

    if (add_item_to_tm(tm, &item))
    {
        fprintf(stderr, "ERROR: Couldn't add item to tm\n");
        return 1;
    }

    if (serialize_tm_file(tm, argv[1 + 17]))
    {
        fprintf(stderr, "ERROR: Couldn't add item to tm\n");
        return 1;
    }

    print_tm_file(tm);
    free_tm_file(tm);
    return 0;
}

int edit_item_in_tm_b(char **argv, size_t i)
{
    tm_file *tm = parse_tm_file(argv[i + 1]);
    if (!tm)
    {
        return 1;
    }

    tm_item_section item =
    {
        0, //
        0, //
        my_atoi_base(argv[i + 2], 16), // item_id
        my_atoi_base(argv[i + 3], 16), // item_loc
        my_atoi_base(argv[i + 4], 16), // x
        my_atoi_base(argv[i + 5], 16), // y
        my_atoi_base(argv[i + 6], 16), // z
        ///*
        {
            {
            str_to_float(argv[i + 7], strlen(argv[i + 7])),  // rot_matrix[0]
            str_to_float(argv[i + 8], strlen(argv[i + 8])),  // rot_matrix[1]
            str_to_float(argv[i + 9], strlen(argv[i + 9])),  // rot_matrix[2]
            },
            {
            str_to_float(argv[i + 10], strlen(argv[i + 10])), // rot_matrix[3]
            str_to_float(argv[i + 11], strlen(argv[i + 11])), // rot_matrix[4]
            str_to_float(argv[i + 12], strlen(argv[i + 12])), // rot_matrix[5]
            },
            {
            str_to_float(argv[i + 13], strlen(argv[i + 13])), // rot_matrix[6]
            str_to_float(argv[i + 14], strlen(argv[i + 14])), // rot_matrix[7]
            str_to_float(argv[i + 15], strlen(argv[i + 15])), // rot_matrix[8]
            },
        },
        //*/
        /*
        {
            {
            strtof(argv[i + 7], NULL),  // rot_matrix[0]
            strtof(argv[i + 8], NULL),  // rot_matrix[1]
            strtof(argv[i + 9], NULL),  // rot_matrix[2]
            },
            {
            strtof(argv[i + 10], NULL),  // rot_matrix[3]
            strtof(argv[i + 11], NULL),  // rot_matrix[4]
            strtof(argv[i + 12], NULL),  // rot_matrix[5]
            },
            {
            strtof(argv[i + 13], NULL),  // rot_matrix[6]
            strtof(argv[i + 14], NULL),  // rot_matrix[7]
            strtof(argv[i + 15], NULL),  // rot_matrix[8]
            },
        },
        */
        //{ 0 },
        strlen(argv[i + 16]),
        argv[i + 16],
    };

    if (edit_item_in_tm(tm, &item))
    {
        fprintf(stderr, "ERROR: Couldn't edit item in tm\n");
        return 1;
    }

    if (serialize_tm_file(tm, argv[1 + 17]))
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

int parse_sav_file_b(char **argv, size_t i)
{
    sav_file *sav = parse_sav_file(argv[i + 1]);
    if (!sav)
    {
        fprintf(stderr, "ERROR: Couldn't parse sav file\n");
        return 1;
    }

    print_sav_file(sav);

    return 0;
}

int reserialize_sav_file_b(char **argv, size_t i)
{
    sav_file *sav = parse_sav_file(argv[i + 1]);
    if (!sav)
    {
        fprintf(stderr, "ERROR: Couldn't parse sav file\n");
        return 1;
    }

    print_sav_file(sav);
    serialize_sav_file(sav, argv[i + 2]);

    return 0;
}

int test(char **argv, size_t i)
{
    sav_file *sav = parse_sav_file("E:\\Archivos de programa (x86)\\Steam\\userdata\\449564145\\254460\\remote\\game0.sav");
    if (!sav)
    {
        fprintf(stderr, "ERROR: Couldn't parse sav file\n");
        return 1;
    }

    sav->pcs[4].room = 0xC1;
    serialize_sav_file(sav, "E:\\Archivos de programa (x86)\\Steam\\userdata\\449564145\\254460\\remote\\game0.sav");
}