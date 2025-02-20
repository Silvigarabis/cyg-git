# **Cyg-Git**  

A proxy between Cygwin's `git` command and VSCode.  

## **Installation**  

1. Place `cyggit.exe` somewhere on your system, e.g., `C:\cygwin64\local\bin\cyggit.exe`.  
2. Modify VSCode's `settings.json`:  

   ```json
   {
     "git.path": "C:\\cygwin64\\local\\bin\\cyggit.exe"
   }
   ```

3. Done! Now VSCode will use `cyggit.exe` as the Git executable.  

## How It Works

    This program directly calls `git.exe` and `cygpath`, assuming both are available in PATH.
    cygpath is used to convert Windows-style paths to Unix-style paths for Cygwin's git.
    If git or cygpath is missing, the program will fail to execute properly.

## **Build**  

You need **MinGW-w64** to build this project.  
Download it from [MinGW Builds](https://github.com/niXman/mingw-builds-binaries/releases/tag/14.2.0-rt_v12-rev1).  
The recommended version:  

- `x86_64-14.2.0-release-win32-seh-msvcrt-rt_v12-rev1.7z`  

To compile:  

```sh
x86_64-w64-mingw32-gcc -o cyggit.exe cyg-git.c
```

*A pre-built binary is also included, in case you don't want to compile it yourself.*  

## **Acknowledgments**  

- Thanks to the original author (@) for creating a **Zsh-based Git wrapper**.  
- Thanks to **ChatGPT** for providing useful information that helped rewrite the entire wrapper in C.  

That's all! 🚀  
