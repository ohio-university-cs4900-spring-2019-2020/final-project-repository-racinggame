#pragma once

#include <iostream>
#include "WOGUI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
    class WO;

	class GUI : public WOGUI {
        public:
            WOMacroDeclaration(GUI, WOGUI);
		    static GUI* New(WOGUI* parentWOGUI = nullptr, float width = 1.0f, float height = 1.0f, std::string panelTex = "");
		    virtual ~GUI() {};

            virtual void onMouseDown(const SDL_MouseButtonEvent& e);
            virtual void onMouseUp(const SDL_MouseButtonEvent& e) {};
            virtual void onMouseClicked(const SDL_MouseButtonEvent& e) {};
            virtual void onMouseMove(const SDL_MouseMotionEvent& e) {};
            virtual void onMouseEnter(const SDL_MouseMotionEvent& e) {};
            virtual void onMouseLeave(const SDL_MouseMotionEvent& e) {};
            virtual void onKeyDown(const SDL_KeyboardEvent& e) {};
            virtual void onKeyUp(const SDL_KeyboardEvent& e) {};
            virtual void onHasFocus() {}; //WOGUI::onHasFocus(); };
            virtual void onLostFocus() {}; //WOGUI::onLostFocus(); };

            bool isFocusable() { return WOGUI::isFocusable(); };
            void isFocusable(bool isFocusable) { WOGUI::isFocusable(isFocusable); };
            bool hasFocus() { return WOGUI::hasFocus();  };
            bool receivesEvents() { return WOGUI::receivesEvents();  };
            void receivesEvents(bool receiveUIEvents) { WOGUI::receivesEvents(receiveUIEvents); };

            bool pixelResidesInBoundingRect(int x, int y);
    
        protected:
            GUI();
       
	};
}

#endif