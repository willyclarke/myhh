# My Handmade Hero attempt number 2

## In brief

The Handmade here project is a fantastic project by Casey Muratori.

Check it out on 'https://handmadehero.org'

As part of my learning process I will type in the code that is relevant for the platform layer in this repo.
The actual code from the HH project will not be checked in. You can buy that from the HandmadeHero project page.

As a starter on OSX I will utilize the code from https://github.com/BeagleJoe/SDL2-minimal.git
Many thanks to BeagleJoe for providing this as a starting point.

## Building the code

With Ninja build:

```
mkdir build
cd ./build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -GNinja
ninja
```

Just skip the -GNinja if Ninja build is not available.
