#include "document_id.h"

#include <stddef.h>
#include <stdio.h>

document_id document_ids[] =
{
    NO_DOCUMENT_ID,
    BUILDING_OF_SCHOOL,
    LEONARD_STATE,
    LETTER_FROM_WALT_KERRIDAN,
    THE_MORTIFILIA,
    SUBJECT_22,
    SHOM_TRANSFORMATION_PHOTOS,
    CENSORED_NEWSPAPER,
    BASEMENT_PHOTOS,
    PRESS_CLIPPINGS,
    FIND_THE_SAFE,
    TYPES_OF_MUTATION,
    LETTER_FROM_WICKSON,
    LETTER_TO_WICKSON,
    BEAUTY_CLUB_,
    BASKETBALL_PROGRAMMEE_,
    PHOTO_TWINS,
    EXPEDITION_DOCUMENTS,
    EXPEDITION_PLAQUE,
    NECROLOGY,
    EXPULSION_COMMUNITY,
};

char *document_names[] =
{
    "NO_DOCUMENT_ID",
    "BUILDING_OF_SCHOOL",
    "LEONARD_STATE",
    "LETTER_FROM_WALT_KERRIDAN",
    "THE_MORTIFILIA",
    "SUBJECT_22",
    "SHOM_TRANSFORMATION_PHOTOS",
    "CENSORED_NEWSPAPER",
    "BASEMENT_PHOTOS",
    "PRESS_CLIPPINGS",
    "FIND_THE_SAFE",
    "TYPES_OF_MUTATION",
    "LETTER_FROM_WICKSON",
    "LETTER_TO_WICKSON",
    "BEAUTY_CLUB_",
    "BASKETBALL_PROGRAMMEE_",
    "PHOTO_TWINS",
    "EXPEDITION_DOCUMENTS",
    "EXPEDITION_PLAQUE",
    "NECROLOGY",
    "EXPULSION_COMMUNITY",
};

char *document_name_from_id(document_id id)
{
    // First we look for the ID in the array of document_id
    // TODO: implement dichotomy search
    for (size_t i = 0; i < NB_DOCUMENT_IDS; i++)
    {
        //printf("Looking for %08X, iteration %zu\n", id, i);
        if (document_ids[i] == id)
        {
            return document_names[i];
        }
    }
    
    // If not found, return "NO_DOCUMENT_ID"
    return document_names[0];
}