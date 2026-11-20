# big red button

a window, a giant red button, and some bullshit excuse to press it (free cookies, double your ram, the button asked nicely). what happens after you click is between you and the button, i'm not ruining it for you, you filthy little masochist. that's the whole goddamn program: no internet, no telemetry, no threads, no background shit, just your dumbass and a button that's waiting for you to slip.

needs cmake 3.16+ and c++17. `cmake -S . -B build && cmake --build build`, then run whatever fell out (`.exe` on windows, `open build/redbutton.app` on mac, `./build/redbutton` everywhere else). unix people add `-DBUTTON_BACKEND=gtk3` for wayland or `=x11` if they hate themselves, auto prefers gtk3. backends are win32, cocoa, gtk3, x11, haiku, only x11 was actually tested so the rest is pray-and-ship.

the messages and timing are in `src/reasons.hpp` if you must spoil it for yourself. mit license, do whatever the fuck you want.
