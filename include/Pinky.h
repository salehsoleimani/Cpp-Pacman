#ifndef PACMAN_PINKY_H
#define PACMAN_PINKY_H

#include "Ghost.h"

//customizable based on Pinky's special behaviors
class Pinky : public Ghost {
public:
    Pinky(sf::Vector2f, GameForm *,bool useTheDoor);

    ~Pinky() override;

    void update(sf::Time) override;
};


#endif //PACMAN_PINKY_H
