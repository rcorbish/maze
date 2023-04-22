
#pragma once

#include <iostream>
#include <vector>
#include "enums.hpp"

typedef struct Weight {
    int from ;
    int to ;
    float weight ;
    Weight( int f, int t, float w ) : from(f), to(t), weight(w) {}
} Weight ;


class Maze {
    friend std::ostream& operator<<(std::ostream& target, const Maze& maze);
    private:
        std::vector<Weight> minimumSpanningTree ;
        std::vector<std::vector<int>> cubes ;

    protected:
        void setRandomWeight( std::vector<Weight> &weights, int m, int n ) ;
        std::vector<std::vector<int>> buildCubesFromGraph() const ;

    public:
        // size of lattice on which maze is built
        const int M ;    
        const int N ;
        
        Maze( int _M, int _N ) ;
        bool isWallInFront( const Direction facing, const int m, const int n ) const ;
        inline std::vector<int> operator[]( int ix ) const { return cubes[ix] ; }
        std::vector<std::pair<int,int>> Maze::verticalSegments( int columnIndex ) ;

} ;


