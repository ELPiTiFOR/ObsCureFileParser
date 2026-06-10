#include "business.h"

#include <stdlib.h>
#include <string.h>

#include "it_file.h"
#include "tm_file.h"
#include "sav_file.h"
#include "hoe_file.h"
#include "hoe_event.h"
#include "utils.h"
#include "room.h"
#include "item.h"

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

int parse_hoe_file_b(char **argv, size_t i)
{
    hoe_file *hoe = parse_hoe_file(argv[i + 1]);
    if (!hoe)
    {
        fprintf(stderr, "ERROR: Couldn't parse hoe file\n");
        return 1;
    }

    print_hoe_file(hoe);

    free(hoe);

    return 0;
}

int reserialize_hoe_file_b(char **argv, size_t i)
{
    hoe_file *hoe = parse_hoe_file(argv[i + 1]);
    if (!hoe)
    {
        fprintf(stderr, "ERROR: Couldn't parse hoe file\n");
        return 1;
    }

    serialize_hoe_file(hoe, argv[i + 2]);
    free(hoe);

    return 0;
}

int iterate_through_hoe(char **argv, size_t i)
{
    uint8_t hoe_path[512] = { 0 };
    strcpy(hoe_path, "E:\\Fran\\OBSCURE\\Game Data\\Testing ground\\");
    size_t og_len = strlen(hoe_path);

    // char *str1 = "TM_ItSetVisible";
    // size_t len1 = strlen(str1);
    char *str1 = "\x00\x00\x00\xD0";
    size_t len1 = 4; //strlen(str1);

    // char *str2 = "TM_ItSetContained";
    // size_t len2 = strlen(str2);
    char *str2 = "\x00\x00\x00\xC9";
    size_t len2 = 4; //strlen(str1);

    room_id min_room = 0;
    size_t min_len = 10000000;

    for (int i = 0; i < NB_ROOMS; i++)
    {
        strcat(hoe_path, path_from_ri((room_id)i));
        strcat(hoe_path, "\\");
        strcat(hoe_path, ristr_from_ri((room_id)i));
        strcat(hoe_path, ".hoe");
        hoe_file *hoe = parse_hoe_file(hoe_path);
        printf("Checking %s\n", hoe_path);

        for (size_t j = 0; j < hoe->nb_chunks; j++)
        {
            if (hoe->chunks[j].type == HOE_EVENT)
            {
                hoe_event *event = hoe->chunks[j].content;
                char event_name[512] = {0};
                for (size_t a = 0; a < event->name.length; a++)
                {
                    event_name[a] = event->name.content[a];
                }

                printf("    %s:\n", event_name);
                for (size_t i = 0; i < event->len_bytecode && i < 16; i++)
                {
                    printf("%02X ", event->bytecode[i]);
                }
                putchar('\n');

                // if (event->len_bytecode && event->len_bytecode < min_len
                //     && strcmp("f002", ristr_from_ri(i)) != 0
                //     && strcmp("g100", ristr_from_ri(i)) != 0
                //     && strcmp("m000", ristr_from_ri(i)) != 0
                //     && strcmp("m100", ristr_from_ri(i)) != 0
                //     && strcmp("g016", ristr_from_ri(i)) != 0
                // )
                // {
                //     min_len = event->len_bytecode;
                //     min_room = i;
                // }
                // else
                // {
                //     // printf("len_bytecode (%zu) >= min_len (%zu)\n", event->len_bytecode, min_len);
                // }

                // if (event->nb_uk_ints != 0)
                // {
                //     printf("In %s:\n", event_name);
                //     print_hoe_uk_ints(event);
                // }
                /*
                char *to_search = "cin";
                if (strstr(event_name, to_search))
                {
                    printf("\"%s\" found in %s at %s with %d\n", to_search, event_name, hoe_path, event->nb_uk_ints);
                    // print_hoe_event(event);
                }
                */
                // size_t found_index = 0;
                // if ((found_index = search_in_array(event->bytecode, event->len_bytecode, str2,
                //     len2)) != event->len_bytecode
                //     && (search_in_array(event->bytecode, event->len_bytecode, str1, len1) == event->len_bytecode)
                // )
                // {
                //     printf("    Found str2 in %s in %s\n", hoe_path, event_name);
                //     // uint32_t type = lsb_32(*(uint32_t *)(event->bytecode + found_index + len1 + 0xd));
                //     // uint32_t uid = lsb_32(*(uint32_t *)(event->bytecode + found_index + len1 + 0x1a));
                //     // type = event->hoe_vars[type].ivalue;
                //     // uid = event->hoe_vars[uid].ivalue;
                //     // printf("        type = %08X, uid = %08X\n", type, uid);
                // }
                // if ((found_index = search_in_array(event->bytecode, event->len_bytecode, str2,
                //     len2)) != event->len_bytecode)
                // {
                //     printf("    Found %s in %s\n", str2, hoe_path);
                //     uint32_t type = lsb_32(*(uint32_t *)(event->bytecode + found_index + len2 + 0xd));
                //     uint32_t uid = lsb_32(*(uint32_t *)(event->bytecode + found_index + len2 + 0x1a));
                //     type = event->hoe_vars[type].ivalue;
                //     uid = event->hoe_vars[uid].ivalue;
                //     printf("        type = %08X, uid = %08X\n", type, uid);
                // }
            }
        }

        free_hoe_file(hoe);
        hoe_path[og_len] = 0;
    }

    printf("min_room: %s, %zu\n", ristr_from_ri(min_room), min_len);
}

room_id find_item(uint8_t *obscure_path, uint32_t uid)
{
    uint8_t path[512] = { 0 };
    strcpy(path, obscure_path);
    size_t og_len = strlen(path);

    for (size_t i = 0; i < NB_ROOMS; i++)
    {
        strcat(path, "\\");
        strcat(path, path_from_ri((room_id)i));
        strcat(path, "\\");
        strcat(path, ristr_from_ri((room_id)i));
        size_t len_without_extension = strlen(path);
        strcat(path, "_n.tm");
        tm_file *tm = parse_tm_file(path);

        for (size_t j = 0; j < tm->len_item_sections; j++)
        {
            if (tm->items[j].item_location == uid)
            {
                return (room_id)i;
            }
        }

        free_tm_file(tm);

        path[len_without_extension] = 0;
        strcat(path, "_d.tm");
        tm = parse_tm_file(path);

        if (tm)
        {
            for (size_t j = 0; j < tm->len_item_sections; j++)
            {
                if (tm->items[j].item_location == uid)
                {
                    return (room_id)i;
                }
            }
            free_tm_file(tm);
        }

        path[og_len] = 0;
    }

    return NOROOM;
}

int test(char **argv, size_t i)
{
    iterate_through_hoe(argv, i);
    // room_id ri = find_item("C:\\SteamLibrary\\steamapps\\common\\Obscure", 0x0D0102);
    // if (ri == NOROOM)
    // {
    //     printf("not found\n");
    // }
    // printf("Room: %s\n", ristr_from_ri(ri));
    // sav_file *sav = parse_sav_file("E:\\Archivos de programa (x86)\\Steam\\userdata\\449564145\\254460\\remote\\game1.sav");
    // if (!sav)
    // {
    //     fprintf(stderr, "ERROR: Couldn't parse sav file\n");
    //     return 1;
    // }

    // sav->pcs[KENNY_PC_INDEX].room = 0x19;

    //add_item(sav, MEDKIT, 100);
    /*
    if (add_weapon(sav, JOSH_PC_INDEX, POWERFUL_FLASHLIGHT, 100))
    {
        printf("something went wrong my boy\n");
    }
    */
    //sav->diff_mode = 0x01;
    //sav->shotgun_ammo = 2000;
    //sav->handgun_ammo = 2000;
    //serialize_sav_file(sav, "E:\\Archivos de programa (x86)\\Steam\\userdata\\449564145\\254460\\remote\\game1.sav");
}