#ifndef PACMAN_FORM_H
#define PACMAN_FORM_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <iostream>
#include <vector>
#include "Wall.h"
#include "Pellet.h"
#include "Values.h"
#include "File.h"

using namespace std;

class Application;

//each application state
class Form {
public:
    Form(string, Application &, float offset = 0);

    virtual ~Form();

    //to poll events from Application window to forms
    virtual void pollEvents(sf::Event &, sf::RenderWindow *) = 0;

    virtual void update(sf::RenderWindow *, const sf::Time &) = 0;

    virtual void render(sf::RenderWindow *) = 0;

    //draw background walls based on map
    virtual void clear(sf::RenderWindow *window);

    const array<array<GameObject::ObjectType, 19>, 26> &getBoard() const;

    Application &getApplicationContext();

private:
    Application &context;

    //walls grid is same for all forms so we handle it here
    void initGrid();

    vector<Wall *> walls;

    //to set offset to map
    float offset = 0;

    //initizalized by class constructor
    string mapFileName;

protected:
    //we need this in GameForm for drawing other entities
    array<array<GameObject::ObjectType, 19>, 26> board;
};


#endif //PACMAN_FORM_H
