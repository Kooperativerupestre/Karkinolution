#include "karkinolution/networking/client_side/client/alias.hpp"

int main() {
	// This is the starting point for implementing an executable that connects to the server.
	KarkinolutionRuntimeClient client;
	return client.connect() ? 0 : 1;
}
