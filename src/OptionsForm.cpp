#include "OptionsForm.h"

OptionsForm::OptionsForm(Application &context) : Form("../res/map_menu.txt", context) {
    initMenuView();
    initImages();
}

OptionsForm::~OptionsForm() {
    delete menuView;
    delete logoIV;
}

void OptionsForm::pollEvents(sf::Event &event, sf::RenderWindow *window) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Enter:
                switch (menuView->getSelectedItemIndex()) {
                    case 0:
                        getApplicationContext().getDialog()
                                .create("Reset game",
                                        "clear all game data",
                                        "reset",
                                        "cancel",
                                        [&]() -> void {
                                            getApplicationContext().resetGame();
                                            getApplicationContext().pushFront(
                                                    new GameForm(
                                                            getApplicationContext()));
                                            getApplicationContext().getDialog().hide();
                                        },
                                        [&]() -> void {
                                            getApplicationContext().getDialog().hide();

                                        }).show();
                        break;
                    case 1:
                        getApplicationContext().getDialog()
                                .create("Clear Record",
                                        "reset game record",
                                        "clear",
                                        "cancel",
                                        [&]() -> void {

                                            try {
                                                File file("../res/high_score.txt");
                                                file.open(ios::out);
                                                file << 0;
                                            } catch (const file_open_exception &ex) {
                                                cerr << "error resetting high score";
                                            }

                                            getApplicationContext().getDialog().hide();
                                        },
                                        [&]() -> void {
                                            getApplicationContext().getDialog().hide();

                                        }).show();
                        break;
                    case 2:
                        getApplicationContext().popForm();
                        break;
                }
                break;
            case sf::Keyboard::Down:
                menuView->selectItem(menuView->getSelectedItemIndex() + 1);
                break;
            case sf::Keyboard::Up:
                menuView->selectItem(menuView->getSelectedItemIndex() - 1);
                break;
            default:
                break;
        }
    }
}

void OptionsForm::update(sf::RenderWindow *window, const sf::Time &dt) {
}

void OptionsForm::render(sf::RenderWindow *window) {
    menuView->render(window);
    window->draw(*logoIV);
}

void OptionsForm::initMenuView() {
    menuView = new MenuView(sf::Vector2f(Config::videoMode.width / 2.f, 325));
    menuView->pushItem("reset");
    menuView->pushItem("clear score");
    menuView->pushItem("back to menu");
    menuView->selectItem(0);
}

void OptionsForm::initImages() {
    logoIV = new sf::RectangleShape({385, 91});
    logoIV->setOrigin(logoIV->getLocalBounds().width / 2, 0);
    logoIV->setPosition(Config::videoMode.width / 2, 187);
    if (!logoSrc.loadFromFile("../res/images/logo.png")) throw runtime_error("Cannot open resource");
    logoIV->setTexture(&logoSrc);
}