## Kitra520 in application programmer

This is the firmware upgrade utility source code.
Pre-built firmware for Artik 5 is provieded in the Kitra520 main repository.

Check kitra520 wiki about to see how to use it.

To compile the project run

```
arm-linux-gnueabi-g++ -o out *.cpp
```

You need g++-arm-linux-gnueabihf toolchain installed.

Alternativelly you can also build on Artik:

```
g++ -o out *.cpp
```

on Artik.
