#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <boost/shared_ptr.hpp>

enum E_GAME_STATE
{
    EGS_QUIT = 0,
    EGS_MAIN_MENU,
    EGS_GAME
};

enum E_ITEM_ACTION
{
    EIA_FIRST_ACTION = 0,
    EIA_SECOND_ACTION,
    EIA_USE_ACTION
};

class Explore;
typedef Explore* ExplorePtr;

#endif // GAMESTATE_H
