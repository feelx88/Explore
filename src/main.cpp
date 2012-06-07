#include "game/Explore.h"

int main( int, char** )
{
    Explore game;
    game.setGameState( EGS_GAME );
    return game.run();
}

