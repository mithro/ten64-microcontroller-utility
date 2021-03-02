# Usage

## lpc804cli --set-mac <mac>

Set the mac address. Expects a string of length 12.

## lpc804cli --get-info

Get uC info including CPU id, mac address and firmware version.

## lpc804cli --get-state

Get the system control state including whether powered by ATX, external reset button enabled, external reset button hold time, boot source and dip switch to indicate whether reset triggers are ignored.

## lpc804cli --set-reset-holdtime <time {3:10}>

Set the external reset button hold time in seconds. Range is 3 to 10 seconds.

## lpc804cli --set-reset-enabled <0|1>

Enable or disable the external reset button function.


