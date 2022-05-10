#include "darknet7_base_state.h"

Darknet7BaseState::Darknet7BaseState(DarkNet7* darknet)
    : darknet(darknet),
    VKB(new VirtualKeyBoard(darknet))
{}
