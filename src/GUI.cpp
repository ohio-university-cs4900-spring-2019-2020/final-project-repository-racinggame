#include "GUI.h"

#include <iostream>
#include "AftrAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

WOMacroDeclaration(GUI, WOGUI);

GUI* GUI::New(WOGUI* parentWOGUI, float width, float height, std::string panelTex) {
	GUI* gui = new GUI();
	gui->onCreate(width, height, panelTex);
	return gui;
}

GUI::GUI() : IFace(this), WOGUI(nullptr) {}

void GUI::onMouseDown(const SDL_MouseButtonEvent& e) { 
	
}

bool GUI::pixelResidesInBoundingRect(int x, int y) { 
	return WOGUI::pixelResidesInBoundingRect(x, y); 
}

#endif