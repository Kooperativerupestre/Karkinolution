import socket
import struct
import subprocess
import sys
import time

HOST = "127.0.0.1"
PORT = 9000

creature_id = 1

frame = (
    struct.pack(">I", 13)
    + struct.pack(">B", 1)
    + struct.pack(">I", 1)
    + struct.pack(">Q", creature_id)
)

expected_response = (
    struct.pack(">I", 25)
    + struct.pack(">B", 2)
    + struct.pack(">I", 1)
    + b"Creature with id = 1"
)


def wait_for_server():
    for _ in range(50):
        try:
            return socket.create_connection((HOST, PORT), timeout=1)
        except ConnectionRefusedError:
            time.sleep(0.1)

    raise RuntimeError("Server did not start")


server = None

try:
    server = subprocess.Popen(
        [sys.argv[1]],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    with wait_for_server() as sock:
        sock.sendall(frame)
        response = sock.recv(1024)

    if response != expected_response:
        print("FAIL")
        print("Expected:", expected_response.hex(" "))
        print("Received:", response.hex(" "))
        sys.exit(1)

    print("PASS")

except Exception as error:
    print("FAIL")
    print(error)
    sys.exit(1)

finally:
    if server is not None:
        server.terminate()
        server.wait()
