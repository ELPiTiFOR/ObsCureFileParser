#ifndef ITEM_ID_H
#define ITEM_ID_H

#define NB_ITEM_IDS 36

typedef enum
{
    NO_ITEM_ID = 0x0000,
    WEAK_FLASHLIGHT = 0X00C9,
    BAT = 0x00CA,
    OLD_PISTOL = 0x00CB,
    LASER = 0x00CC,
    SHOTGUN = 0x00CE,
    METAL_BAR = 0x00D1,
    LIGHT_GRENADE = 0x00D2,
    GUN_WITH_FLASHLIGHT = 0x00D3,
    EMPTY_SLOT_ = 0x00D4,
    YELLOW_FLASHLIGHT = 0x00D5,
    POWERFUL_FLASHLIGHT = 0x00D6,
    AUTOMATIC_PISTOL = 0x00D7,
    REVOLVER = 0x00D8,
    DOUBLE_BARREL_SHOTGUN = 0x00D9,
    ALUMINIUM_BAT = 0x00DA,
    LASER_GUN = 0x00DC,
    SHOTGUN_AMMO = 0x0130,
    HANDGUN_AMMO = 0x0132,
    STATUETTE = 0x0191,
    SAFE = 0x0192,
    SCREWDRIVER = 0x019A,
    LEVER = 0x019C,
    PHOTO = 0x01F6,
    DOCUMENT = 0x01F7,
    MAP = 0x01F8,
    KEY_ = 0x0259,
    DISC = 0x025A,
    TAPE = 0x025B,
    PLIERS = 0x025C,
    WHEEL = 0x025E,
    MEDKIT = 0x025F,
    FILM_24 = 0x0260,
    VIDEO_TAPE = 0x0261,
    REEL_OF_WIRE = 0x0263,
    ENERGY_DRINK = 0x0264,
} item_id;

extern item_id item_ids[NB_ITEM_IDS];
extern char *item_names[NB_ITEM_IDS];

char *item_name_from_id(item_id id);

#endif /* !ITEM_ID_H */