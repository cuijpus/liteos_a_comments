# LiteOS Cortex-A<a name="EN-US_TOPIC_0000001096612501"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Constraints](#section119744591305)
-   [Usage](#section741617511812)
-   [Preparations](#section1579912573329)
-   [Source Code Acquisition](#section11443189655)
-   [Compilation and Building](#section2081013992812)
-   [Contribution](#section1371123476304)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

The OpenHarmony LiteOS Cortex-A is a new-generation kernel developed based on the Huawei LiteOS kernel. Huawei LiteOS is a lightweight operating system \(OS\) built for the Internet of Things \(IoT\) field. With the rapid development of the IoT industry, OpenHarmony LiteOS Cortex-A brings small-sized, low-power, and high-performance experience and builds a unified and open ecosystem for developers. In addition, it provides rich kernel mechanisms, more comprehensive Portable Operating System Interface \(POSIX\), and a unified driver framework, Hardware Driver Foundation \(HDF\), which offers unified access for device developers and friendly development experience for application developers.  [Figure 1](#fig27311582210)  shows the architecture of the OpenHarmony LiteOS Cortex-A kernel.

**Figure  1**  Architecture of the OpenHarmony LiteOS Cortex-A kernel<a name="fig27311582210"></a>  
![](figures/architecture-of-the-openharmony-liteos-cortex-a-kernel.png "architecture-of-the-openharmony-liteos-cortex-a-kernel")

## Directory Structure<a name="section161941989596"></a>

```
/kernel/liteos_a
├── apps                   # User-space init and shell application programs
├── arch                   # System architecture, such as ARM
│   └── arm                # Code for ARM architecture
├── bsd                    # Code of the driver and adaptation layer module related to the FreeBSD, such as the USB module
├── compat                 # Kernel API compatibility
│   └── posix              # POSIX APIs
├── drivers                # Kernel drivers
│   └── char               # Character device
│       ├── mem            # Driver for accessing physical input/output (I/O) devices
│       ├── quickstart     # APIs for quick start of the system
│       ├── random         # Driver for random number generators
│       └── video          # Framework of the framebuffer driver
├── fs                     # File system module, which mainly derives from the NuttX open-source project
│   ├── fat                # FAT file system
│   ├── jffs2              # JFFS2 file system
│   ├── include            # Header files exposed externally
│   ├── nfs                # NFS file system
│   ├── proc               # proc file system
│   ├── ramfs              # RAMFS file system
│   └── vfs                # VFS layer
├── kernel                 # Kernel modules including the process, memory, and IPC modules
│   ├── base               # Basic kernel modules including the scheduling and memory modules
│   ├── common             # Common components used by the kernel
│   ├── extended           # Extended kernel modules including the dynamic loading, vDSO, and LiteIPC modules
│   ├── include            # Header files exposed externally
│   └── user               # Init process loading
├── lib                    # Kernel library
├── net                    # Network module, which mainly derives from the lwIP open-source project
├── platform               # Code for supporting different systems on a chip (SOCs), such as Hi3516D V300
│   ├── hw                 # Logic code related to clocks and interrupts
│   ├── include            # Header files exposed externally
│   └── uart               # Logic code related to the serial port
├── platform               # Code for supporting different systems on a chip (SOCs), such as Hi3516D V300
├── security               # Code related to security features, including process permission management and virtual ID mapping management
├── syscall                # System calling
└── tools                  # Building tools as well as related configuration and code
```

## Constraints<a name="section119744591305"></a>

-   Programming languages: C and C++
-   Applicable development boards: Hi3516D V300
-   Hi3516D V300 uses the FAT file system by default.

## Usage<a name="section741617511812"></a>

OpenHarmony LiteOS Cortex-A supports the  [Hi3516D V300](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/quickstart-appendix-hi3516.md). You can develop and run your applications based on this development board.

### Preparations<a name="section1579912573329"></a>

You need to set up the compilation environment on Ubuntu.

-   [Setting Up Ubuntu Development Environment](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/Readme-EN.md)

### Source Code Acquisition<a name="section11443189655"></a>

Download and decompress a set of source code on a Ubuntu server to acquire the  [source code](https://gitee.com/openharmony/docs/blob/master/en/device-dev/get-code/sourcecode-acquire.md).

### Compilation and Building<a name="section2081013992812"></a>

For details about how to develop the first application, see: [Developing the First Example Program Running on Hi3516](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/quickstart-pkg-3516-helloworld.md)

For compilation, please refer to the [compilation instructions](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/quickstart-pkg-3516-build.md).

## Contribution<a name="section1371123476304"></a>

[How to involve](https://gitee.com/openharmony/docs/blob/HEAD/en/contribute/contribution-guide.md)

[Commit message spec](https://gitee.com/openharmony/kernel_liteos_a/wikis/Commit%20message%20%E8%A7%84%E8%8C%83)

[C&C++ Secure Coding Guide](https://gitee.com/openharmony/docs/blob/master/en/contribute/OpenHarmony-c-cpp-secure-coding-guide.md)

[LiteOS-A Kernel Coding Style Guide](https://gitee.com/openharmony/kernel_liteos_a/wikis/LiteOS-A%20%E5%86%85%E6%A0%B8%E7%BC%96%E7%A0%81%E8%A7%84%E8%8C%83%E6%8C%87%E5%8D%97)

## Repositories Involved<a name="section1371113476307"></a>

[Kernel subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/kernel.md)

[drivers\_liteos](https://gitee.com/openharmony/drivers_liteos/blob/master/README.md)

[kernel\_liteos\_a](https://gitee.com/openharmony/kernel_liteos_a/blob/master/README.md)

