#pragma once

#include <array>
#include <random>
#include <string>
#include <string_view>

namespace prank {

inline constexpr std::array<std::string_view, 40> reasons{{
    "Press this for $1,000.",
    "Press this, please.",
    "Free cookies. One click away.",
    "Click to unlock infinite wisdom.",
    "A completely normal red button.",
    "Click for a free graphics card.",
    "This button knows your future.",
    "Congratulations! You won something.",
    "Click to double your RAM.",
    "Your prize is waiting.",
    "Press for instant happiness.",
    "This is definitely not a prank.",
    "One click = one free pizza.",
    "An important scientific experiment.",
    "Press to become 12% cooler.",
    "Do not question the button.",
    "Click to fix all your problems.",
    "This button is government approved.",
    "Press this for absolutely no reason.",
    "The button specifically asked for you.",
    "Click to install more FPS.",
    "Press to claim your new yacht.",
    "You can trust a big red button.",
    "Click to reveal a huge secret.",
    "A stranger on the internet said to.",
    "Press for a lifetime supply of air.",
    "This will look great on your resume.",
    "Click to delete Monday.",
    "Become a millionaire in one click.",
    "Please press me. I am lonely.",
    "There's definitely a reward.",
    "Click for a free submarine.",
    "Press here for unlimited luck.",
    "A once-in-a-lifetime opportunity.",
    "The button has excellent reviews.",
    "Press to achieve enlightenment.",
    "Your computer wants you to click.",
    "Click to download more internet.",
    "Press this to see something amazing.",
    "No catch. No tricks. Just click.",
    "click to download free ram (plz clik)"
    "Seriously. Click for cookie, fatty."
    "Click or you die tmr frfr"
    "If no click then no gift ;)"
    "click for rtx 5090 ti super waifu edition"
    "Click for girlfren (or boyfren)"
    "Click or monster come eat you and your cookies >:)"
    "1 click = 1 praise to creator of me"
    "such red button, such nice"
    "many cookie such nice button"
    "woof"
    "click or you get poopy diaper"
    "BRO PLEASE CLICK TO FREE ME FROM CONFINEMENT"
    "click or scp 173 snap your neck"
    "Click if you want christmas early"
    "Click and you get 2 birthdays in a year"
    "Having fun is realizing clicking the button is a good choice"
    "Button click = gta 9 release TODAY"
    "Click for boeing 737 max"
    "Click to reverse debt"
    "Click to evade taxes (successfully)"
    "Click to silence crying babies on airplanes mafia style"
    "imagine this red button is a scary spider. smash it."
    "im tired, wake me up i got work in the morning"
    "Only losers dont click."
    "Remember that one image of the miner walking away from the jackpot? thats you if you dont click."
    "*sniff* discord mod detected. click for free anime waifu (age: fresh)"
}};

inline constexpr int window_width = 450;
inline constexpr int window_height = 260;
inline constexpr int punchline_ms = 1400;
// A real null-terminated array: backends hand this straight to C APIs.
inline constexpr char punchline[] = "you are an idiot";

inline std::string random_reason() {
    static std::random_device device;
    static std::mt19937 engine(device());
    std::uniform_int_distribution<std::size_t> pick(0, reasons.size() - 1);
    return std::string(reasons[pick(engine)]);
}

} // namespace prank
