# ACS ADSDriver
This library works as an intermediary between the ACS Hardware Device environment and a TwinCAT development environment through the ADS protocol.
The repository consists of 2 packages:

**ADSInterface:** communication interface, here the ambcommands that are generated in the HWDevice are packaged and sent and the responses are received. To compile it is required to install the Beckhoff ADS library. It is similar to ADS Devio but packaged as a singleton, with this all ADS connections are concentrated in a single implementation

**GenHWDevice:** Code generator with new templates for driver integration (It works similarly to the simulator and does not include new dependencies).

## Compile ADSInterface
 For this class, it is required to install the Beckhoff ADS library

### Ads Beckhoff library 
Official ADS library repository https://github.com/Beckhoff/ADS
#### dependencies 
- meson
- ninja
#### clone the repository
> git clone https://github.com/Beckhoff/ADS.git
> cd ADS
> git checkout 4e480d105f0a6e20b42f7f4e4593836305317566

#### configure meson to build the library
Edit meson.build and replace:
> project('AdsLib', 'cpp',
  version : '0.1',
  default_options : [
	    'buildtype=release',
	    'warning_level=3',
	    'werror=true',
	    'b_pie=true',
  ]
)

to

> project('AdsLib', 'cpp',
  version : '0.1',
  default_options : [
    'cpp_std=c++11',
    'buildtype=release',
    'warning_level=3',
    'werror=false',
    'b_pie=true',
  ]
)

and  
> adslib = static_library('AdsLib',
  [common_files, router_files],
  include_directories: inc,
)

to
> adslib = shared_library('AdsLib',
  [common_files, router_files],
  include_directories: inc,
)
#### Build library
> meson build
> ninja -C build

If the build fails, edit the file AdsLib/RouterAccess.cpp and replace line 66
> SearchPciBusResNew res {};

by
> SearchPciBusResNew res = SearchPciBusResNew();

Finally, install the library where appropriate and go to ADSInterface/src and compile.
> cd ADSInterface/src
> make clean all install

## Compile GenHWDevice
go to GenHWDevice/src and compile.
> cd GenHWDevice/src
> make clean all install

## Hardware Device Component
Generate HWDevice code with the changes added in the new GenHWDevice
> cd Device/HardwareDevice/LORR/src
> make gen

Compile component
> make clean all install

## Configure CDB files
For the XML with the component instance in the CDB, 3 new configuration attributes were added (required) which must be configured:

**remoteip:** TwinCAT server ip

**adsport:**  ADS port of the TwinCAT project (default 866)

**amsnetid:** AMSNetId of the Twincat server (192.168.56.1.1.1)

For example for the CONTROL/DV02/LORR schema, configuration is:

 > \<LORRBaseADS xmlns="urn:schemas-cosylab-com:LORRBaseADS:1.0" xsi:schemaLocation="urn:schemas-cosylab-com:LORRBaseADS:1.0 http://www.nrao.edu/schemas/CONTROL/LORRBaseADS.xsd"
      xmlns:amb="urn:schemas-cosylab-com:AmbDevice:1.0"  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     remoteip="192.168.1.11" adsport="866" amsnetid="192.168.56.1.1.1">

For the configuration of the Component, it is necessary to replace the Code and the Type. Where Code and Type are of the form \<HWDevice>CompAdsImpl, for example for the CONTROL/DV02/LORR component the configuration is:

 > \<Component xmlns="urn:schemas-cosylab-com:Component:1.0"
           Name="CONTROL/DV02/LORR"
           Code="LORRCompAdsImpl" 
           Type="IDL:alma/Control/LORRCompAdsBase:1.0" 
           Container="CONTROL/DV02/cppContainer"
    ImplLang="cpp"/>
