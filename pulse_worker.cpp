#include "pulse_worker.h"
#include "core.h"

pulse_worker::pulse_worker (channels_mask_t channels)
	: mask (channels)
{ }

pulse_worker::pulse_worker (pulse_worker&& rhs)
	: mask (rhs.mask)
	, thread()
	, mutex()
	, condvar()
	, queue()
{ }

void pulse_worker::add_to_queue (std::chrono::_V2::steady_clock::time_point pulse_end)
{
	lock_guard_t L (mutex);
	queue.emplace (pulse_end);
	condvar.notify_all();
}

void pulse_worker::loop()
{
	lock_guard_t L (mutex);
	bool state = false;

	for (; !Core::instance.is_destroying();) {
		while (!Core::instance.is_destroying() && queue.empty()) {
			condvar.wait (L);
		}

		if (queue.empty()) {
			continue;
		}

		utils::clock::time_point sleep_until = queue.front();
		queue.pop();

		mutex.unlock();
		if (!state) {
			state = true;
			Core::instance.edge (mask, true);
		}
		std::this_thread::sleep_until (sleep_until);
		mutex.lock();

		if (queue.empty()) {
			state = false;
			Core::instance.edge (mask, false);
		}
	}
}

void pulse_worker::run()
{
	thread = std::thread (&pulse_worker::loop, this);
}

void pulse_worker::join()
{
	if (thread.joinable()) {
		thread.join();
	}
}