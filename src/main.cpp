#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <future>

using namespace std::chrono_literals;

void case1();
void case2();
void case3();
void case4();
void case5();
void case6();

int main()
{
	std::cout << "=== Starting Case 1 ===\n";
	case1();

	std::cout << "\n=== Starting Case 2 ===\n";
	case2();

	std::cout << "\n=== Starting Case 3 ===\n";
	case3();

	std::cout << "\n=== Starting Case 4 ===\n";
	case4();

	std::cout << "\n=== Starting Case 5 ===\n";
	case5();

	std::cout << "\n=== Starting Case 6 ===\n";
	case6();

	return 0;
}

// ============================================

// Case 1: Demonstrates this_thread and sleep_for
void case1()
{
	std::cout << "Case 1: Starting sleep for 1 second...\n";
	auto start = std::chrono::high_resolution_clock::now();

	std::this_thread::sleep_for(1s);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Case 1: Slept for " << elapsed.count() << " seconds\n";
}

// Case 2: Demonstrates mutex, atomic, and lock_guard
void case2()
{
	std::mutex mtx;
	std::atomic<int> atomic_counter{0};
	int mutex_counter = 0;
	int regular_counter = 0;
	constexpr int iterations = 100000;

	auto worker = [&]()
	{
		for (int i = 0; i < iterations; ++i)
		{
			// Regular operation - no lock
			regular_counter++;

			// Atomic operation - no lock needed
			atomic_counter++;

			// Regular variable protected by mutex
			std::lock_guard<std::mutex> mutex_counter_guard(mtx);
			mutex_counter++;
		}
	};

	std::thread t1(worker);
	std::thread t2(worker);

	t1.join();
	t2.join();

	std::cout << "Case 2: Regular counter: " << regular_counter << "\n";
	std::cout << "Case 2: Atomic counter: " << atomic_counter << "\n";
	std::cout << "Case 2: Mutex counter: " << mutex_counter << "\n";
}

// Case 3: Demonstrates unique_lock, condition_variable, and try_lock
void case3()
{
	std::mutex mtx;
	std::condition_variable cv;

	bool cv_data_ready = false;
	bool cv_data_processed = false;

	// Worker thread
	auto worker = [&]()
	{
		std::unique_lock<std::mutex> lock(mtx);

		// Wait until main() sends data
		cv.wait(lock, [&]
				{ return cv_data_ready; });

		std::cout << "Case 3: Worker thread is processing data\n";
		std::this_thread::sleep_for(1s);

		// Send data back to main()
		cv_data_processed = true;
		std::cout << "Case 3: Worker thread signals data processing completed\n";

		lock.unlock();
		cv.notify_one();
	};

	std::thread t(worker);

	// Simulate data preparation
	{
		std::lock_guard<std::mutex> lock(mtx);
		std::cout << "Case 3: Main thread is preparing data\n";
		std::this_thread::sleep_for(1s);
		cv_data_ready = true;
		std::cout << "Case 3: Main thread signals data cv_data_ready for processing\n";
	}
	cv.notify_one();

	// Wait for the worker
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [&]
				{ return cv_data_processed; });
	}

	std::cout << "Case 3: Back in main(), data processing completed\n";

	t.join();
}

// Case 4: Demonstrates try_lock with retry logic for critical work
void case4()
{
	std::mutex critical_mtx;
	bool critical_work_done = false;

	// Worker thread that does non-critical work first, then tries to do critical work
	auto worker = [&]()
	{
		// Non-critical work (can proceed even if mutex is locked)
		std::cout << "Case 4: Worker doing non-critical work (always runs)\n";
		std::this_thread::sleep_for(500ms);

		// Try to acquire lock for critical work (may fail first time)
		int attempts = 0;
		while (attempts < 3)
		{
			if (critical_mtx.try_lock())
			{
				std::cout << "Case 4: Worker acquired lock for critical work (attempt "
						  << attempts + 1 << ")\n";

				// Critical work
				std::cout << "Case 4: Worker performing CRITICAL work\n";
				std::this_thread::sleep_for(1s);
				critical_work_done = true;

				critical_mtx.unlock();
				break;
			}
			else
			{
				std::cout << "Case 4: Worker failed to acquire lock (attempt "
						  << attempts + 1 << "), doing alternative work\n";
				std::this_thread::sleep_for(300ms);
				attempts++;
			}
		}

		if (!critical_work_done)
		{
			std::cout << "Case 4: Worker gave up after 3 attempts\n";
		}
	};

	// Main thread holds the lock initially to demonstrate try_lock failure
	std::thread t(worker);

	std::cout << "Case 4: Main thread locking mutex for 1.5s\n";
	critical_mtx.lock();
	std::this_thread::sleep_for(500ms); // Hold lock long enough for worker to fail first try

	std::cout << "Case 4: Main thread releasing mutex\n";
	critical_mtx.unlock();

	t.join();
}

// Case 5: Demonstrates promise/future and async
void case5()
{
	std::cout << "\n=== Case 5: Promise/Future and Async ===" << std::endl;

	// ----------------------------
	// Part 1: Promise and Future
	// ----------------------------
	std::promise<int> promise;
	std::future<int> future = promise.get_future();

	auto worker = [](std::promise<int> &&prom)
	{
		std::cout << "Worker thread: Doing some computation..." << std::endl;
		std::this_thread::sleep_for(1s);
		prom.set_value(42); // Set the value in promise
		std::cout << "Worker thread: Value set in promise" << std::endl;
	};

	std::thread t(worker, std::move(promise));
	std::cout << "Main thread: Waiting for result..." << std::endl;

	// Blocks until the future has a valid result
	int result = future.get();
	std::cout << "Main thread: Got result from future: " << result << std::endl;
	t.join();

	// ----------------------------
	// Part 2: Async with launch policy
	// ----------------------------
	auto async_task = []()
	{
		std::cout << "Async task: Starting heavy computation..." << std::endl;
		double sum = 0;
		for (int i = 0; i < 10000000; i++)
		{
			sum += std::sin(i) * std::cos(i);
		}
		return sum;
	};

	// Launch the async task (explicitly using async policy)
	std::future<double> async_result = std::async(std::launch::async, async_task);

	std::cout << "Main thread: Doing other work while async task runs..." << std::endl;
	std::this_thread::sleep_for(500ms);

	// Get the result (will block if not ready)
	double value = async_result.get();
	std::cout << "Main thread: Got async result: " << value << std::endl;

	// ----------------------------
	// Part 3: Multiple futures
	// ----------------------------
	auto square = [](int x)
	{
		std::this_thread::sleep_for(500ms);
		return x * x;
	};

	std::future<int> future1 = std::async(std::launch::async, square, 5);
	std::future<int> future2 = std::async(std::launch::async, square, 6);
	std::future<int> future3 = std::async(std::launch::async, square, 7);

	std::cout << "Main thread: Waiting for multiple futures..." << std::endl;
	std::cout << "Results: "
			  << future1.get() << ", "
			  << future2.get() << ", "
			  << future3.get() << std::endl;
}

void case6()
{
	std::cout << "\n=== Case 6: detach() vs async with future ===" << std::endl;

	// --------------------------------------------------
	// Option 1: Using std::thread with detach()
	// --------------------------------------------------
	std::cout << "Creating detached thread...\n";
	std::thread t1([]()
				   {
					   std::cout << "Detached thread starting work...\n";
					   std::this_thread::sleep_for(1s);
					   std::cout << "Detached thread completed work\n";
					   // Note: No way to get return value or know exactly when it finishes
				   });
	t1.detach(); // Thread runs independently

	// Main thread continues immediately
	std::cout << "Main thread continues after detach()\n";
	std::this_thread::sleep_for(500ms); // Give detached thread time to start

	// --------------------------------------------------
	// Option 2: Using std::async with future
	// --------------------------------------------------
	std::cout << "\nLaunching async task...\n";
	auto future = std::async(std::launch::async, []()
							 {
								 std::cout << "Async task starting work...\n";
								 std::this_thread::sleep_for(1s);
								 std::cout << "Async task completed work\n";
								 return 42; // Can return a value
							 });

	// Main thread can do other work while async task runs
	std::cout << "Main thread doing other work...\n";
	std::this_thread::sleep_for(500ms);

	// When we need the result (or want to ensure completion)
	std::cout << "Main thread waiting for async result...\n";
	int result = future.get(); // Blocks until task completes
	std::cout << "Got async result: " << result << "\n";

	// --------------------------------------------------
	// Key Differences Demonstration
	// --------------------------------------------------
	std::cout << "\nKey Differences:\n";
	std::cout << "1. detached thread:\n";
	std::thread t2([]()
				   {
        std::this_thread::sleep_for(500ms);
        std::cout << "  - No return value possible\n";
        std::cout << "  - No way to know when done\n";
        std::cout << "  - Risk of program ending before thread completes\n"; });
	t2.detach();

	auto future2 = std::async(std::launch::async, []()
							  {
        std::this_thread::sleep_for(500ms);
        return std::string("  - Can get return values\n"
                           "  - Can wait for completion\n"
                           "  - Automatic thread management\n"); });

	std::cout << "2. async with future:\n";
	std::cout << future2.get(); // Get the comparison points

	// Give detached threads time to complete before program exits
	std::this_thread::sleep_for(1s);
}
