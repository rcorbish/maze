
#include <iostream>

#include "maze.hpp"
#include "player.hpp"
#include "opengldisplay.hpp"

Maze *maze ;
Player *player ;

using namespace std ;

int main( int argc, char **argv, char **envp ) {
    const auto m = ( argc>1 ) ? atoi( argv[1] ) : 25 ;
    const auto n = ( argc>2 ) ? atoi( argv[2] ) : m ;
    maze  = new Maze( m, n ) ;

    cout << *maze << endl ;

    player = new Player() ;

    displayinit( argc, argv ) ;

    return 0 ;
}