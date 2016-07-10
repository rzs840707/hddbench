# hddbench
A cross-platform GUI application to benchmark storage devices

<img src="https://raw.githubusercontent.com/haikarainen/hddbench/master/screenshot.png" alt="Screenshot of application">


This application benchmarks the read and write speeds of storage devices. It aims to benchmark practical & realistic speeds, and does not attempt to optimize anything in terms of increasing speeds. Therefore, it should be an accurate reflection of how most applications work in practice.

## Notice

This application is in a very early stage. There is a current suspicion that, because of how the application currently behaves, physical storage device controllers may cache the write-test, and then send that cache back in the read-test instead of actual reads. There are plans to implement options to work-around these optimizations, to give more accurate results.

This has not yet been confirmed though, as the read differences are slight, and for example not at all that of an OS RAM cache optimization. Additionally, the high read speeds I've encountered has happened even with very large chunk sizes, on both my SSD and my HDD.

Feel free to contribute.
