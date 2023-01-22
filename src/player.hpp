
#pragma once

#include <iostream>
#include "maze.hpp"
#include "enums.hpp"


class Player {
    friend std::ostream& operator<<(std::ostream& target, const Player& player);
    private:
        int m ;
        int n ;
        Direction direction ;
    protected:

    public:
        Player( const int _M=0, const int _N=0, const Direction _direction=East );
        void move( const Direction direction, const Maze &maze );
        void step( const Maze &maze ) { move( direction, maze); }
        void reset() { m=0; n=0; }
        void turn( const Turn turn );
        int getM() const { return m; }
        int getN() const { return n; }        
        Direction getDirection() const { return direction; }
        bool isWallInFront( const Maze &maze ) const ;
        Direction leftDirection() const ;
        Direction rightDirection() const ;
} ;

