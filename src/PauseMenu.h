#pragma once

#include "GUI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {

    class PauseMenu {
        public:
            static PauseMenu* New() { return new PauseMenu(); };
            virtual ~PauseMenu() {};

            float getResumeWidth() { return resume->getWidthGUI(); };
            float getExitWidth() { return exit->getWidthGUI(); };
            float getRestartWidth() { return restart->getWidthGUI(); };
            void setResumeWidth(float width) { resume->setWidthGUI(width); };
            void setExitWidth(float width) { exit->setWidthGUI(width); };
            void setRestartWidth(float width) { restart->setWidthGUI(width); };

            GUI* resume;
            GUI* exit;
            GUI* restart;

            void pauseGame();
            void resumeGame();
            void exitGame();
           
            bool isPaused() { return paused; };

        protected:
            PauseMenu();

        private:
            bool paused = false;

    };
}

#endif