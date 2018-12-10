## Summary
This package contains sample code that shows how to use the [SHORE library](https://www.iis.fraunhofer.de/shore).

Please note that this is a supplementary addon package for the SHORE library for realtime face detection. 
The SHORE library is not included in this  project - please [contact us](mailto:facedetection@iis.fraunhofer.de) for a evaluation license. 
We do not offer any official support for this package. See the LICENSE file for details.

## Building
The following packages must be installed before building: ```cmake, libboost-dev, libjpeg``` 
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DSHORE_SDK_PATH=${YOUR_SHORE_SDK_PATH} -DSHORE_VERSION=161 ../
cmake --build . --target install -- -j 4
```

### Troubleshooting
#### Protected SHORE library
If you are using a protected version of the SHORE library you may get errors like the following 
if there are any licensing problems:
```
Sentinel LDK Protection System - Feature not found (32)
```

Make sure you have plugged in the provided USB key or installed the 
Sentinel Runtime package and activated the license.

Please contact [facedetection@iis.fraunhofer.de](mailto:facedetection@iis.fraunhofer.de) to get support.

#### Stub library (Linux only)
If you are using a protected version of the SHORE library you have to link against the provided
*stub*-library. An application that is linked against the *stub*-library will generate the following output at run time:
```
ERROR: This is the Shore library stub that is required during linking. Replace it by the real Shore library before running the application.
```

After executing the installation step (```make install``` ) the "real" SHORE library is copied into the INSTALL folder.
The RPATH-property of the executable is set up in a way that the linker will find the SHORE library in the respective folder.


### Contributions
Please note that we only accept pull requests that are licensed under the MIT License.

