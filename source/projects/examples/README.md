ChucK Class Library Reference
    https://chuck.stanford.edu/doc/reference/
    https://chuck.cs.princeton.edu/doc/reference/

examples organized by topic
    https://chuck.stanford.edu/doc/examples/
    https://chuck.cs.princeton.edu/doc/examples/

ChucK Documentation (tutorials, language specification, etc.)
    https://chuck.stanford.edu/doc/
    https://chuck.cs.princeton.edu/doc/

Join the Community (seek/give help; share work)
    https://chuck.stanford.edu/community/
    https://chuck.cs.princeton.edu/community/

Many other things ChucK
    https://chuck.stanford.edu/
    https://chuck.cs.princeton.edu/


For `chuck-max`, the following changes was made to the chuck examples directory:

- removed
    
    - `book`, due to its deep directory structure.

- added:
    
    - `max`, for max specific chuck files

    - `test`, for tests related to `chuck-max`

    - `util`, for misc utilities such as `help.ck` and `status.ck`

    - `chugins`, for additional package-level chugins

    - `data`, added `amen.wav`
    
    - `midid/data`, added `africa.mid`, and `yiruma.mid`
    
    - `stk/honkeytonk-algo3.ck`
    
    - `tests` folder with external specific tests.
    
- changed:

    - `effects/autotune.ck` to generout output file in `/tmp/` instead of locally

```c++
{
    dac => WvOut2 record => blackhole;
    // output file name
    //"./autotuned-obama.wav" => string outfile;
    "/tmp/autotuned-obama.wav" => string outfile;
    "./autotuned-obama.wav" => string outfile;
    // print
    cherr <= "recording to file: '" <= outfile <= "'..." <= IO.nl();
    // set output file name
    //me.dir() + outfile => record.wavFilename;
    outfile => record.wavFilename;
    me.dir() + outfile => record.wavFilename;
}
```

