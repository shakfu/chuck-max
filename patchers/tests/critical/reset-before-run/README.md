# Reset-Before-Run crash

This is an unresolved crash scenario that occurs if a `clear vm` or `reset` message is sent to `chuck~` before running a chuck file which contains a custom global event.

The description and ongoing discussion of this issue can be found on [github](https://github.com/shakfu/chuck-max/issues/11).

It can be prevented by switching on the `run_needs_audio` attribute.
