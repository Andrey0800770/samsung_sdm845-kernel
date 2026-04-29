########################################################################
# Kernel settings
########################################################################

# Kernel variant. This is currently used only on the Source package name.
# Use 'android' for Android kernels ("downstream") or 'mainline' for upstream
# kernels.
VARIANT = android

# Kernel base version
KERNEL_BASE_VERSION = 4.9.337

# The kernel cmdline to use
KERNEL_BOOTIMAGE_CMDLINE = androidboot.hardware=qcom video=vfb:640x400,bpp=32,memsize=3072000 msm_rtb.filter=0x237 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 service_locator.enable=1 swiotlb=2048 androidboot.configfs=true androidboot.usbcontroller=a600000.dwc3 firmware_class.path=/vendor/firmware_mnt/image/ loop.max_part=7 androidboot.selinux=permissive buildvariant=userdebug console=tty0 selinux=0 apparmor=1 security=apparmor droidian.lvm.prefer

# Slug for the device vendor. This is going to be used in the KERNELRELASE
# and package names.
DEVICE_VENDOR = samsung

# Slug for the device model. Like above.
DEVICE_MODEL = star2qltechn
# Slug for the device platform. If unsure, keep this commented.
# DEVICE_PLATFORM = platform

# Marketing-friendly full-name. This will be used inside package descriptions
DEVICE_FULL_NAME = Samsung Galaxy S9+

# Whether to use configuration fragments to augment the kernel configuration.
# If unsure, keep this to 0.
KERNEL_CONFIG_USE_FRAGMENTS = 0

# Enable kernel config device extra fragments
# KERNEL_CONFIG_EXTRA_FRAGMENTS = container.config debug.config other.config

# Whether to use diffconfig to generate the device-specific configuration.
# If you enable this, you should set KERNEL_CONFIG_USE_FRAGMENTS to 1.
# If unsure, keep this to 0.
KERNEL_CONFIG_USE_DIFFCONFIG = 0

# The diffconfig to apply. Only used when KERNEL_CONFIG_USE_DIFFCONFIG is
# enabled.
# KERNEL_PRODUCT_DIFFCONFIG = my_diffconfig

# Defconfig to use
KERNEL_DEFCONFIG = star2qlte_chn_halium_defconfig

# Whether to include DTBs with the image. Use 0 (no) or 1.
# GKI devices should set this to 0
KERNEL_IMAGE_WITH_DTB = 1

# Path to the DTB
# If you leave this undefined, an attempt to find it automatically
# will be made.
# KERNEL_IMAGE_DTB = arch/arm64/boot/dts/qcom/my_dtb.dtb

# Whether to include a DTB Overlay. Use 0 (no) or 1.
# GKI devices should set this to 0
KERNEL_IMAGE_WITH_DTB_OVERLAY = 0

# Path to the DTB overlay.
# If you leave this undefined, an attempt to find it automatically
# will be made.
# KERNEL_IMAGE_DTB_OVERLAY = arch/arm64/boot/dts/qcom/my_overlay.dtbo

# Whether to include the DTB Overlay into the kernel image
# Use 0 (no, default) or 1.
# dtbo.img will always be shipped in the linux-bootimage- package.
# GKI devices should set this to 0
KERNEL_IMAGE_WITH_DTB_OVERLAY_IN_KERNEL = 0

# Path to a specifc configuration file for mkdtboimg.
# The default is to leave it undefined.
# KERNEL_IMAGE_DTB_OVERLAY_CONFIGURATION = debian/custom_dtbo_config.cfg

# Path to the DTB directory. Only define if KERNEL_IMAGE_DTB_OVERLAY_CONFIGURATION
# is defined too.
# KERNEL_IMAGE_DTB_OVERLAY_DTB_DIRECTORY = arch/arm64/boot/dts/qcom

# Path to the prebuilt DT image. should only be defined on header version 1 and below. 
# mostly used on samsung devices. default is to leave it undefined
# KERNEL_PREBUILT_DT = debian/dt.img

# Various other settings that will be passed straight to mkbootimg
# GKI devices can leave BASE_OFFSET, KERNEL_OFFSET, INITRAMFS_OFFSET, SECOND_OFFSET and TAGS_OFFSET empty
KERNEL_BOOTIMAGE_PAGE_SIZE = 4096
KERNEL_BOOTIMAGE_BASE_OFFSET = 0x00000000
KERNEL_BOOTIMAGE_KERNEL_OFFSET = 0x00008000
KERNEL_BOOTIMAGE_INITRAMFS_OFFSET = 0x02000000
KERNEL_BOOTIMAGE_SECONDIMAGE_OFFSET = 0x00f00000
KERNEL_BOOTIMAGE_TAGS_OFFSET = 0x01e00000

# Specify boot image security patch level if needed
# KERNEL_BOOTIMAGE_PATCH_LEVEL = 2022-04-05

# Specify boot image OS version if needed
# KERNEL_BOOTIMAGE_OS_VERSION = 12.0.0

# Required for header version 2, ignore otherwise
# GKI devices should leave this offset empty
KERNEL_BOOTIMAGE_DTB_OFFSET = 0x1f00000

# Kernel bootimage version. Defaults to 0 (legacy header).
# As a rule of thumb:
# Devices launched with Android 8 and lower: version 0
# Devices launched with Android 9: version 1
# Devices launched with Android 10: version 2
# Devices launched with Android 11: version 2 or 3 (GKI)
# Devices launched with Android 12: version 3 (GKI) or 4 (GKI)
# Devices launched with Android 13: version 3 (GKI) or 4 (GKI)
KERNEL_BOOTIMAGE_VERSION = 1

# Kernel initramfs compression. Defaults to gzip.
# All non-gki devices need a gzip (gz) initramfs
# For devices launched with a GKI kernel it can be either gzip (gz) or lz4 (lz4)
KERNEL_INITRAMFS_COMPRESSION = gz

# Whether to generate a vendor_boot image. Supported since
# bootimage version 3.
# vendor_boot images will contain the required kernel modules that
# are to be loaded during the initramfs stage.
# If building for a device with bootimage version >= 3, you
# should probably want to enable it.
#
# Module list should be added in KERNEL_DIR/droidian/modules.load.
KERNEL_BOOTIMAGE_GENERATE_VENDOR_BOOT = 0

# The cmdline for the vendor_boot image
KERNEL_BOOTIMAGE_VENDOR_CMDLINE = 

########################################################################
# Android verified boot
########################################################################

# Whether to build a flashable vbmeta.img. Please note that currently
# only empty vbmeta images (disabling verified boot) can be generated.
# Use 0 (no) or 1 (default).
DEVICE_VBMETA_REQUIRED = 1

# Samsung devices require a special flag. Enable the following if your
# device is a Samsung device that requires flag 0 to be present
# Use 0 (no, default) or 1.
DEVICE_VBMETA_IS_SAMSUNG = 0

# boot partition size. If specified, an AVB footer will be added at the
# end of the bootimage.
KERNEL_BOOTIMAGE_PARTITION_SIZE = 

########################################################################
# Automatic flashing on package upgrades
########################################################################

# Whether to enable kernel upgrades on package upgrades. Use 0 (no) or 1.
FLASH_ENABLED = 0

# If your device is treble-ized, but aonly, you should set the following to
# 1 (yes).
FLASH_IS_AONLY = 1

# `flash-bootimage` defaults are enough for most recent devices, but legacy
# devices won't work out of the box.
# If you set the following to 1, this package will set `flash-bootimage`'s
# DEVICE_IS_AB and BOOTIMAGE_SLOT_A accordingly, thus enabling flashing
# on older devices.
#
# Do not enable if you don't know what you're doing
FLASH_IS_LEGACY_DEVICE = 0

# On some exynos devices partition names are capitalized (boot is BOOT and so on)
# This flag makes the kernel to get flashed to the correct partition on updates.
FLASH_IS_EXYNOS = 0

# On some devices flashing userdata.img via fastboot fails and telnet should
# be used instead. Set to 1 for telnet and 0 for fastboot
FLASH_USE_TELNET = 0

# Device manufacturer. This must match the `ro.product.vendor.manufacturer`
# Android property. If you don't want to specify this, leave it undefined,
# FLASH_INFO_CPU will be checked instead.
FLASH_INFO_MANUFACTURER = Samsung

# Device model. This must match the `ro.product.vendor.model`
# Android property. If you don't want to specify this, leave it undefined,
# FLASH_INFO_CPU will be checked instead.
FLASH_INFO_MODEL = Galaxy S9+

# Device CPU. This will be grepped against /proc/cpuinfo to check if
# we're running on the specific device. Note this is a last-resort
# method, specifying FLASH_INFO_MANUFACTURER and FLASH_INFO_MODEL is
# recommended.
FLASH_INFO_CPU = SDM845

# Space-separated list of supported device ids as reported by fastboot
FLASH_INFO_DEVICE_IDS = star2qlte star2qltesq star2qltechn star2qlteue

########################################################################
# Kernel build settings
########################################################################

# Whether to cross-build. Use 0 (no) or 1.
BUILD_CROSS = 1

# (Cross-build only) The build triplet to use. You'll probably want to
# use aarch64-linux-android- if building Android kernels.
# might need to use aarch64-linux-gnu- with newer kernels such as GKI
BUILD_TRIPLET = aarch64-linux-gnu-

# (Cross-build only) The build triplet to use with clang. You'll probably
# want to use aarch64-linux-gnu- here.
BUILD_CLANG_TRIPLET = aarch64-linux-gnu

# The compiler to use. Recent Android kernels are built with clang.
BUILD_CC = clang

# Use llvm instead of gcc. Recent Android kernels (past android 12) need llvm.
BUILD_LLVM = 1

# Set to 1 to skip modules packaging if CONFIG_MODULES is disabled in defconfig 
BUILD_SKIP_MODULES = 0

# Set clang version
CLANG_VERSION = 22.0-r584948
# Set to 1 to use a manually installed toolchain
# Remember to update the path in BUILD_PATH
CLANG_CUSTOM = 1

# Extra paths to prepend to the PATH variable. You'll probably want
# to specify the clang path here (the default).
BUILD_PATH = /opt/clang-custom/bin

# Extra packages to add to the Build-Depends section. Mainline builds
# can have this section empty, unless cross-building.
# The default is enough to install the Android toolchain, including clang.
DEB_TOOLCHAIN = linux-initramfs-halium-generic:arm64, binutils-aarch64-linux-gnu, g++-4.9-aarch64-linux-android, libgcc-4.9-dev-aarch64-linux-android-cross, gcc-arm-linux-gnueabi

# Where we're building on
# Comment DEB_BUILD_ON to enable host arch autodetection.
# Only amd64 and arm64 archs are supported yet.
DEB_BUILD_ON = amd64

# Where we're going to run this kernel on
DEB_BUILD_FOR = arm64

# Target kernel architecture
KERNEL_ARCH = arm64

# Kernel target to build
KERNEL_BUILD_TARGET = Image.gz-dtb
