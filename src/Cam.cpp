#include "Cam.h"

using namespace Aftr;

Cam::Cam(Camera* cam) {
    this->camera = cam;
    this->location = WOGUILabel::New(nullptr);
    this->location->setText("0");
    this->location->setColor(255, 0, 0, 255);
    this->location->setFontSize(30.f); //font size is correlated with world size
    this->location->setPosition(Vector(0.5f, 0.97f, 0.f));
    this->location->setFontPath(ManagerEnvironmentConfiguration::getSMM() + "/fonts/arial.ttf");
};

void Cam::onKeyDown(const SDL_KeyboardEvent& key) {
    if (this->isMovementKey(key.keysym.sym)) {
        std::set<SDL_Keycode>::iterator found = keysPressed.find(key.keysym.sym);
        if (found == keysPressed.end()) {
            keysPressed.insert(key.keysym.sym);
        }
    }
}

void Cam::onKeyUp(const SDL_KeyboardEvent& key) {
    if (this->isMovementKey(key.keysym.sym)) {
        std::set<SDL_Keycode>::iterator found = keysPressed.find(key.keysym.sym);
        if (found != keysPressed.end()) {
            keysPressed.erase(found);
        }
    }
}

void Cam::update() {
    //this->location->setText(getLocString());
    for (SDL_Keycode key : this->keysPressed) {
        switch (key) {
            case SDLK_w:
            case SDLK_UP:
                this->camera->moveInLookDirection(5);
                break;
            case SDLK_a:
            case SDLK_LEFT:
                for (int i = 0; i < 5; i++) this->camera->moveLeft();
                break;
            case SDLK_s:
            case SDLK_DOWN:
                this->camera->moveOppositeLookDirection(5);
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                for (int i = 0; i < 5; i++) this->camera->moveRight();
                break;
            case SDLK_0:
                this->camera->setPosition(Vector(0.f, 0.f, 0.f));
                this->camera->rotateToIdentity();
                break;
        }
    }
}

std::string Cam::getLocString() {
    return this->camera->getPosition().toString();
}

bool Cam::isMovementKey(SDL_Keycode key) {
    return (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_DOWN || key == SDLK_RIGHT ||
        key == SDLK_w || key == SDLK_a || key == SDLK_s || key == SDLK_d || key == SDLK_LSHIFT);
}