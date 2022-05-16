# My Handmade Hero attempt number 2

## In brief

The Handmade Hero project is a fantastic project by Casey Muratori.

Check it out on 'https://handmadehero.org'

As part of my learning process I will type in the code that is relevant for the platform layer in this repo.
The actual code from the HH project will not be checked in. You can buy that from the HandmadeHero project page.

As a starter on OSX I will utilize the code from https://github.com/BeagleJoe/SDL2-minimal.git
Many thanks to BeagleJoe for providing this as a starting point.

## Dependencies

The platform layer utilize Simple DirectMedia Layer 2 for the platform layer. (https://www.libsdl.org/index.php).

Although it is not really in the spirit of Casey to use build tools this project use CMake for generation of build files.

## Building the code

With Ninja build:

```
mkdir build
cd ./build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -GNinja
ninja
```

Or when using GNU make:

```
mkdir build
cd ./build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make -j9
```
