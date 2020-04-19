#pragma once

#include "AftrAPI.h"
#include "RacingModule.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgPause : public NetMsg {
		public:
			NetMsgMacroDeclaration(NetMsgPause);

			virtual ~NetMsgPause() {};
			virtual bool toStream(NetMessengerStreamBuffer& os) const;
			virtual bool fromStream(NetMessengerStreamBuffer& is);
			virtual void onMessageArrived();
			virtual std::string toString() const;

			bool paused;
	};
}

#endif