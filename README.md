RCM logdump
=
A quick and dirty dumper of IRAM to a log file on the sd card. Logs should be placed from ``0x40038000-0x4003D000`` for a maximum size of ``0x5000`` (20KiB).

#define IRAM_SAFE_START 0x40038000ull
#define IRAM_SAFE_END 0x4003D000ull

Based on [Hekate](https://github.com/CTCaer/hekate) and [Lockpick_RCM](https://github.com/shchmue/Lockpick_RCM).