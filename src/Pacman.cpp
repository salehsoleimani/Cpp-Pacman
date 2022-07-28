#include "Pacman.h"
#include "Ghost.h"

Pacman::Pacman(sf::Vector2f position, GameForm *context) : GameObject(position), context(context) {
    pacman.setPosition(position);

    animator = new Animator(pacman);

    animator->add("right", sf::milliseconds(300), "../res/sprites/pacman.png", sf::Vector2i(30, 0), 5);
    animator->add("left", sf::milliseconds(300), "../res/sprites/pacman.png", sf::Vector2i(30, 60), 5);
    animator->add("up", sf::milliseconds(300), "../res/sprites/pacman.png", sf::Vector2i(30, 30), 5);
    animator->add("down", sf::milliseconds(300), "../res/sprites/pacman.png", sf::Vector2i(30, 90), 5);

    animator->add("die", sf::milliseconds(300), "../res/sprites/pacman_die.png", sf::Vector2i(24, 0), 12, false);

    updateRelativePosition();
}

Pacman::~Pacman() {

}

void Pacman::pollEvents(sf::Event &event) {
    if (event.type == sf::Event::KeyPressed)
        switch (event.key.code) {
            case sf::Keyboard::Down:
                direction = Directions::DOWN;
                animator->setAnimation("down");
                break;
            case sf::Keyboard::Up:
                direction = Directions::UP;
                animator->setAnimation("up");
                break;
            case sf::Keyboard::Left:
                direction = Directions::LEFT;
                animator->setAnimation("left");
                break;
            case sf::Keyboard::Right:
                direction = Directions::RIGHT;
                animator->setAnimation("right");
                break;
            default:
                direction = Directions::INIT;
                animator->setAnimation("die");
                break;
        }
}

void Pacman::update(sf::Time dt) {

    animator->update(dt);

    sf::Vector2f currentPosition = pacman.getPosition();

    //a vector to hold next move coordinates
    nextMove = {0, 0};

    switch (direction) {
        case Directions::DOWN:
            nextMove.y += speed;
            break;
        case Directions::UP:
            nextMove.y -= speed;
            break;
        case Directions::LEFT:
            nextMove.x -= speed;
            break;
        case Directions::RIGHT:
            nextMove.x += speed;
            break;
        case Directions::INIT:
            nextMove = {0, 0};
            break;
    }

    pacman.move(nextMove);

    //eating fruits
    for (auto food: context->getFoods()) {
        if (food->getRelativePosition().x == relativePosition.x &&
            food->getRelativePosition().y == relativePosition.y && !food->isEaten()) {
            food->ate();
            context->raiseScore(food->getPpt());
        }
    }

    //check collision with ghosts
    for (auto ghost: context->getGhosts()) {
        if (ghost->getRelativePosition().x == relativePosition.x &&
            ghost->getRelativePosition().y == relativePosition.y) {
            animator->setAnimation("die");
            context->lose();
        }
    }


    updateRelativePosition();

    //walking through gates
    if (pacman.getPosition().x >= Dimensions::wallSize.x * Dimensions::WALL_COL)
        pacman.setPosition(-Dimensions::wallSize.x, pacman.getPosition().y);

    else if (pacman.getPosition().x <= -Dimensions::wallSize.x)
        pacman.setPosition(Config::videoMode.width, pacman.getPosition().y);

    else if ((pacman.getPosition().x < 1 / 2 * Dimensions::wallSize.x ||
              (pacman.getPosition().x >
               Dimensions::wallSize.x * (Dimensions::WALL_COL - 1) - 1 / 2 * Dimensions::wallSize.x)) &&
             (direction == Directions::RIGHT || direction == Directions::LEFT))
        return;

    else {
        if (checkCollision(relativePosition.x, relativePosition.y)) {
            pacman.setPosition(currentPosition);

            currentPosition = pacman.getPosition();

            updateRelativePosition();

            if (lastMove != nextMove && !checkCollision(relativePosition.x, relativePosition.y))
                pacman.move(lastMove);

            updateRelativePosition();

            if (checkCollision(relativePosition.x, relativePosition.y))
                pacman.setPosition(currentPosition);

        } else
            lastMove = nextMove;
    }

}

bool Pacman::checkCollision(float x, float y) {
    if (context->getBoard()[ceil(y)][ceil(x)] == ObjectType::WALL ||
        context->getBoard()[ceil(y)][floor(x)] == ObjectType::WALL ||
        context->getBoard()[floor(y)][ceil(x)] == ObjectType::WALL ||
        context->getBoard()[floor(y)][floor(x)] == ObjectType::WALL)
        return true;
    return false;
}

void Pacman::eat(Food *food) {
}

void Pacman::updateRelativePosition() {
    relativePosition = {pacman.getPosition().x / Dimensions::wallSize.x,
                        pacman.getPosition().y / Dimensions::wallSize.x};
}

void Pacman::render(sf::RenderTarget *target) {
    target->draw(pacman);
}