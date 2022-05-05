#include "EventHandler.h"

void EventHandler::Initialize() { Get(); }

void EventHandler::PollEvents() { glfwPollEvents(); }
