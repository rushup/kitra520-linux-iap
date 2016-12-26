## Kitra520 in application programmer

This is the firmware upgrade utility source code.
Pre-built firmware for Artik 5 is provided in the Kitra520 main repository.

Check kitra520 wiki to see how to use it.

To compile the project run

```
arm-linux-gnueabihf-g++ -o out *.cpp
```

You need g++-arm-linux-gnueabihf toolchain installed.

Alternativelly you can also build on Artik:

```
g++ -o out *.cpp
```
