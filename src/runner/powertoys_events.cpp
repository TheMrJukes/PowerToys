#include "pch.h"
#include "powertoys_events.h"
#include "lowlevel_keyboard_event.h"
#include "win_hook_event.h"

PowertoysEvents& powertoys_events() {
  static PowertoysEvents powertoys_events;
  return powertoys_events;
}

void PowertoysEvents::start_winhook_event(DWORD event) {
  start_win_hook_event(event);
}

void PowertoysEvents::stop_winhook_event(DWORD event) {
  stop_win_hook_event(event);
}

void PowertoysEvents::register_receiver(const std::wstring & event, PowertoyModuleIface* module) {
  std::unique_lock lock(mutex);
  auto& subscribers = receivers[event];
  if (subscribers.empty()) {
    first_subscribed(event, module);
  }
  else {
    update_subscribed(event, module);
  }
  subscribers.push_back(module);
}

void PowertoysEvents::unregister_receiver(PowertoyModuleIface* module) {
  std::unique_lock lock(mutex);
  for (auto&[event, subscribers] : receivers) {
    subscribers.erase(remove(begin(subscribers), end(subscribers), module), end(subscribers));
    if (subscribers.empty()) {
      last_unsubscribed(event);
    }
  }
}

intptr_t PowertoysEvents::signal_event(const std::wstring & event, intptr_t data) {
  intptr_t rvalue = 0;
  std::shared_lock lock(mutex);
  if (auto it = receivers.find(event); it != end(receivers)) {
    for (auto& module : it->second) {
      if (module)
        rvalue |= module->signal_event(event.c_str(), data);
    }
  }
  return rvalue;
}

void PowertoysEvents::first_subscribed(const std::wstring& event, PowertoyModuleIface* module) {
  if (event == ll_keyboard) {
    start_lowlevel_keyboard_hook();
  }
  else if (event == win_hook_event) {
    auto events = module->get_winhook_events(this);
	for (auto const& blah : events) {
		start_win_hook_event(blah);
	}
  }
}

void PowertoysEvents::last_unsubscribed(const std::wstring& event) {
    if (event == ll_keyboard) {
      stop_lowlevel_keyboard_hook();
    }
    else if (event == win_hook_event) {
      stop_all_win_hook_events();
    }
}

void PowertoysEvents::update_subscribed(const std::wstring& event, PowertoyModuleIface* module) {
    if (event == win_hook_event) {
		auto events = module->get_winhook_events(this);
		for (auto const& blah : events) {
		  start_win_hook_event(blah);
		}
    }
}
