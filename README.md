# Uranium

Current version is v1.0.3

Transfer Files or Directories Between Linux and Windows Without SSH Support

## Requirements
### Running

- linux(ubuntu)  linux_release
   tar -axvf linux_release.tar.bz2 
   cd linux_release
   source .env
   ./server_tester username password 
- windows        release
   tar -axvf release.tar.bz2

### Building

- cygwin
    * make clean; make clean-cache
    * make -j8
- linux(ubuntu)
    * make clean; make clean-cache;
    * make -j8; make linux_release

## Download
From [here](https://github.com/NiklasWang/Uranium/releases/)
