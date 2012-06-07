#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <boost/shared_ptr.hpp>

enum E_GAME_STATE
{
    EGS_QUIT = 0,
    EGS_MAIN_MENU,
    EGS_GAME
};

class Explore;
typedef Explore* ExplorePtr;

#endif // GAMESTATE_H
