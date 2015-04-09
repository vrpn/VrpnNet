# VrpnNet
.NET bindings for VRPN - Changes beyond the last upstream release

###The projects are as follows:
VrpnNet - The .Net library for using VRPN.  
PrintVals - A C# console program example of how to read values from a VRPN tracker device and a button device.  
ImagerServerSample - A C# example program of how to transmit images using the VRPN imager through VrpnNet.  
ImagerRemoteSample - A C# example program showing how to read images from a VRPN imager device.  

###Known Bugs:
The VRPN imager does not work with 32-bits per channel floating point images.

###XML Documentation Hints:
VrpnNet is configured to build xml documentation that will work with the Object Browser in Visual Studio.  Currently, only the imager classes are well documented.  There are a couple of gotchas to be aware of when building this documentation:

0. The documentation compiler will attempt to including any comment that starts with /// or /\*\*, including those in the static linked vrpn libraries.  To eliminate invalid documentation from the vrpn libraries they need to have all /// and /\*\* comments removed.  To do this, open all the headers in Visual Studio and do a find and replace using regular expressions for the following commands (without the quotes):
    - Find "///~(/)" and replace with "//"  
    - Find "/\\\*\\\*~(\\\*)" and replace with "/\*"  
0. The documentation compiler incorrectly generates the tags for nullable parameters.  Open the generated VrpnNet.xml file and find "Nullable`1" and replace it with "Nullable" (no quotes).
0. The compilation of the documentation sometimes fails with a file not accessible warning.  If this happens, clean the solution and rebuild.
