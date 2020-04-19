#include "NetMsgPause.h"

#include <iostream>
#include <sstream>
#include <string>

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

NetMsgMacroDefinition(NetMsgPause);

bool NetMsgPause::toStream(NetMessengerStreamBuffer& os) const {
	std::string out = paused ? "true" : "false";
	os << out;
	return true;
}

bool NetMsgPause::fromStream(NetMessengerStreamBuffer& is) {
	std::string in;
	is >> in;
	paused = in == "true" ? true : false;
	
	return true;
}

void NetMsgPause::onMessageArrived() {
	if (paused) {
		((RacingModule*) ManagerGLView::getGLView())->pauseMenu->pauseGame();
	} else {
		((RacingModule*) ManagerGLView::getGLView())->pauseMenu->resumeGame();
	}
}

std::string NetMsgPause::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "   Message: " << this->paused << std::endl;
	return ss.str();
}

#endif