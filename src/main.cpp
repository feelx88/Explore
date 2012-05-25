#include "game/Explore.h"

int main( int, char** )
{
    ExplorePtr game = Explore::create();
    game->setGameState( EGS_GAME );
    return game->run();
}

