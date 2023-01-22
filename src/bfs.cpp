
#include <map>
#include <string.h>
#include <iostream>

constexpr int NumPlaces = 8 ;

using namespace std ;


class State {
    private:
        char state[NumPlaces] ;
        char left ;
        char right ;

    public:
        bool operator == ( const State &o ) const {
            return
                left == o.left &&
                right == o.right && 
                0 == memcmp( state, o.state, sizeof(State) ) ;
        }

        bool move( uint16_t click ) {
            if( click<=0 || click>=NumPlaces-1 ) {
                return false ;
            }

            int l = state[click-1] ;
            int r = state[click+1] ;

            if( l == 0 && left != 0) return false ;
            if( r == 0 && right != 0) return false ;

            return true ;
        }

} ;


map<State,int> Visited ;

int main( int argc, char **argv, char **envp ) {
    return 0 ;
}


