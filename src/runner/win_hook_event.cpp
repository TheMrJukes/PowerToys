#include "pch.h"
#include "win_hook_event.h"
#include "powertoy_module.h"
#include <mutex>
#include <deque>
#include <thread>

static std::mutex mutex;
static std::deque<WinHookEvent> hook_events;
static std::condition_variable dispatch_cv;
static std::map<DWORD, HWINEVENTHOOK> hook_handles;

static void CALLBACK win_hook_event_proc(HWINEVENTHOOK winEventHook,
                                         DWORD event,
                                         HWND window,
                                         LONG object,
                                         LONG child,
                                         DWORD eventThread,
                                         DWORD eventTime) {
  std::unique_lock lock(mutex);

  wchar_t blah[256];
  wsprintf(blah, L"%d\n", event);
  OutputDebugString(blah);

  hook_events.push_back({ event,
                          window,
                          object,
                          child,
                          eventThread,
                          eventTime });
  lock.unlock();
  dispatch_cv.notify_one();
}

static bool running = false;
static std::thread dispatch_thread;
static void dispatch_thread_proc() {
  std::unique_lock lock(mutex);
  while (running) {
    dispatch_cv.wait(lock, []{ return !running || !hook_events.empty(); });
    if (!running)
      return;
    while (!hook_events.empty()) {
      auto event = hook_events.front();
      hook_events.pop_front();
      lock.unlock();
      powertoys_events().signal_event(win_hook_event, reinterpret_cast<intptr_t>(&event));
      lock.lock();
    }
  }
}

void start_win_hook_event(DWORD event) {
  std::lock_guard lock(mutex);
  if (!running) {
    dispatch_thread = std::thread(dispatch_thread_proc);
    running = true;
  }

  if (hook_handles.find(event) == hook_handles.cend()) {
    hook_handles[event] = SetWinEventHook(event, event, nullptr, win_hook_event_proc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  }
}

void stop_win_hook_event(DWORD event) {
	std::lock_guard lock(mutex);
	if (running)
	{
		auto iter = hook_handles.find(event);
		if (iter != hook_handles.cend())
		{
			UnhookWinEvent(hook_handles[event]);
			hook_handles.erase(event);
		}
	}
}

void stop_all_win_hook_events() {
  std::unique_lock lock(mutex);
  running = false;
  for (auto const& hook : hook_handles) {
    UnhookWinEvent(hook.second);
  }
  lock.unlock();
  dispatch_cv.notify_one();
  dispatch_thread.join();
  lock.lock();
  hook_events.clear();
  hook_events.shrink_to_fit();
}
