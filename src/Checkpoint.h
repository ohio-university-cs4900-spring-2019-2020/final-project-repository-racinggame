#pragma once

#include "WOWayPointSpherical.h"
#include "WOWayPointAbstract.h"

namespace Aftr
{
    class Checkpoint : public WOWayPointSpherical {
        public:	
            static Checkpoint* New(const WayPointParametersBase& params, float radius, int num);
            virtual ~Checkpoint();
            virtual void onTrigger();
            virtual int getCheckpointNum() { return checkpointNum; };
        protected:
            Checkpoint(const WayPointParametersBase& params, float radius, int num);
        private:
            int checkpointNum;
    };
} 

