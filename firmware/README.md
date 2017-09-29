
Howdy y'all,

The GoodWatch firmware is in this directory.  You will find user-level
applications in the `apps/` subdirectory, while `*.c` in the root
directory contain low-level drivers.

To add your own application, for one of the smaller examples, such as
`stopwatch.c`.  Your app must provide functions for `_init()` (to run
at startup), `_draw()` (to run every quarter-second for rendering),
and `_exit()` which should just return 0 unless you need to intercept
the Mode button, in which case it may return 1 to delay the exit.  Add
these three functions to a line of the `apps[]` structure in
`applist.h`.

In general, we try not to over-abstract the hardware because we don't
intend the design to be portable away from the CC430.  Include
`<msp430.h>` if you need to access the CC430's IO registers or
`"api.h"` for all the standard functions of the GoodWatch firmware.

Cheers,

--Travis
