#pragma once

#include "GUI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
    class ProgressBar {
        public:
            static ProgressBar* New() { return new ProgressBar(); };
            virtual ~ProgressBar() {};

            float getProgressWidth() { return progress->getWidthGUI(); };
            float getBarWidth() { return bar->getWidthGUI(); };
            void setProgressWidth(float width) { progress->setWidthGUI(width); };
            void setBarWidth(float width) { bar->setWidthGUI(width); }
            void addToProgress(float toAdd) { progress->setWidthGUI(getProgressWidth() + toAdd); };

            GUI* bar;
            GUI* progress;

            bool fill = false;
            bool empty = false;

            void update();
            void fillBar();
            void emptyBar();
            void toggleFill();
            void toggleEmpty();
            void reset();
            void hide();
            void show();

            bool isFull();
            bool isHidden();

        protected:
            ProgressBar();
    };
}

#endif