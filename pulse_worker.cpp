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
	, destroying (false)
{ }

void pulse_worker::add_to_queue (std::chrono::_V2::steady_clock::time_point pulse_end)
{
	lock_guard_t L (mutex);
	queue.emplace (pulse_end);
	L.unlock();
	condvar.notify_all();
}

void pulse_worker::loop()
{
	lock_guard_t L (mutex);
	bool state = false;

	for (; !destroying; ) {
		condvar.wait (L, [this] () { return destroying || !queue.empty(); });

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
	destroying = false;
	thread = std::thread (&pulse_worker::loop, this);
}

void pulse_worker::join()
{
	lock_guard_t L (mutex);
	destroying = true;
	L.unlock();
	condvar.notify_all();

	if (thread.joinable()) {
		thread.join();
	}
}