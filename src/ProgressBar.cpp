#include "ProgressBar.h"

#include "RacingModule.h"
#include "AftrManagers.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

ProgressBar::ProgressBar() {
    this->bar = GUI::New(nullptr, 0.0, 0.1f, "../mm/images/progress-bar/panel.png");
    this->progress = GUI::New(nullptr, 0.0f, 0.1f, "../mm/images/progress-bar/red.png");
    this->bar->setLabel("Progress Bar (Bar)");
    this->progress->setLabel("Progress Bar (Progress)");
    this->bar->isFocusable(false);
    this->progress->isFocusable(false);
    this->bar->receivesEvents(false);
    this->progress->receivesEvents(false);
    this->bar->setPosition(Vector(0.15f, 0.05f, 0.0f));
    this->progress->setPosition(Vector(-0.15f, 0.05f, 0.0f));
}

void ProgressBar::update() {
    if (this->fill) {
        this->fillBar();
    }
    else if (this->empty) {
        this->emptyBar();
    }
    if (this->isFull()) {
        this->reset();
    }
}

void ProgressBar::fillBar() {
    if (this->isHidden()) {
        this->show();
    }
    if (this->getProgressWidth() < 0.89f) {
        this->addToProgress(0.01f);
    } else {
        this->fill = false;
    }
}

void ProgressBar::emptyBar() {
    if (this->getProgressWidth() > 0.3f) {
        this->addToProgress(-0.01f);
    } else {
        this->empty = false;
        this->hide();
    }
}

void ProgressBar::toggleFill(){
    if (this->fill) {
        this->fill = false;
    } else {
        this->fill = true;    
    }
    this->empty = false;
}

void ProgressBar::toggleEmpty() {
    if (this->empty) {
        this->empty = false;
    } else {
        this->empty = true;
    }
    this->fill = false;
}

void ProgressBar::reset() {
    this->setProgressWidth(0.3f);
    this->fill = false;
    this->empty = false;
    this->hide();
}

bool ProgressBar::isFull() {
    return this->getProgressWidth() >= 0.89f;
}

void ProgressBar::hide() {
    this->setBarWidth(0.0f);
    this->setProgressWidth(0.0f);
}

void ProgressBar::show() {
    this->setBarWidth(0.3f);
    this->setProgressWidth(0.3f);
}

bool ProgressBar::isHidden() {
    return (this->getBarWidth() == 0.0f && this->getProgressWidth() == 0.0f);
}

#endif