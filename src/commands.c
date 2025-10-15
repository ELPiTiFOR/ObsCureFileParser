#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "business.h"
#include "it_file.h"

void help()
{
    printf("DESCRIPTION:\n");
    printf("\tRandomizes items in ObsCure.\n");
    printf("WARNING:\n");
    printf("\tThis program modifies your game files. If something goes wrong, the game files could go corrupted\n");
    printf("\tand the game could stop working. You are responsible for the fate of your game files. Think about\n");
    printf("\tmaking a backup of your files just in case. Specially the folder `_levels` and the `allitems.it` file\n");
    printf("\twhich are the files this programs modifies. The paths to each one of them are the ones that follow:\n");
    printf("\t            `$OBS_PATH\\data\\_levels`\n");
    printf("\t            `$OBS_PATH\\data\\_common\\allitems.it`\n");
    printf("\tFor more information on this program, refer to the README.\n");
    printf("USAGE:\n");
    printf("\t.\\ObsCureRandomizer.exe [<options>]\n");
    printf("OPTIONS:\n");
    printf("\t--help: show this help\n");
    printf("\t--room <room_id>: select a room by ID\n");
    printf("\t--path <game_path>: select the absolute path where the game is installed (mandatory)\n");
    printf("\t--randomize: if a room was selected, it randomizes it. Otherwise, it randomizes the whole game\n");
    printf("\t--restore: if a room was selected, it restores it to its original state. Otherwise, it\n");
    printf("\t           randomizes the whole game\n");
    printf("\tAnd more, read the README please.\n");
    printf("EXAMPLES:\n");
    printf("\t.\\ObsCureRandomizer.exe --path C:\\SteamLibrary\\steamapps\\common\\Obscure --room b008 --randomize\n");
    printf("\t.\\ObsCureRandomizer.exe --path C:\\SteamLibrary\\steamapps\\common\\Obscure --room b008 --restore\n");
    printf("\t.\\ObsCureRandomizer.exe --path C:\\SteamLibrary\\steamapps\\common\\Obscure --randomize\n");
    printf("\t.\\ObsCureRandomizer.exe --path C:\\SteamLibrary\\steamapps\\common\\Obscure --restore\n");
    printf("\t.\\ObsCureRandomizer.exe --help\n");
}

void test()
{
    //path("C:\\SteamLibrary\\steamapps\\common\\Obscure");
    //print_every_room();
    //print_struct_rooms_ids();
    //print_enum_rooms_ids();
    //print_enum_rooms_array();
}

int execute_commands(int argc, char **argv)
{
    // now we manage all other commands/actions
    for (int i = 0; i < argc; i++)
    {
        // 1: path
        if (strcmp(argv[i], "--it-parse") == 0 && argc > i + 1)
        {
            if (argc > i + 1)
            {
                if (parse_it_file_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't parse `.it` file b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }

        // 1: path
        // 2: item_id
        // 3: item_loc
        // 4: extra_info
        // 5: multiplier
        // 6: diff_mode
        // 7: path2
        if (strcmp(argv[i], "--it-add") == 0 && argc > i + 1)
        {
            if (argc > i + 7)
            {
                if (add_item_to_it_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't add item b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }
        // 1: path
        // 2: item_id
        // 3: item_loc
        // 4: extra_info
        // 5: multiplier
        // 6: diff_mode
        // 7: path2
        if (strcmp(argv[i], "--it-edit") == 0 && argc > i + 1)
        {
            if (argc > i + 7)
            {
                if (edit_item_in_it_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't edit item b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }

        // 1: path
        // 2: item_loc
        // 3: path2
        if (strcmp(argv[i], "--it-remove") == 0 && argc > i + 1)
        {
            if (argc > i + 3)
            {
                if (remove_item_from_it_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't remove item b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }
        // 1: path
        if (strcmp(argv[i], "--tm-parse") == 0 && argc > i + 1)
        {
            if (argc > i + 1)
            {
                if (parse_tm_file_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't parse `.tm` file b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }
        //  1: path
        //  2: item id
        //  3: item loc
        //  4: x pos
        //  5: y pos
        //  6: z pos
        //  7: rot_matrix[0]
        //  8: rot_matrix[1]
        //  9: rot_matrix[2]
        // 10: rot_matrix[3]
        // 11: rot_matrix[4]
        // 12: rot_matrix[5]
        // 13: rot_matrix[6]
        // 14: rot_matrix[7]
        // 15: rot_matrix[8]
        // 16: diff_mode string
        // 17: path2
        if (strcmp(argv[i], "--tm-add") == 0)
        {
            if (argc > i + 17)
            {
                if (add_item_to_tm_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't parse `.tm` file b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }

        //  1: path
        //  2: item id
        //  3: item loc
        //  4: x pos
        //  5: y pos
        //  6: z pos
        //  7: rot_matrix[0]
        //  8: rot_matrix[1]
        //  9: rot_matrix[2]
        // 10: rot_matrix[3]
        // 11: rot_matrix[4]
        // 12: rot_matrix[5]
        // 13: rot_matrix[6]
        // 14: rot_matrix[7]
        // 15: rot_matrix[8]
        // 16: diff_mode string
        // 17: path2
        if (strcmp(argv[i], "--tm-edit") == 0)
        {
            if (argc > i + 17)
            {
                if (edit_item_in_tm_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't edit `.tm` file b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }
        
        // 1: path
        // 2: item_loc
        // 3: path2
        if (strcmp(argv[i], "--tm-remove") == 0)
        {
            if (argc > i + 3)
            {
                if (remove_item_from_tm_b(argv, i))
                {
                    fprintf(stderr, "ERROR: Couldn't parse `.tm` file b\n");
                }

                i++;
            }
            else
            {
                fprintf(stderr, "ERROR: Provide everything\n");
            }
        }
    }
    return 0;
}