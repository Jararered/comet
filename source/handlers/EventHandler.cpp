#include "EventHandler.h"

EventHandler::EventHandler() {}

EventHandler::~EventHandler() {}

void EventHandler::PollEvents() { glfwPollEvents(); }
