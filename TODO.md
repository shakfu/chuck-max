# TODO

- [ ] Fix Faust cleanup bug

- [ ] Add Windows Support

- [ ] Consistent use of `ck_check_file` for `ck_add`, `ck_replace`, etc..

- [ ] add test for chuck file args (conventional style)

- [ ] Convert args part of `<filename>:arg1:arg2:argN` to args as atom list, so that

  ```text
  add stk/honkeytonk-algo1.ck:10:32.1:sample.wav

  becomes

  add stk/honkeytonk-algo1.ck 10 33.1 sample.wav
  ```

- [ ] Better examples using Max message for global variables

- [x] Add editor support code editor: double-click to edit, etc.. or via filewatcher

- [ ] Package externals, chugins, scripts and patchers in a self-contained signed and notarized Max package

- ...
