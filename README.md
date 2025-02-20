# Cyg-Git

A proxy between Cygwin's `git` command and VSCode.

- put `cyggit.exe` to somewhere, I will use `C:\cygwin64\local\bin\cyggit.exe`
- modify it on VSCode's `settings.json`
```json
{
  "git.path": "C:\\cygwin64\\local\\bin\\cyggit.exe"
}
```
- All done!

## build

use MinGW64 to build it, download on <https://github.com/niXman/mingw-builds-binaries/releases/tag/14.2.0-rt_v12-rev1>,
uses `x86_64-14.2.0-release-win32-seh-msvcrt-rt_v12-rev1.7z`

build with it: `x86_64-w64-mingw32-gcc -o cyggit.exe cyg-git.c` 

*As your whises, I have put one built binary right there.*

## Thanks

thanks for origin auther to make a zsh version of git-wrapper.
thanks for ChatGPT, he gave a lots of infomation to enable me to rewrite the whole wrapper with C language.

That's all!
