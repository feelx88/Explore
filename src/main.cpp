#include "game/Explore.h"

int main( int, char** )
{
    ExplorePtr game = Explore::create();
    return game->run();
}

