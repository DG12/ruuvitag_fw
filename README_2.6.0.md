Based on version 2.5.9.<p>
Minimun number of source files changes<p>
<li>Add tunables to main
<li>Revised NFC report added tunable information in main. (DATA: was improperly coded)
<li>Added app_error_fault_handler log message and blink 13 times.
<li>Transmit configuration packet ( x'C0') after initalization which includes tunable final values.
 <br>
   Example: C0 20 42 00 00 05 00 05 00 00 00 D6 D6 D6 D6
          
See <a href=http://MyBeacons.info/tunables.html>MyBeacons.info/tunables</a> for details.
