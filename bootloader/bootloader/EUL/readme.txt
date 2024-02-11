Omega UEFI Loader

Omega UEFI Loader (OUL in short) is the loader of Omega OS when 
it's running on the UEFI environment. The same as Omega OS, OUL is
also a free software, it's released under the terms of GNU GPL v3.
See COPYING for more details.

To build it, you have to download EDK2 project at <https://github.com/tianocore/edk2/>
and set up a UEFI environment, then copy ./OULPkg to the 
/path/to/your/edk2/project/edk2/OULPkg and run build.

To simplify the build, this archive provides the pre-built binary at
/path/to/Omega-OS-x.y/prebuilt/bootx64.efi, just copy it to the
/path/to/Omega-OS-x.y/build (it'll be automatically generated while 
building the source code).
