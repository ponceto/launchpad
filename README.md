# Launchpad

## What is it ?

This project is designed to experiment with the `Novation Launchpad Mini` [MIDI](https://en.wikipedia.org/wiki/MIDI) protocol.

This project is based on the excellent cross-platform [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library and is written in C++14 for Linux, but it should compile and run on BSD without any change.

## Supported models

The project has only been tested with the `Launchpad Mini MK2` but should work with the original `Launchpad` and `Launchpad Mini MK1`.

![Launchpad Mini MK2](doc/Novation_Launchpad_Mini_MK2.png)

## Disclaimer

This project is absolutely not affiliated with [Novation](https://novationmusic.com/) or [Focusrite](https://focusrite.com/).

It has been developed according to the programmer reference made available by Novation. You can find a copy of this reference in the `doc` directory.

## License

This project is availaible under [GNU General Public License, version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).

```
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

## How to build and install

### Preamble

This project is fully [autotoolized](https://en.wikipedia.org/wiki/GNU_Autotools), so it is simple to build and install it with the traditional commands `configure`, `make` and `make install`.

### Install the dependencies

Under Debian or derivatives (Ubuntu, Mint, ...), you should install these dependencies. On the other distros, please install the equivalent packages.

Build tools:

```
build-essential
```

Build dependencies:

```
librtmidi-dev >= 3.0.0
```

### Generate the configure script

If the `configure` script is not present, you must generate it. This the case if you are cloning the git repository.

Before generating the `configure` script, you must ensure you have the following packages installed on your system:

```
autoconf
automake
libtool
```

If everything is installed, you just have to run this command:

```
autoreconf -v -i -f
```

And voilà ! The configure script is now available and usable.

### Configure the project

Run the `configure` script

```
./configure --prefix={destination-path}
```

The prefix is used to inform the build system where to install the project.

### Build the project

If the source configuration has done without any error, you can now build the project with `make`:

```
make
```

You can build in parallel if you have a multiple cpu cores on your machine: 

```
make -j${NUMBER_OF_PROCESSES}
```

Ajust the `NUMBER_OF_PROCESSES` with this rule of thumb: `NUMBER_OF_PROCESSES = NUMBER_OF_CPU + 1`.

Example for a 4 cores CPU:

```
make -j5
```

### Install the project

This step is optional and can require `root` permissions.

```
make install
```

If everything is ok, the project is now installed on your system.

You can find the executable into the `${destination-path}/bin` directory.

## How to run

### Preamble

If you have installed the project, you can find the executable in the `${destination-path}/bin` directory. In the other case, the executable is located in the `./src/launchpad` directory

### Usage

```
Usage: launchpad [options] <command> [<args>]

Commands:

    help [{command}]                    display help
    list                                list available MIDI ports
    reset                               reset the Launchpad
    cycle                               cycle colors
    print {message}                     print a message
    scroll {message}                    scroll a message
    matrix                              matrix-like rain effect
    gameoflife [{pattern}]              display the game of life

Options:

    -h, --help                          display this help
    -l, --list                          list available MIDI ports

    --delay={value[us|ms|s|m]}          delay (ms by default)

    --midi={port}                       MIDI input/output
    --midi-input={port}                 MIDI input
    --midi-output={port}                MIDI output

MIDI input/output:

    [I] Launchpad Mini
    [O] Launchpad Mini

    +---------------------------------------+
    |  1   2   3   4   5   6   7   8        |
    | ( ) ( ) ( ) ( ) ( ) ( ) ( ) ( )       |
    |                                       |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) A |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) B |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) C |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) D |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) E |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) F |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) G |
    | [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] ( ) H |
    |                                       |
    | Novation               Launchpad MINI |
    +---------------------------------------+

```

### Select the MIDI device

By default, the utility will use the string `Launchpad Mini` to find a usable Launchpad.

But you can select a specific MIDI peripheral by using `--midi`, `--midi-input`, `--midi-output` options.

The utility will use each provided string like a prefix to compare with all available inputs/outputs and will choose the first match.

Example:

```
launchpad --midi="Launchpad Mini:Launchpad Mini MIDI 1"
```

You can list all MIDI peripherals with this the `list` command or the `--list` or `-l` options.

## Available commands

### help

You can display the help with this command:

```
Usage: launchpad [options] help [{command}]

Display help information about the program or a command

Arguments:

    command             specifies the command

```

### list

You can list all MIDI peripherals with this command:

```
Usage: launchpad [options] list

List all available MIDI ports

Arguments:

    none

```

Outputs example:

```
Available MIDI inputs :

[I] Midi Through:Midi Through Port-0 14:0
[I] Launchpad Mini:Launchpad Mini MIDI 1 20:0

Available MIDI outputs :

[O] Midi Through:Midi Through Port-0 14:0
[O] Launchpad Mini:Launchpad Mini MIDI 1 20:0

```

### reset

You can reset the device with this command:

```
Usage: launchpad [options] reset

Reset the Launchpad

Arguments:

    none

```

### cycle

You can cycle all colors with this command:

```
Usage: launchpad [options] cycle

Display a color cycle on the Launchpad

Arguments:

    none

```

### print

You can print a message with this command:

```
Usage: launchpad [options] print {message}

Print a message on the Launchpad

Arguments:

    message             specifies the message to print

```

### scroll

You can scroll a message with this command:

```
Usage: launchpad [options] scroll {message}

Scroll a message on the Launchpad

Arguments:

    message             specifies the message to scroll

```

### matrix

You can display a Matrix-like rain effect with this command:

```
Usage: launchpad [options] matrix

Matrix-like rain effect

Arguments:

    none

```

### gameoflife

You can display the [Conway Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) with this command:

```
Usage: launchpad [options] gameoflife {pattern}

Display a Game of Life on the Launchpad

Arguments:

    pattern             specifies the pattern to simulate:

                        'random' display a random pattern
                        'glider' display a glider pattern

```
