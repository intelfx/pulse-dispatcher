#include "pulse_worker.h"
#include "core.h"

pulse_worker::pulse_worker (channels_mask_t channels, pulse_mode m)
	: mask (channels)
	, destroying (false)
	, mode (m)
{ }

pulse_worker::pulse_worker (pulse_worker&& rhs)
	: mask (rhs.mask)
	, thread()
	, mutex()
	, condvar()
	, queue()
	, destroying (false)
	, mode (rhs.mode)
{ }

void pulse_worker::add_to_queue (utils::clock::time_point pulse_end)
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
	bool ignore_next_pulse = false;

	while (!destroying) {
		condvar.wait (L, [this] () { return destroying || !queue.empty(); });

		if (queue.empty()) {
			continue; // and check `destroying` as part of the while-condition
		}

		utils::clock::time_point sleep_until = queue.front();
		queue.pop();

		mutex.unlock();
		if (!state && !ignore_next_pulse) {
			if (mode == MODE_SINGLE_PULSE) {
				ignore_next_pulse = true;
			}
			state = true;
			Core::instance.edge (mask, true);
		}
		std::this_thread::sleep_until (sleep_until);
		mutex.lock();

		if (state && (queue.empty() || ignore_next_pulse)) {
			state = false;
			Core::instance.edge (mask, false);
		}

		if (queue.empty() && ignore_next_pulse) {
			ignore_next_pulse = false;
		}
	}
}

void pulse_worker::run()
{
	destroying = false;
	thread = std::thread (&pulse_worker::loop, this);
}

void pulse_worker::stop_and_join()
{
	lock_guard_t L (mutex);
	destroying = true;
	L.unlock();
	condvar.notify_all();

	if (thread.joinable()) {
		thread.join();
	}
}