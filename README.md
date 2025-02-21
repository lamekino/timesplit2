# *TimeSplit2* - Split large audio into individual tracks
--------------------------------------------------------------------------------

TimeSplit2 is a program which takes a large audio source (mp3 file, ogg, flac,
etc) and copies sections into their own files. You can take a list of songs
like:

```
stdin:
0:00 Song 1
2:00 Song 2
3:30 Song 3
6:30 Song 4
```

With a "Songs.mp3" (8 minutes) and you will get:
"Song 1.mp3" (2 minutes),
"Song 2.mp3" (1 minute 30 seconds),
"Song 3.mp3" (3 minutes),
"Song 4.mp3 (1 minute 30 seconds)

## Usage
--------------------------------------------------------------------------------

`./timesplit2 [flags] [audio-source]`

### Flags:
* -h / --help : view help
* -j / --jobs : set thread count
* -o / --output : set output directory
* -i / --timestamps : use a file

## Build process
--------------------------------------------------------------------------------

```sh
$ make # build the executable
$ make install # local install (~/.local)
$ make && sudo PREFIX=/usr/local make install # system install (/usr/local)
$ make clean # remove build files
$ make uninstall # remove the executable from $PREFIX
```

--------------------------------------------------------------------------------
*No LLM generated code was used in the creation of this project.*
