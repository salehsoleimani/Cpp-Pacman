#include "Pellet.h"

Pellet::Pellet(sf::Vector2f position, PelletType foodType) : Snack(position), foodType(foodType) {
    if (foodType == PelletType::NORMAL) {
        snackType = Snack::SnackType::PELLET;
        food.setRadius(3.0f);
    }
    else {
        snackType = Snack::SnackType::POWER;
        food.setRadius(7.0f);
    }
    food.setOrigin(food.getGlobalBounds().width / 2, food.getGlobalBounds().height / 2);
    food.setPosition({position.x + Dimensions::wallSize.x / 2, position.y + Dimensions::wallSize.y / 2});
    food.setFillColor(Colors::colorDot);

    eaten = false;
}

Pellet::~Pellet() {

}

void Pellet::render(sf::RenderTarget *target) {
    target->draw(food);
}

void Pellet::update(sf::Time dt) {
    //blinking power state
    if (foodType == PelletType::POWER && !isEaten()) {
        progress += dt;
        if (progress >= blinkAnimation) {
            progress = sf::Time::Zero;
            if (foodState == PelletState::BLINK) {
                food.setFillColor(Colors::colorDot);
                foodState = PelletState::NORMAL;
            } else {
                food.setFillColor(sf::Color::Transparent);
                foodState = PelletState::BLINK;
            }
            progress = sf::Time::Zero;
        }
    }
}

void Pellet::eat() {
    //amount of pause before eating pellet
    if (foodType == PelletType::NORMAL)
        sf::sleep(sf::seconds(1 / Config::windowFrameRate));
    else
        sf::sleep(sf::seconds(3 / Config::windowFrameRate));

    eaten = true;
    food.setFillColor(sf::Color::Transparent);
}

//enhance this later
unsigned Pellet::getPpt() const {
    switch (foodType) {
        case PelletType::NORMAL:
            return 20;
        case PelletType::POWER:
            return 50;
        default:
            return 0;
    }
}