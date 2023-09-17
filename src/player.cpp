

#include "player.hpp"

Player::Player( const int _m, const int _n, const Direction _direction )
                         : m(_m), n(_n), direction( _direction)  {
    
}

void Player::move( const Direction direction, const Maze &maze ) {
    const auto cube = maze[m][n] ;
    if( !maze.isWallInFront( direction, m, n ) ) {
        // above also checks for boundaries so we don't need to recheck
        switch( direction ) {
        case North :
            m-- ;
            break ;
        case South :
            m++ ;
            break ;
        case West :
            n-- ;
            break ;
        case East :
            n++ ;
            break ;
        }
    }
}


void Player::turn( const Turn turn ) {
    if( turn == Right ) {
        direction = rightDirection() ;
    } else {
        direction = leftDirection() ;
    }
}


Direction Player::rightDirection() const {
    Direction answer = direction ;
    switch (direction) {
        case North:
            answer = East;
            break;
        case East:
            answer = South;
            break;
        case South:
            answer = West;
            break;
        case West:
            answer = North;
            break;
        default:
            break;
    }
    return answer ;
}
    
Direction Player::leftDirection() const {
    Direction answer = direction ;
    switch (direction) {
        case North:
            answer = West;
            break;
        case East:
            answer = North;
            break;
        case South:
            answer = East;
            break;
        case West:
            answer = South;
            break;
        default:
            break;
    }
    return answer ;
}
 



bool Player::isWallInFront( const Maze &maze ) const {
    return maze.isWallInFront( getDirection(), getM(), getN() ) ;
}


std::ostream& operator<<(std::ostream& target, const Player& player) {
    target << "[" << player.getM() << "," << player.getN() << "] " ;
    switch( player.getDirection() ) {
        case North :
        target << "↑" ;
        break ;
        case South :
        target << "↓" ;
        break ;
        case West :
        target << "←" ;
        break ;
        case East :
        target << "→" ;
        break ;
        default:
        target << "?" ;
        break ;
    } 
    return target ;
}
