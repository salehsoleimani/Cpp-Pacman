#ifndef PACMAN_CLYDE_H
#define PACMAN_CLYDE_H

#include "Ghost.h"

//customizable based on clyde's special behaviors
class Clyde : public Ghost {
public:
    Clyde(sf::Vector2f, GameForm *);

    ~Clyde();

};


#endif //PACMAN_CLYDE_H
