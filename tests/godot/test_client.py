import socket
import subprocess
import sys
import time

HOST = "127.0.0.1"
PORT = 9000


def wait_for_server():
    for _ in range(50):
        try:
            return socket.create_connection((HOST, PORT), timeout=1)
        except ConnectionRefusedError:
            time.sleep(0.1)

    raise RuntimeError("Server did not start")


def main():
    if len(sys.argv) < 5:
        print(
            "Usage: python3 test_client.py <server_app> <godot_bin> <project_path> <script_path>"
        )
        sys.exit(1)

    server_app = sys.argv[1]
    godot_bin = sys.argv[2]
    project_path = sys.argv[3]
    script_path = sys.argv[4]

    server = None

    try:
        server = subprocess.Popen(
            [server_app],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        with wait_for_server():
            pass

        godot_process = subprocess.run(
            [godot_bin, "--headless", "--path", project_path, "-s", script_path],
            capture_output=True,
            text=True,
        )

        if godot_process.stdout:
            print(godot_process.stdout)

        if godot_process.returncode != 0:
            print("Godot test failed with returncode:", godot_process.returncode)
            if godot_process.stderr:
                print(godot_process.stderr)
            sys.exit(godot_process.returncode)

        print("PASS")

    except Exception as error:
        print("FAIL")
        print(error)
        sys.exit(1)

    finally:
        if server is not None:
            server.terminate()
            server.wait()


if __name__ == "__main__":
    main()
