
# hypervisor-for-beginners
For the beginner aspiring to write a simple hypervisor.

This hypervisor is Intel VT-x/EPT based thin hypervisor for windows with minimal code.
The hypervisor demonstrates the basics:
1) programmatically check hardware features that support virtualization.
2) initialize Virtual Machine Control Structure (VMCS).
3) initialize Extended Page Table (EPT).
4) setting up stack for vmm.
5) instantiating virtualization on multiple-processors.
6) handling vm-exits.


BUILD Environment: 
1) Visual Studio Community Edition 2015 with update 3 
2) Windows 10 SDK
3) Windows 10 WDK

Installation and UnInstallation
1) Clone/Download the source code and compile it in Visual Studio.
2) To install do:
   2.1) Open cmd.exe as adminstrator and then:
        2.1.1) bcdedit /set testsigning on
        2.1.2) sc create hypervisor type= kernel binpath= c:/Users/<user>/desktop/hypervisor.sys
        2.1.3) sc start hypervisor
3) To uninstall do:
  3.1) Open cmd.exe as administrator and then:
       3.1.1) sc stop hypervisor
       3.1.2) sc delete hypervisor
  
All logs are currently printed to Dbgview.


Supported platforms:
The hypervisor has been tested on windows 7 x64 in vmware vmplayer 12 with virtualization enabled.
