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
    HOOK,
    FUSES,
    WOOD_PLANK,
    NEEDLE,
    SCREWDRIVER,
    PIECE_OF_PAPER,
    LEVER,
    FILM,
    PHOTO,
    DOCUMENT,
    MAP,
    BASEMENT_PLAN,
    KEY,
    DISC,
    TAPE,
    PLIERS,
    WHEEL,
    MEDKIT,
    CELLPHONE,
    VIDEO_TAPE,
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
    "HOOK",
    "FUSES",
    "WOOD_PLANK",
    "NEEDLE",
    "SCREWDRIVER",
    "PIECE_OF_PAPER",
    "LEVER",
    "FILM",
    "PHOTO",
    "DOCUMENT",
    "MAP",
    "BASEMENT_PLAN",
    "KEY",
    "DISC",
    "TAPE",
    "PLIERS",
    "WHEEL",
    "MEDKIT",
    "CELLPHONE",
    "VIDEO_TAPE",
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