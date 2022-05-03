#include "EventHandler.h"

void EventHandler::Initialize() { Instance(); }

void EventHandler::PollEvents() { glfwPollEvents(); }
