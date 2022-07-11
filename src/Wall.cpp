#include "Wall.h"

Wall::Wall(sf::Vector2f position){
    this->position = position;
    wall = new sf::RectangleShape;
    wall->setPosition(position);
    wall->setOrigin(wall->getSize().x / 2, wall->getSize().y / 2);
    wall->setFillColor(Application::colorWall);
    wall->setOutlineColor(Application::colorBlue);
    wall->setOutlineThickness(-1.f);
    wall->setSize(Application::wallSize);
}

void Wall::update(sf::RenderTarget *target) {

}

Wall::~Wall() {
    delete wall;
}

void Wall::render(sf::RenderTarget *target) {
    target->draw(*wall);
}
