#pragma once

#include <interface/win_hook_event_data.h>

void start_win_hook_event(DWORD event);
void stop_win_hook_event(DWORD event);
void stop_all_win_hook_events();
