#pragma once

#include "AftrAPI.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgWO : public NetMsg {
		public:
			NetMsgMacroDeclaration(NetMsgWO);

			static NetMsgWO makeNetMsgWO(Vector position, Mat4 displayMatrix, int index);

			virtual ~NetMsgWO() {};
			virtual bool toStream(NetMessengerStreamBuffer& os) const;
			virtual bool fromStream(NetMessengerStreamBuffer& is);
			virtual void onMessageArrived();
			virtual std::string toString() const;

			Mat4 pose;
			Vector pos;
			int index;			
	};
}

#endif