# chuck-max

Ongoing attempt to embed [chuck](https://chuck.stanford.edu) in a Max/Msp external.



## Setup

Note: `make setup` does the following:

```bash
ln -s $(shell pwd) "$(HOME)/Documents/Max 8/Packages/$(shell basename `pwd`)"
```

## Usage

Open the help file for a demo.


## Status

- [ ] improve interface
- [ ] fix sound input!
- [x] fix sound output! Thanks [DBraund](https://github.com/DBraun)
- [x] initial attempt to embed (no sound yet) without compilation or max/msp errors
- [x] compiled `chuck~` with libchuck.a without errors
- [x] created libchuck.a
- [x] converted chuck makefile to CMAKE


## Credits

I was originally persuaded to try to get Brad Garton's original `chuck~` external for Max 5 working (which is included in the `source/legacy` folder for reference) but I gave up since both the Max and Chuck apis had moved on quite a bit since.

Then I attempted to create a new external against the latest Max and Chuck apis in the still non-functional [pd-chuck](https://github.com/shakfu/pd-chuck) project.

In this last attempt, everything worked except audio: there was something challenging in getting Chuck to output data in the pd perform function which I couldn't solve. I asked for help on chuck's github issue tracker and received none. I asked for help in the electro-music forum and received only one post which was appreciated but none too helpful. So, I left it for another day.

I'm not sure why I tried again with Max/MSP but it turned out to be good idea, because this time I didn't give up. This time, I searched Github, and this time, I found David Braun's excellent[ChucKDesigner](https://github.com/DBraun/ChucKDesigner) project which embeds chuck in a Touch Designer plugin.

David is a [CCRMA](https://github.com/CCRMA) (the home of chuck) alum and the author of the fantastic [DawDreamer](https://github.com/DBraun/DawDreamer) project which I am already a great fan of... and in this case, he was solving the same problem for Touch Designer that I was trying to solve for pd and Max/MSP, so I was quite confident that I would get closer to a solution to my audio issue... after looking for the `chuck->run` call I found the following nugget:

```c++
// chuck->run(inbuffer, *output->channels, output->numSamples); // this doesn't work because of interleaved samples.
// Chuck returns LRLRLRLR but for touchdesigner we want LLLLRRRR.
// Therefore we must use an intermediate buffer
```

And then I read his code as his solved this problem.

This followed by a big AHA that it was likely that PD and Max/MSP, like TouchDesigner, require non-interleaved samples, so after about half an hour of retrofitting David's solution to this project's `perform64` function, I compiled and loaded the external in Max and it worked perfectly the first time!

So, basically, this project owes a big thanks to David Braun!

