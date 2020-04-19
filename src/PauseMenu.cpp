#include "PauseMenu.h"

#include "AftrAPI.h"
#include "RacingModule.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

PauseMenu::PauseMenu() {
    this->exit = GUI::New(nullptr, 0.0f, 0.2f, "../mm/images/pause-menu/exitbutton.png");
    this->exit->setLabel("Pause Menu (Exit)");
    this->exit->isFocusable(false);
    this->exit->receivesEvents(false);
    this->exit->setPosition(Vector(0.5f, 0.35f, 0.0f));
    this->resume = GUI::New(nullptr, 0.0f, 0.2f, "../mm/images/pause-menu/resumebutton.png");
    this->resume->setLabel("Pause Menu (Resume)");
    this->resume->isFocusable(false);
    this->resume->receivesEvents(false);
    this->resume->setPosition(Vector(0.5f, 0.65f, 0.0f));
    //this->restart = GUI::New(nullptr, 0.0f, 0.1f, "../mm/images/pause-menu/restart.png");
    //this->restart->setLabel("Pause Menu (Restart)");
    //this->restart->isFocusable(false);
    //this->restart->receivesEvents(false);
    //this->restart->setPosition(Vector(0.5f, 0.5f, 0.0f));
}

void PauseMenu::pauseGame() {
    this->setExitWidth(0.5f);
    this->setResumeWidth(0.5f);
    //this->setRestartWidth(0.5f);
    this->paused = true;
    ((RacingModule*) ManagerGLView::getGLView())->getHandlerMouseState()->reset();
    //((RacingModule*) ManagerGLView::getGLView())->toggleBanner(false);
}

void PauseMenu::resumeGame() {
    this->setExitWidth(0.0f);
    this->setResumeWidth(0.0f);
    //this->setRestartWidth(0.0f);
    this->paused = false;
    //((RacingModule*) ManagerGLView::getGLView())->toggleBanner(true);
}

void PauseMenu::exitGame() {
    SDL_Event e = SDL_Event();
    e.type = SDL_QUIT;
    ((RacingModule*) ManagerGLView::getGLView())->handleEvent(e);
}

#endif