#include "item_id.h"

#include <stddef.h>
#include <stdio.h>

item_id item_ids[] =
{
    NO_ITEM_ID,
    WEAK_FLASHLIGHT,
    BAT,
    OLD_PISTOL,
    LASER,
    SHOTGUN,
    METAL_BAR,
    LIGHT_GRENADE,
    GUN_WITH_FLASHLIGHT,
    EMPTY_SLOT_,
    YELLOW_FLASHLIGHT,
    POWERFUL_FLASHLIGHT,
    AUTOMATIC_PISTOL,
    REVOLVER,
    DOUBLE_BARREL_SHOTGUN,
    ALUMINIUM_BAT,
    LASER_GUN,
    SHOTGUN_AMMO,
    HANDGUN_AMMO,
    STATUETTE,
    SAFE,
    SCREWDRIVER,
    LEVER,
    PHOTO,
    DOCUMENT,
    MAP,
    KEY_,
    DISC,
    TAPE,
    PLIERS,
    WHEEL,
    MEDKIT,
    FILM_24,
    SUBJECT_37_,
    REEL_OF_WIRE,
    ENERGY_DRINK
};

char *item_names[] =
{
    "NO_ITEM_ID",
    "WEAK_FLASHLIGHT",
    "BAT",
    "OLD_PISTOL",
    "LASER",
    "SHOTGUN",
    "METAL_BAR",
    "LIGHT_GRENADE",
    "GUN_WITH_FLASHLIGHT",
    "EMPTY_SLOT_",
    "YELLOW_FLASHLIGHT",
    "POWERFUL_FLASHLIGHT",
    "AUTOMATIC_PISTOL",
    "REVOLVER",
    "DOUBLE_BARREL_SHOTGUN",
    "ALUMINIUM_BAT",
    "LASER_GUN",
    "SHOTGUN_AMMO",
    "HANDGUN_AMMO",
    "STATUETTE",
    "SAFE",
    "SCREWDRIVER",
    "LEVER",
    "PHOTO",
    "DOCUMENT",
    "MAP",
    "KEY?",
    "DISC",
    "TAPE",
    "PLIERS",
    "WHEEL",
    "MEDKIT",
    "FILM_24",
    "SUBJECT_37?",
    "REEL_OF_WIRE",
    "ENERGY_DRINK"
};

char *item_name_from_id(item_id id)
{
    // First we look for the ID in the array of item_id
    // TODO: implement dichotomy search
    for (size_t i = 0; i < NB_ITEM_IDS; i++)
    {
        //printf("Looking for %08X, iteration %zu\n", id, i);
        if (item_ids[i] == id)
        {
            return item_names[i];
        }
    }
    
    // If not found, return "NO_ITEM_ID"
    return item_names[0];
}