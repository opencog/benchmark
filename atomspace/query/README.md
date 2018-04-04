## Profiling ##

The benchmark directory now includes a sample profiling for the
bindlink function in `profile_bindlink.cc` This file can be used as a
template for profiling other atomspace functions.

### Using perf_events ###
Install:
```
apt-get install linux-tools
```

oprofile


### Using valgrind ###
Build as normal, then use

```
valgrind --tool=callgrind ./opencog/benchmark/profile_bindlink
```
after this completes:
```
kcachegrind callgrind.out.<pid>
```

### Using gprof ###
To use the `gprof` profiler, you will need to build a profile build
of AtomSpace.

The following commands will create a new profile directory and build
the atomspace with the compiler options so that gprof will output the
appropriate profiling function hooks. It will also make the libraries
static and not shared which is required for gprof to profile functions
in the libraries.

From the top level atomspace directory, i.e. the one that already
contains the build directory, execute:

```
mkdir profile
cd profile
cmake -D CMAKE_BUILD_TYPE=Profile ..
make
```

This will build atomspace using the profile options. After the build
finishes there will be a program `profile_bindlink` in the directory:

```
opencog/benchmark/
```

within the profile build directory. Running:

```
./profile_bindlink
```

from that directory will run the executable and after few seconds it
will exit and generate a file:

```
gmon.out
```

Now execute the following command:

```
gprof profile_bindlink gmon.out > analysis.txt
```

This will take the binary profiling information in gmon.out and format
it so that you can read it in a file called `analysis.txt`. Open
`analysis.txt` in a text viewer and you will see the results of the
profiling.
