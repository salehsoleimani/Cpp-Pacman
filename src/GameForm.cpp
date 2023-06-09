#include "Pacman.h"
#include "GameForm.h"
#include <random>
#include "Inky.h"
#include "Blinky.h"
#include "Clyde.h"
#include "Pinky.h"

GameForm::GameForm(Application &context, string mapFile, bool useTheDoor) : Form(mapFile, context),
                                                                            useTheDoor(useTheDoor) {
    initGame();
    initSprites();
    initSounds();
    getApplicationContext().getDialog()
            .create("Welcome!",
                    "use keys to move",
                    "continue",
                    [&]() -> void {
                        //play pacman beginning welcome sound
                        introMusic.setBuffer(introBuffer);
                        introMusic.play();
                        getApplicationContext().getDialog().hide();
                    }).show();
}

void GameForm::initSounds() {
    string path = "../res/sounds/";
    introBuffer.loadFromFile(path + "pacman_beginning.wav");
    introMusic.setBuffer(introBuffer);
    chompBuffer.loadFromFile(path + "pacman_chomp.wav");
    chompSound.setBuffer(chompBuffer);
    deathBuffer.loadFromFile(path + "pacman_death.wav");
    deathSound.setBuffer(deathBuffer);
    eatFruitBuffer.loadFromFile(path + "pacman_eatfruit.wav");
    eatFruitSound.setBuffer(eatFruitBuffer);
    eatGhostBuffer.loadFromFile(path + "pacman_eatghost.wav");
    eatGhostSound.setBuffer(eatGhostBuffer);
    intermissionBuffer.loadFromFile(path + "pacman_intermission.wav");
    intermissionSound.setBuffer(intermissionBuffer);
}

void GameForm::initGame() {
    score = 0;
    readRecord();

    txtScore = new TextView("score\n0", {219, 22});
    txtRecord = new TextView("high score\n" + to_string(highScore), {329, 22});
    txtLevel = new TextView("level\n1", {109, 22});
    txtLevel->setCharacterSize(Font::smallFontSize);
    txtScore->setCharacterSize(Font::smallFontSize);
    txtRecord->setCharacterSize(Font::smallFontSize);

    auto *icBackTexture = new sf::Texture;
    icBackTexture->loadFromFile("../res/icons/ic_back.png");
    btnBackIc.setPosition({32, (txtLevel->getGlobalBounds().top + txtLevel->getGlobalBounds().height) / 2});
    btnBackIc.setTexture(*icBackTexture);

    heartTexture = new sf::Texture;
    heartTexture->loadFromFile("../res/sprites/heart.png");
    heartTexture->setSmooth(true);

    //sound icon
    soundIcon = new sf::Texture;
    soundIcon->loadFromFile("../res/icons/ic_sound_on.png");
    soundIcon->setSmooth(true);

    soundOffIcon = new sf::Texture;
    soundOffIcon->loadFromFile("../res/icons/ic_sound_off.png");
    soundOffIcon->setSmooth(true);

    btnSound.setPosition({500, 27});
    if (soundOn)
        btnSound.setTexture(*soundIcon);
    else
        btnSound.setTexture(*soundOffIcon);

    sf::Sprite heartSprite(*heartTexture);

    for (int j = 2; j < 5; ++j) {
        sf::Vector2f position = sf::Vector2f{j * Dimensions::wallSize.x, 25 * Dimensions::wallSize.x};
        if (board[25][j] == GameObject::ObjectType::HEART) {
            heartSprite.setPosition(position);
            hearts.push_back(heartSprite);
        }
    }
}

void GameForm::initSprites() {
    snacksCount = 0;

    for (int i = 0; i < 26; ++i)
        for (int j = 0; j < Dimensions::WALL_COL; ++j) {
            Pellet *snack = nullptr;
            sf::Vector2f position = sf::Vector2f{j * Dimensions::wallSize.x, i * Dimensions::wallSize.x};
            switch (board[i][j]) {
                case GameObject::ObjectType::PACMAN:
                    pacman = new Pacman(position, this);
                    pacmanPosition = position;
                    break;
                case GameObject::ObjectType::PELLET: //normal foods
                    snacksCount++;
                    snack = new Pellet(position, Snack::SnackType::PELLET);
                    snack->setRelativePosition(sf::Vector2f(sf::Vector2i{j, i}));
                    snacks.push_back(snack);
                    break;
                case GameObject::ObjectType::PELLET_POWER: //power foods
                    snacksCount++;
                    snack = new Pellet(position, Snack::SnackType::POWER);
                    snack->setRelativePosition(sf::Vector2f(sf::Vector2i{j, i}));
                    snacks.push_back(snack);
                    break;
                case GameObject::ObjectType::INKY:
                    ghosts.push_back(new Inky(position, this, useTheDoor));
                    break;
                case GameObject::ObjectType::BLINKY:
                    ghosts.push_back(new Blinky(position, this,useTheDoor));
                    break;
                case GameObject::ObjectType::CLYDE:
                    ghosts.push_back(new Clyde(position, this,useTheDoor));
                    break;
                case GameObject::ObjectType::PINKY:
                    ghosts.push_back(new Pinky(position, this,useTheDoor));
                    break;
                default:
                    break;
            }
        }
}

GameForm::~GameForm() {
    delete pacman;
    for (auto snack: snacks) delete snack;
    for (auto ghost: ghosts) delete ghost;
    delete heartTexture;
    delete soundOffIcon;
    delete soundIcon;
    delete txtRecord;
    delete txtLevel;
    delete txtScore;
    //saving high score
    if (score > highScore)
        storeRecord();
}

void GameForm::pollEvents(sf::Event &event, sf::RenderWindow *window) {
    sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(*window));
    switch (event.type) {
        case sf::Event::MouseButtonReleased:
            if (btnSound.getGlobalBounds().contains(mousePosition)) {
                soundOn = !soundOn;
                if (soundOn)
                    btnSound.setTexture(*soundIcon);
                else {
                    btnSound.setTexture(*soundOffIcon);
                    introMusic.stop();
                    chompSound.stop();
                    deathSound.stop();
                    eatFruitSound.stop();
                    eatGhostSound.stop();
                    intermissionSound.stop();
                }
            }
        case sf::Event::MouseButtonPressed:
            // user pauses the game
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (btnBackIc.getGlobalBounds().contains(mousePosition)) {
                    getApplicationContext().getDialog()
                            .create("Pause",
                                    "pause the game",
                                    "okay",
                                    "cancel",
                                    [&]() -> void {
                                        getApplicationContext().pushForm(
                                                new MainForm(getApplicationContext()));
                                        getApplicationContext().getDialog().hide();
                                    },
                                    [&]() -> void {
                                        getApplicationContext().getDialog().hide();
                                    }).show();
                }
            }
            break;
            //handling pacman moves
        case sf::Event::KeyPressed:
            pacman->pollEvents(event);
            break;
        default:
            break;
    }
}

void GameForm::update(sf::RenderWindow *window, const sf::Time &dt) {

    //if pacman ate all snacks rearrange board - reach to next level
    if (eatenSnacks == snacksCount) {
        getApplicationContext().getDialog()
                .create("Victory",
                        "reached level " + to_string(level + 1),
                        "Hooray!",
                        [&]() -> void {
                            getApplicationContext().getDialog().hide();
                            level++;
                            txtLevel->setString("level\n" + to_string(this->level));
                            resetBoard();
                        }).show();
    }

    //updating other views
    if (pacman)
        pacman->update(dt);

    updateFruits(dt);

    for (auto snack: snacks)
        if (snack) snack->update(dt);

    for (auto ghost: ghosts)
        if (ghost) ghost->update(dt);

    readRecord();
    txtRecord->setString("high score\n" + to_string(highScore));

    txtScore->setString("score\n" + to_string(this->score));
}

void GameForm::updateFruits(const sf::Time &dt) {
//counting timer when fruit is shown
    if (isFruitVisible) fruitTimer += dt.asSeconds();

    //when fruit hides
    if (fruitTimer >= 10 && isFruitVisible) {
        fruitTimer = 0;
        delete snacks.back();
        snacks.pop_back();
        isFruitVisible = false;
    }

    //showing fruit after a range of snacks
    if ((eatenSnacks >= 70 && fruitsCount == 0) || (eatenSnacks >= 170 && fruitsCount == 1)) {
        fruitsCount++;
        isFruitVisible = true;
        vector<Snack *> randomSnacks;
        for (auto snack: snacks) {
            if (snack->isEaten())
                randomSnacks.push_back(snack);
        }
        std::shuffle(randomSnacks.begin(), randomSnacks.end(), std::mt19937(std::random_device()()));

        Fruit::FruitType fruitType = Fruit::FruitType::APPLE;

        if (level >= 241) fruitType = Fruit::FruitType::BANANA;
        else if (level >= 225) fruitType = Fruit::FruitType::GRAPES;
        else if (level >= 192) fruitType = Fruit::FruitType::PEACH;
        else if (level >= 129) fruitType = Fruit::FruitType::CHERRY;
        else if (level >= 65) fruitType = Fruit::FruitType::STRAWBERRY;

        Fruit *newFruit = new Fruit(randomSnacks[0]->getPosition(), fruitType);
        newFruit->setRelativePosition(randomSnacks[0]->getRelativePosition());
        snacks.push_back(newFruit);
    }
}

void GameForm::render(sf::RenderWindow *window) {
    txtRecord->render(window);
    txtScore->render(window);
    txtLevel->render(window);
    for (auto snack: snacks) snack->render(window);
    for (auto ghost: ghosts) ghost->render(window);
    for (auto heart: hearts) window->draw(heart);
    if (pacman) pacman->render(window);
    window->draw(btnBackIc);
    window->draw(btnSound);
}

void GameForm::lose() {
    sf::sleep(sf::milliseconds(300));
    int lives = hearts.size();
    //if hearts remaining
    if (lives > 1) {
        hearts.pop_back();
        delete pacman;
        pacman = new Pacman(pacmanPosition, this);

        for (auto ghost: ghosts) ghost->setPosition(ghost->getInitialPosition(), true);

        score -= 20;
    } else {
        getApplicationContext().getDialog()
                .create("Game Over!",
                        "you lose",
                        "continue",
                        [&]() -> void {
                            level = 1;
                            hearts.clear();
                            resetBoard();
                            initGame();
                            score = 0;
                            getApplicationContext().getDialog().hide();
                        }).show();
    }
}

void GameForm::resetBoard() {
    isFruitVisible = false;
    fruitTimer = 0;
    if (score > highScore)
        storeRecord();
    readRecord();
    for (auto snack: snacks) delete snack;
    for (auto ghost: ghosts) delete ghost;
    snacks.clear();
    ghosts.clear();
    initSprites();
    eatenSnacks = 0;
    fruitsCount = 0;
}

unsigned GameForm::getLevel() const {
    return level;
}

list<Snack *> &GameForm::getSnacks() {
    return snacks;
}

list<Ghost *> &GameForm::getGhosts() {
    return ghosts;
}

void GameForm::raiseScore(int score) {
    this->score += score;
    //if pacman is eating normal,power pellets raise eaten snacks
    if (score <= 50)
        eatenSnacks++;
}

void GameForm::storeRecord() {
    try {
        File file("../res/high_score.txt");
        file.open(ios::out);
        file << score;
    } catch (const file_open_exception &ex) {
        cerr << "error reading high score";
    }
}

void GameForm::readRecord() {
    try {
        File file("../res/high_score.txt");
        file.open(ios::in);
        file >> highScore;
    } catch (const file_open_exception &ex) {
        cerr << "error reading record" << endl;
        highScore = 0;
    }
}

void GameForm::playSound(SoundTracks soundTrack) {
    if (introMusic.getStatus() == sf::Sound::Playing || !soundOn) return;
    switch (soundTrack) {
        case SoundTracks::CHOMP:
            if (deathSound.getStatus() != sf::Sound::Playing)
                chompSound.play();
            break;
        case SoundTracks::DEATH:
            chompSound.stop();
            deathSound.play();
            break;
        case SoundTracks::EAT_GHOST:
            eatGhostSound.play();
            break;
        case SoundTracks::EAT_FRUIT:
            eatFruitSound.play();
            break;
        case SoundTracks::INTERMISSION:
            intermissionSound.play();
            break;
    }
}

const sf::Vector2f &GameForm::getPacmanPosition() const {
    return pacman->getPosition();
}

const Directions &GameForm::getPacmanDirection() const {
    return pacman->getDirection();
}