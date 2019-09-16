#pragma once

#include <interface/win_hook_event_data.h>

void start_win_hook_event(DWORD min, DWORD max);
void stop_win_hook_event();
void update_win_hook_event(DWORD min, DWORD max);

