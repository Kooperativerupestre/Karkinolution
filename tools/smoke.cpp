// Generic smoke-test harness for Karkinolution.
//
// Drop ANY karkinolution code inside a lambda passed to run_snippet() in
// main(), and it will run it under one of two modes:
//
//   - Captured mode (default): catches exceptions, prints what() and which
//     snippet failed, then keeps going to the next snippet.
//   - Raw mode (--raw / -r): lets the error propagate uncaught, installs a
//     custom terminate handler that prints the exception type, its what(),
//     and a full symbolized backtrace (function + file:line, even for
//     functions in anonymous namespaces) before aborting.
//
// Usage:
//   ./smoke            -> captured mode
//   ./smoke --raw      -> raw mode, crashes loud with a real backtrace

#include <karkinolution/world/world.hpp>
#include <karkinolution/ticks/worldcycle.hpp>

#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <unistd.h>
#include <functional>


namespace {

bool g_raw_errors = false;

// Absolute path of this very binary, resolved once at startup.
// Needed because /proc/self/exe, read from INSIDE the child shell spawned by
// popen(), points to that shell's own binary, not to smoke -- so we resolve
// it up front, in our own process, and reuse that path for addr2line calls.
std::string g_self_path;

std::string resolve_own_path() {
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1) {
        return "";
    }
    buf[len] = '\0';
    return std::string(buf);
}

// Resolves a stack address to "function at file:line" using addr2line
// against this binary's real path. Works even for functions living in
// anonymous namespaces, which plain backtrace_symbols cannot name.
std::string resolve_with_addr2line(void* addr) {
    if (g_self_path.empty()) {
        return "";
    }

    char cmd[512];
    std::snprintf(cmd, sizeof(cmd),
                  "addr2line -e '%s' -f -C -p %p 2>/dev/null",
                  g_self_path.c_str(), addr);

    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        return "";
    }

    char buf[512] = {0};
    std::string result;
    if (fgets(buf, sizeof(buf), pipe)) {
        result = buf;
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
    }
    pclose(pipe);
    return result;
}

// Prints a readable backtrace: tries addr2line first (function + file:line),
// falls back to the demangled symbol name from backtrace_symbols, and as a
// last resort prints the raw address.
void print_backtrace() {
    constexpr int MAX_FRAMES = 64;
    void* frames[MAX_FRAMES];
    int n = backtrace(frames, MAX_FRAMES);
    char** symbols = backtrace_symbols(frames, n);

    std::cerr << "\n--- backtrace (" << n << " frames) ---\n";

    for (int i = 0; i < n; ++i) {
        std::string resolved = resolve_with_addr2line(frames[i]);

        if (!resolved.empty() && resolved.find("?? ") == std::string::npos) {
            std::cerr << "  #" << i << ": " << resolved << '\n';
            continue;
        }

        std::string line = symbols[i];
        auto paren_open = line.find('(');
        auto plus_pos = line.find('+', paren_open);

        if (paren_open != std::string::npos && plus_pos != std::string::npos) {
            std::string mangled = line.substr(paren_open + 1, plus_pos - paren_open - 1);

            int status = 0;
            char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);

            if (status == 0 && demangled) {
                std::cerr << "  #" << i << ": " << demangled
                          << " " << line.substr(plus_pos) << '\n';
                std::free(demangled);
                continue;
            }
        }

        std::cerr << "  #" << i << ": " << line << '\n';
    }

    std::free(symbols);
    std::cerr << "--- end of backtrace ---\n\n";
}

// Custom terminate handler: runs when an uncaught exception (or a bare
// abort/assert) reaches the top. Prints the exception type + what(), then
// the backtrace, then aborts.
void custom_terminate_handler() {
    std::exception_ptr eptr = std::current_exception();

    if (eptr) {
        try {
            std::rethrow_exception(eptr);
        } catch (const std::exception& e) {
            std::cerr << "\n[UNCAUGHT EXCEPTION] type: std::exception (or derived)\n"
                      << "  what(): " << e.what() << '\n';
        } catch (...) {
            std::cerr << "\n[UNCAUGHT EXCEPTION] unknown type (does not derive from std::exception)\n";
        }
    } else {
        std::cerr << "\n[TERMINATE] called with no active exception (likely a raw abort/assert)\n";
    }

    print_backtrace();
    std::abort();
}

// ---------------------------------------------------------------------------
// The generic runner. Wrap ANY piece of karkinolution code in a lambda and
// hand it to run_snippet() -- it doesn't care what the code does, only
// whether it throws.
// ---------------------------------------------------------------------------

using Snippet = std::function<void()>;

bool run_snippet(const std::string& name, const Snippet& fn) {
    std::cout << "Running: " << name << '\n';

    if (g_raw_errors) {
        // No try/catch here on purpose: if it throws, it propagates all the
        // way up to custom_terminate_handler, uncaught, with a real
        // backtrace.
        fn();
    } else {
        try {
            fn();
        } catch (const std::exception& e) {
            std::cerr << "[FAIL] " << name << " - exception: " << e.what() << '\n';
            return false;
        } catch (...) {
            std::cerr << "[FAIL] " << name << " - unknown exception (does not derive from std::exception)\n";
            return false;
        }
    }

    std::cout << "[PASS] " << name << '\n';
    return true;
}

// Convenience helper for the common case: run N ticks of a World and report
// which tick it died on if it dies. Built on top of run_snippet, so it gets
// the exact same raw/captured behavior for free.
bool run_world_ticks(const std::string& name, World world, int ticks) {
    return run_snippet(name, [&world, ticks]() {
        for (int tick = 0; tick < ticks; ++tick) {
            std::cout << "  tick " << tick << " start\n";
            RunnerWorld::run(world);
            std::cout << "  tick " << tick << " end\n";
        }
    });
}

}

int main(int argc, char** argv) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    g_self_path = resolve_own_path();
    std::set_terminate(custom_terminate_handler);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--raw" || arg == "-r") {
            g_raw_errors = true;
        }
    }

    std::cout << "Error mode: "
              << (g_raw_errors ? "RAW (crashes loud, shows type/what()/backtrace, aborts)"
                                : "CAPTURED (prints and moves on to the next snippet)")
              << "\n\n";

    bool success = true;

    // -----------------------------------------------------------------
    // Standard world smoke tests. Feel free to keep, remove, or tweak these.
    // -----------------------------------------------------------------

    constexpr int TICKS = 20;

    success &= run_world_ticks("Crab chaos world", WorldFactory::create_crab_chaos(), TICKS);
    success &= run_world_ticks("Paranoic world", WorldFactory::create_paranoic(), TICKS);
    success &= run_world_ticks("Titanic world", WorldFactory::create_titanic(), TICKS);
    success &= run_world_ticks("Normal world", WorldFactory::create_normal(), TICKS);

    // -----------------------------------------------------------------
    // PLAYGROUND: drop any karkinolution code here. Each block is fully
    // independent and runs under the exact same raw/captured logic as
    // everything else above. Add as many as you want.
    //
    // Example:
    //
    // success &= run_snippet("my custom test", []() {
    //     World world = WorldFactory::create_normal();
    //     RunnerWorld::run(world);
    //     // ... any karkinolution call you want to poke at ...
    // });
    // -----------------------------------------------------------------

    return success ? 0 : 1;
}