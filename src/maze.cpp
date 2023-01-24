
#include <cstdlib>
#include <map>


#include "maze.hpp"

using namespace std ;



Maze::Maze( int _M, int _N ) : M(_M), N(_N) {
    srand ( 100 ) ;

    vector<Weight> weights ;
    weights.reserve( M * N * 4 ) ;
    
    for( int n=0 ; n<N ; n++ ) {
        for( int m=0 ; m<M ; m++ ) {
            setRandomWeight( weights, m, n ) ;
        }
    }

    sort( weights.begin(), weights.end(), [](const Weight &a,const Weight &b) -> bool { return a.weight < b.weight ; }  ) ;

    minimumSpanningTree.reserve( M*N-1 ) ;
    map<int,int> nodesInTree ;
    
    for( const Weight &w : weights ) {
        auto groupFrom = nodesInTree.find(w.from) ;
        auto groupTo = nodesInTree.find(w.to) ;
        int from = ( groupFrom == nodesInTree.end() ) ? -1 : (groupFrom->second) ;
        int to = ( groupTo == nodesInTree.end() ) ? -1 : (groupTo->second) ;

        if( from == to && from>=0 ) continue ;   // cycle detected
        
        if( from<0 && to<0 ) { // unconnected nodes - just add a new group
            nodesInTree[w.from] = w.from ; 
            nodesInTree[w.to] = w.from ; 
        } else if( from>=0 && to>=0 ) {  // merge 2 groups
            for( const auto &item : nodesInTree ) {
                if( item.second == to ) {
                    nodesInTree[item.first] = from ;
                }
            }
        } else if( from<0 ) {  // add into an existing group
            nodesInTree[w.from] = to ; 
        } else if( to<0 ) { // also add into an existing group
            nodesInTree[w.to] = from ; 
        }

        minimumSpanningTree.emplace_back( w.from, w.to, w.weight ) ;
        if( minimumSpanningTree.size() == M*N-1 ) break ;
    }
    sort( minimumSpanningTree.begin(), minimumSpanningTree.end(), 
        [](const Weight &a,const Weight &b) -> bool { return a.from < b.from ; }  ) ;

    cubes = buildCubesFromGraph() ;
}


void Maze::setRandomWeight( vector<Weight> &weights, int m, int n ) {
    int ixFrom = m + ( n * M ) ;

    int mm = m-1 ;
    if( mm>=0 ) {
        int ixTo = ixFrom-1 ;
        float wt = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ;
        weights.emplace_back( ixFrom, ixTo, wt ) ;
    }

    mm = m+1 ;
    if( mm<M ) {
        int ixTo = ixFrom+1 ;
        float wt = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ;
        weights.emplace_back( ixFrom, ixTo, wt ) ;
    }

    int nn = n-1 ;
    if( nn>=0 ) {
        int ixTo = ixFrom - M ;
        float wt = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ;
        weights.emplace_back( ixFrom, ixTo, wt ) ;
    }

    nn = n+1 ;
    if( nn<N ) {
        int ixTo = ixFrom + M ;
        float wt = static_cast <float>( rand() ) / static_cast <float>( RAND_MAX ) ;
        weights.emplace_back( ixFrom, ixTo, wt ) ;
    }
}

vector<vector<int>> Maze::buildCubesFromGraph() const {
    vector<vector<int>> cubes ;
    cubes.reserve( M ) ;
    // fill M x N array with 0 values
    for( int i=0 ; i<M ; i++ ) {
        cubes.emplace_back( N, 0 ) ;
    }

    for( const auto &w : minimumSpanningTree ) {
        int x0 = w.from / M ;
        int y0 = w.from % M ;
        int x1 = w.to / M ;
        int y1 = w.to % M ;


        if( x0==x1 && y1 > y0 ) {   // Down 
            cubes[y0][x0] |= South ;
            cubes[y1][x1] |= North ;
        }
        if( x0==x1 && y0 > y1 ) {   // Up 
            cubes[y0][x0] |= North ;
            cubes[y1][x1] |= South ;
        }
        if( y0==y1 && x1 > x0 ) {   // Left
            cubes[y0][x0] |= East ;
            cubes[y1][x1] |= West ;
        }
        if( y0==y1 && x0 > x1 ) {   // Right
            cubes[y0][x0] |= West ;
            cubes[y1][x1] |= East ;
        }
    }
    return cubes ;
}

bool Maze::isWallInFront( const Direction facing, const int m, const int n ) const {

    int cube = (m<M && n<N) ? cubes[m][n] : 0 ;  // where are the paths in the maze ?

    bool rc = false ;
    switch (facing)
    {
    case North:
        return m==0 || !(cube & North) ;
        break;
    case East:
        return n==(N-1) || !(cube & East) ;
        break;
    case South:
        return m==(M-1) || !(cube & South) ;
        break;
    case West:
        return n==0 || !(cube & West) ;
        break;
    default:
        break;
    }
    return rc ;
}

static string images[] = { "?", "╷", "╵", "│", "╶", "┌" , "└", "├", "╴", "┐", "┘", "┤", "─", "┬", "┴", "┼"   } ;

ostream & operator << ( ostream &s, const Maze &maze ) {
    vector<vector<int>> cubes = maze.buildCubesFromGraph() ;
    bool firstRow = true ;
    for( vector<int> row : cubes ) {
        if( !firstRow ) s << '\n' ;
        for( int col : row ) {
            s << images[col] ;
        }
        firstRow = false ;
    }
    
    
    // for( const auto &w : maze.minimumSpanningTree ) {
    //     int x0 = w.from / maze.M ;
    //     int y0 = w.from % maze.M ;
    //     int x1 = w.to / maze.M ;
    //     int y1 = w.to % maze.M ;
    //     s << x0 << "," << y0 << " " << x1 << "," << y1 << " (" << w.weight << ")\n" ;
    // }
    
    return s ;
}
