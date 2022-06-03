#include "darknet7_base_state.h"
#include "darknet7.h"

Darknet7BaseState::Darknet7BaseState(DarkNet7* darknet)
    : darknet(darknet),
    VKB(new VirtualKeyBoard(darknet->getGUI(), darknet->getButtonInfo()))
{}
