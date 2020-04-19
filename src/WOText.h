#pragma once

#include "AftrAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class WOText : public WOFTGLString {
    public:
        static WOText* New(const std::string& fontPath, float fontSize, std::string initial = "");
        virtual ~WOText() {}

        Model* getModel() { return WOFTGLString::getModel(); }
        MGLFTGLString* getModelT() { return WOFTGLString::getModelT<MGLFTGLString>(); }
        std::string getText() { return WOFTGLString::getText(); }
        Mat4 getDisplayMatrix() { return getModel()->getDisplayMatrix(); }

        void setText(const std::string& text) { WOFTGLString::setText(text); }
        void setText(const std::wstring& text) { WOFTGLString::setText(text); }      
        void setLookDirection(Vector dir) { getModel()->setLookDirection(dir); }
        void setNormalDirection(Vector dir) { getModel()->setNormalDirection(dir); }
        void setScale(Vector scale) { getModel()->setScale(scale); }
        void setColor(aftrColor4f color) { getModelT()->setFontColor(color); }
        void setSize(float width, float height) { getModelT()->setSize(30, 10); }
        void setFontAspectRatioType(const FONT_ASPECT_RATIO_TYPE& aspectType) { getModelT()->setFontAspectRatioType(aspectType); }
        void setDisplayMatrix(Mat4 matrix) { getModel()->setDisplayMatrix(matrix); }

        void type(const SDL_Keycode& key);

        void startingRot();

        bool typing = false;
        bool shift = false;

    protected:
        WOText();
        virtual void onCreate(const std::string& fontPath, float fontSize, std::string initial = "");
        char getShiftedChar(char c);
	};
}

#endif