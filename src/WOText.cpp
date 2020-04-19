#include "WOText.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

WOText* WOText::New(const std::string& fontPath, float fontSize, std::string initial) {
	WOText* wo = new WOText();
	wo->onCreate(fontPath, fontSize, initial);
	return wo;
}

WOText::WOText() : IFace(this), WOFTGLString() {}

void WOText::onCreate(const std::string& fontPath, float fontSize, std::string initial) {
	WOFTGLString::onCreate(fontPath, fontSize);
	this->setText(initial);
    this->setLookDirection(Vector(0, -1, 0));
    this->setNormalDirection(Vector(-1, 0, 0));
	this->setFontAspectRatioType(FONT_ASPECT_RATIO_TYPE::FIXED_BY_HEIGHT);
}

void WOText::startingRot() {
    float starting[16] = {
        0, -1, 0, 0,
        0, 0, 1, 0,
        -1, 0, 0, 0,
        0, 0, 0, 1
    };
    this->setDisplayMatrix(Mat4(starting));
}

void WOText::type(const SDL_Keycode& key) {
    std::string text = this->getText();
    if (key == SDLK_BACKSPACE) {
        if (text.size() < 2) {
            text = "";
        } else {
            text.pop_back();
        }
    } else if (this->shift) {
        text += this->getShiftedChar(key);
    } else {
        text += key;
    }
    this->setText(text);
}

char WOText::getShiftedChar(char c) {
    if (c < 123 && c > 96) {
        return (c - 32);
    }
    switch (c) {
        case 39: return 34; // ' -> "
        case 44: return 60; // , -> <
        case 45: return 95; // - -> _
        case 46: return 62; // . -> >
        case 47: return 63; // / -> ?
        case 48: return 41; // 0 -> )
        case 49: return 33; // 1 -> !
        case 50: return 64; // 2 -> @
        case 51: return 35; // 3 -> #
        case 52: return 36; // 4 -> $
        case 53: return 37; // 5 -> %
        case 54: return 94; // 6 -> ^
        case 55: return 38; // 7 -> &
        case 56: return 42; // 8 -> *
        case 57: return 40; // 9 -> (
        case 59: return 58; // ; -> :
        case 61: return 43; // = -> +
        case 91: return 123;// [ -> {
        case 92: return 124;// \ -> |
        case 93: return 125;// ] -> }
        case 96: return 126;// ` -> ~
        default: return c;
    }
}

#endif