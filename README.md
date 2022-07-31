# spotykach
quirky sound machine
VST3 Mac

https://www.instagram.com/p/CfbAkIvvhju/

### Build VST

- open spotykach.xcworkspace
- the `spotykach_vst` project assumes `vst3sdk` folder is located at the same level with worksapce folder, i.e.:
```
__
  |-spotykach
  |-vst3sdk
  |-....
```
- in file [vst/CMakeLists.txt](https://github.com/vlad-litvinenko/spotykach/blob/main/vst/CMakeLists.txt) batch-replace occurences of `/Users/vlly/DSP/` with your path
- in file [vst/build/CMakeCache.txt](https://github.com/vlad-litvinenko/spotykach/blob/main/vst/build/CMakeCache.txt) batch-replace occurences of `/Users/vlly/DSP/` with your path
- build `spotykach_core` target in `spotykach_core` project first as it contains plugin logic
- build `spotykach_vst` target in `spotykach_vst` project
- In case build complanes `.../vst/build/CMakeFiles/spotykach_vst.dir/Info.plist` rebuild `spotykach_vst` again.
