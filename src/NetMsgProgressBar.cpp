#include "NetMsgProgressBar.h"

#include <iostream>
#include <sstream>
#include <string>

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

NetMsgMacroDefinition(NetMsgProgressBar);

NetMsgProgressBar::NetMsgProgressBar(const float width) { this->width = width; }

bool NetMsgProgressBar::toStream(NetMessengerStreamBuffer& os) const {
	os << this->width;
	return true;
}

bool NetMsgProgressBar::fromStream(NetMessengerStreamBuffer& is) {
	is >> this->width;
	return true;
}

void NetMsgProgressBar::onMessageArrived() {
	((RacingModule*) ManagerGLView::getGLView())->progressBar->setProgressWidth(this->width);
	std::cout << this->toString() << std::endl;
}

std::string NetMsgProgressBar::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "\tWidth: " << this->width << "...\n";

	return ss.str();
}

#endif