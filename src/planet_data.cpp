#include "planet_data.h"

/*
 * planet information
 * see more at: https://science.nasa.gov/solar-system/planets/
 * and at: https://nssdc.gsfc.nasa.gov/planetary/factsheet/
 */
planetInfo planetsData[] = {
    {"Mercury", "0.4 astronomical units" , "2,440 km" , "0 moons"  , "59 Earth days"   , "88 Earth days"    },
    {"Venus"  , "0.72 astronomical units", "6,051 km" , "0 moons"  , "243 Earth days"  , "225 Earth days"   },
    {"Earth"  , "1.0 astronomical unit"  , "6,378 km" , "1 moon"   , "1 Earth day"     , "365 Earth days"   },
    {"Mars"   , "1.5 astronomical units" , "3,390 km" , "2 moons"  , "23.9 Earth hours", "687 Earth days"   },
    {"Jupiter", "5.2 astronomical units" , "69,911 km", "95 moons" , "10 Earth hours"  , "4,333 Earth days" },
    {"Saturn" , "9.5 astronomical units" , "58,232 km", "146 moons", "10.7 Earth hours", "10,756 Earth days"},
    {"Uranus" , "19.8 astronomical units", "25,362 km", "27 moons" , "17 Earth hours"  , "30,687 Earth days"},
    {"Neptune", "30 astronomical units"  , "24,622 km", "14 moons" , "16 Earth hours"  , "60,190 Earth days"}
};

/* planet properties */
planetProperties planetProp[] = {
    {2.0f, 2.0f, 0.3f, 0.04f},  // mercury
    {1.5f, 3.0f, 0.4f, 0.1f },  // venus
    {1.0f, 4.0f, 0.5f, 0.1f },  // earth
    {0.8f, 5.0f, 0.6f, 0.09f},  // mars
    {0.6f, 6.0f, 0.7f, 0.3f },  // jupiter
    {0.3f, 7.0f, 0.8f, 0.4f },  // saturn
    {0.2f, 8.0f, 1.0f, 0.35f},  // uranus
    {0.1f, 9.0f, 0.9f, 0.35f}   // neptune
};

/* moon properties */
planetProperties moonProp = {6.0f, 0.3f, 3.0f, 0.03f};
