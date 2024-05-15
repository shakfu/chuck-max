# chugins directory

> A chugin (or ChuGin) is a plug-in for the ChucK audio programming language. Chugins are loaded at runtime by ChucK and are fundmental to extending the capabilities of the language by adding class libraries and audio Unit Generators (UGens) and Unit Analyzers (UAnae). 

The `chuck~` external's functionality can be extended including chugins in this directory.


- Start here for a site to learn about the general topic of [extending ChucK](https://chuck.cs.princeton.edu/extend/).


- The base set of included chugins are the [CCRMA chugins](https://github.com/ccrma/chugins) which is also a good place to look at chugin code.


- A more complex example of a chugin is the [Fauck](https://github.com/ccrma/fauck) chugin:

> FaucK is a Chugin that combines the powerful, succinct Functional AUdio STream (Faust) language with the strongly-timed ChucK audio programming language. FaucK allows programmers to evaluate Faust code on-the-fly inside ChucK and control Faust signal processors using ChucK’s sample-precise timing and concurrency mechanisms.

- Another advanced included chugin is the [WarpBuf](https://github.com/ccrma/chugins/tree/main/WarpBuf) chugin: 

> With WarpBuf you can time-stretch and independently transpose the pitch of an audio file. The supported formats include wav, flac, mp3, ogg, opus, and vorbis. 

