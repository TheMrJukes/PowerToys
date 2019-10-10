#pragma once
#include <interface/powertoy_module_interface.h>
#include <string>

class PowertoysEvents : public IPowertoysEvents {
public:
  virtual void start_winhook_event(DWORD event) override;
  virtual void stop_winhook_event(DWORD event) override;

  void register_receiver(const std::wstring& event, PowertoyModuleIface* module);
  void unregister_receiver(PowertoyModuleIface* module);
  intptr_t signal_event(const std::wstring& event, intptr_t data);
private:
  void first_subscribed(const std::wstring& event, PowertoyModuleIface* module);
  void last_unsubscribed(const std::wstring& event);
  void update_subscribed(const std::wstring& event, PowertoyModuleIface* module);

  std::shared_mutex mutex;
  std::unordered_map<std::wstring, std::vector<PowertoyModuleIface*>> receivers;
};

PowertoysEvents& powertoys_events();


