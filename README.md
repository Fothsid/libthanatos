# libthanatos

libthanatos is a simple C++ library for reading and writing Resident Evil Outbreak and Resident Evil Outbreak File 2 3D models.

## Important information
* tl;dr: Please do not rely on this library
* libthanatos heavily relies on the C++ STL, so be careful regarding that.
* libthanatos was only tested on GCC and MSVC.
* libthanatos isn't very documented and is not planned to be documented in the near future.
* libthanatos could crash the program if something in the NBD is wrong, I haven't really made it error-prone that much :c
* libthanatos outputs error and warning messages to `stderr` by default, define `OB_ERROR_OUTPUT` before including `thanatos.h` in order to change that.
* Error and warning message output is not very consistent.
* This library *does not* automatically convert triangle lists to triangle strips and vice-versa. Do that yourself.
* This library *does not* automatically convert TIM2 textures to a different format and vice-versa. Once again, do that yourself. [OBTool](https://github.com/Fothsid/OBTool) has TIM2 [handling](https://github.com/Fothsid/OBTool/blob/main/src/tim2.c) and [conversion](https://github.com/Fothsid/OBTool/blob/main/src/tim2utils.c) code. Feel free to use it.

## Example usage

```cpp
#include <cstdio>
#include <fstream>
#include "thanatos.h"

int main(int argc, char* argv[])
{
    std::ifstream nbdStream("test.nbd", std::ios_base::binary); // Make sure it's in std::ios_base::binary mode
    OBNbd nbd;
    if (!nbd.read(nbdStream))
    {
        fprintf(stderr, "Error parsing the NBD file\n");
        return 0;
    }
    printf("Bone count: %d\n", nbd.model.hierarchy->nodeList.size());
}

```
