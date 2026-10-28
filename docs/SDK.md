# C++ SDK

## Build and link

The SDK is currently built as part of the `karkinolution` CMake target; there is no separate installable SDK package or
library target.

From the repository root, build the library with:

```bash
cmake --preset debug
cmake --build --preset debug --target karkinolution
```

In a CMake client that includes this repository as a subdirectory, link against the target:

```cmake
add_subdirectory(path/to/Karkinolution)
target_link_libraries(my_client PRIVATE karkinolution)
```

The target provides the repository's `src` include root and its required dependencies. The SDK's client and domain
interpreter headers are under `karkinolution/networking/sdk/src/`. Prefer the client and interpreter APIs; binary
protocol, frame, and transport headers are implementation details.

## Client lifecycle

Create an `asio::io_context` and run it while the client is connected. `Client::connect()` establishes the TCP
connection synchronously. Then call `request()` or the asynchronous request-queue methods. When finished, call
`disconnect()`, `stop_new_async_operations()`, and `wait_until_zero_async_operations()` before destroying the client
and stopping the I/O thread.

`Client::request()` blocks for its response, up to the SDK's fixed five-second wait. It returns an expected optional
response variant:

- A response frame or server error frame is present when the request receives a frame.
- A `std::nullopt` represents no response value.
- An unexpected `CancelledError` indicates the wait timed out.
- Connection failures from `connect()` are reported as `ConnectionError`. Connection loss while waiting for a request
  is reported as `ClientError`.

The low-level client returns server error frames as part of the response variant; domain interpreters convert those
frames to `ClientError`. Interpreter functions may also return `std::nullopt` when the requested entity or value does
not exist. Currently, interpreters also map a cancelled or missing low-level result to `std::nullopt`, so callers using
an interpreter cannot distinguish a timeout from an absent entity.

## Requests and concurrency

`request()` is synchronous and blocks the calling thread. For non-blocking submission, use
`send_without_waiting()`, which returns a request ID, and later call `pop_response()` with that ID. The SDK associates
responses with request IDs, but the current client does not restart its asynchronous read after receiving data. As a
result, do not pipeline requests or rely on sending another request after a response on the same connection. Multiple
pending IDs are tracked internally, but that does not currently guarantee that each response will be read. Keep the
`io_context` running while a request is in flight.

## Example

This example requests a creature through the SDK's domain interpreter. Replace `1` with an ID available from the
server.

```cpp
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <karkinolution/networking/sdk/src/interpreters/creature.hpp>

#include <exception>
#include <iostream>
#include <thread>

int main() {
	asio::io_context io;
	auto             work_guard = asio::make_work_guard(io);
	std::thread      io_thread([&io] {
		io.run();
	});

	int exit_code = 0;
	{
		KarkinolutionSDK::Client::Client client{
			io,
			KarkinolutionSDK::Client::Config{.host = "127.0.0.1", .port = 9000}};

		try {
			client.connect();
			const auto creature = KarkinolutionSDK::Interpreters::Creature::see_creature(client, 1);
			if (!creature) {
				std::cerr << "Creature was not found\n";
				exit_code = 1;
			}
		} catch (const std::exception &error) {
			std::cerr << "SDK request failed: " << error.what() << '\n';
			exit_code = 1;
		}

		client.disconnect();
		client.stop_new_async_operations();
		client.wait_until_zero_async_operations();
	}

	work_guard.reset();
	io.stop();
	io_thread.join();
	return exit_code;
}
```

The public SDK boundary is the client API and domain interpreters. Raw frame layouts, protocol encoding, and transport
implementation are internal and should not be constructed or depended on by client applications.
