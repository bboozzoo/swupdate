=================================================
swupdate: syntax and tags with the default parser
=================================================

Introduction
------------

swupdate uses the library "libconfig"
as default parser for the image description.
However, it is possible to extend swupdate and add an own
parser, based on a different syntax and language as the one
supported by libconfig. In the examples directory
there is the code for a parser written in LUA, with the
description in XML.

Using the default parser, sw-description follows the
syntax rules described in the libconfig manual.
Please take a look at http://www.hyperrealm.com/libconfig/libconfig_manual.html
for an explanation of basic types.
The whole description must be contained in the sw-description file itself:
using of the #include directive is not allowed by swupdate.

The following example explains better the implemented tags:

::

	software =
	{
		version = "0.1.0";

		hardware-compatibility: [ "1.0", "1.2", "1.3"];

		/* partitions tag is used to resize UBI partitions */
		partitions: ( /* UBI Volumes */
			{
				name = "rootfs";
				device = "mtd4";
			  	size = 104896512; /* in bytes */
			},
			{
				name = "data";
				device = "mtd5";
		  		size = 50448384; /* in bytes */
			}
		);


		images: (
			{
				filename = "rootfs.ubifs";
				volume = "rootfs";
			},
			{
				filename = "swupdate.ext3.gz.u-boot";
				volume = "fs_recovery";
			},
			{
				filename = "sdcard.ext3.gz";
				device = "/dev/mmcblk0p1";
				compressed = true;
			},
			{
				filename = "bootlogo.bmp";
				volume = "splash";
			},
			{
				filename = "uboot-env";
				type = "uboot";
			},
			{
				filename = "uImage.bin";
				volume = "kernel";
			},
			{
				filename = "fpga.txt";
				type = "fpga";
			}
		);

		files: (
			{
				filename = "README";
				path = "/README";
				device = "/dev/mmcblk0p1";
				filesystem = "vfat"
			}
		);

		scripts: (
			{
				filename = "erase_at_end";
		 	},
			{
				filename = "display_info";
			}
		);

		uboot: (
			{
				name = "vram";
				value = "4M";
			},
			{
				name = "addfb";
				value = "setenv bootargs ${bootargs} omapfb.vram=1:2M,2:2M,3:2M omapdss.def_disp=lcd"
			}
		);
	}

The first tag is "software". The whole description is contained in
this tag. It is possible to group settings per device by using `Board
specific settings`_.

hardware-compatibility
----------------------

hardware-compatibility: [ "major.minor", "major.minor", ... ]

It lists the hardware revisions that are compatible with this software image.

Example:

	hardware-compatibility: [ "1.0", "1.2", "1.3"];

This means that the software is compatible with HW-Revisions
1.0, 1.2 and 1.3, but not for 1.1 or other version not explicitly
listed here.
It is then duty of the single project to find which is the
revision of the board where swupdate is running. There is no
assumption how the revision can be obtained (GPIOs, EEPROM,..)
and each project is free to select the way most appropriate.
The result must be written in the file /etc/hwrevision (or in
another file if specified as configuration option) before
swupdate is started.

partitions : UBI layout
-----------------------

This tag allows to change the layout of UBI volumes.
Please take care that MTDs are not touched and they are
configured by the Device Tree or in another way directly
in kernel.


::

	partitions: (
		{
			name = <volume name>;
			size = <size in bytes>;
			device = <MTD device>;
		},
	);

All fields are mandatory. swupdate searches for a volume of the
selected name and adjusts the size, or creates a new volume if
no volume with the given name exists.

images
------

The tag "images" collects the image that are installed to the system.
The syntax is:

::

	images (
		{
			filename[mandatory] = <Name in CPIO Archive>;
			volume[optional] = <destination volume>;
			device[optional] = <destination volume>;
			type[optional] = <handler>;
			/* optionally, the image can be compressed if it is in raw mode */
			compressed;
		},
		/* Next Image */
		.....
	);

*volume* is only used to install the image in a UBI volume. volume and
*device* cannot be used at the same time. If device is set,
the raw handler is automatically selected.

The following example is to update a UBI volume:


::

		{
			filename = "core-image-base.ubifs";
			volume = "rootfs";
		}


To update an image in raw mode, the syntax is:


::

		{
			filename = "core-image-base.ext3";
			device = "/dev/mmcblk0p1";
		}


However, writing to flash in raw mode must be managed in a special
way. Flashes must be erased before copying, and writing into NAND
must take care of bad blocks and ECC errors. For this reasons, the
handler "flash" must be selected:

For example, to copy the kernel into the MTD7 of a NAND flash:

::

		{
			filename = "uImage";
			device = "mtd7";
			type = "flash";
		}

filename is mandatory. Its is the Name of the file extracted by the stream.
volume is only mandatory in case of UBI volumes. It should be not used
in other cases.


Files
-----

It is possible to copy single files instead of images.
This is not the preferred way, but it can be used for
debugging or special purposes.

::

	files: (
		{
			filename = <Name in CPIO Archive>;
			path = <path in filesystem>;
			device = <device node >;
			filesystem = <filesystem for mount>;
		}
	);

Entries in "files" section are managed as single files. The attributes
"path" and "filesystem" are mandatory. swupdate copies the file in the path
specified after mounting the device.


Scripts
-------
::

	scripts: (
		{
			filename = <Name in CPIO Archive>;
	 	},
	);

Scripts runs in the order they are put into the sw-description file.
The result of a script is valuated by swupdate, that stops the update
with an error if the result is <> 0.

Scripts are LUA scripts and they are run using the internal interpreter.
They are copied into a temporary directory before execution and their name must
be unique inside the same cpio archive.
Scripts must have at least one of the following functions:

::

	function preinst()

swupdate scans for all scripts and check for a preinst function. It is
called before installing the images.


::

	function postinst()

swupdate scans for all scripts and check for a postinst function. It is
called after installing the images.


uboot
-----

There are two ways to update the bootloader (U-Boot) environment.
First way is to add a file with the list of variables to be changed
and setting "uboot" as type of the image. This inform swupdate to
call the U-Boot handler to manage the file.

::

		{
			filename = "uboot-env";
			type = "uboot";
		},

The format of the file is described in u-boot documentation. Each line
is in the format

::

	<name of variable>	<value>

if value is missing, the variable is unset.

The second way is to define in a group setting the variables
that must be changed:

::

	uboot: (
		{
			name = <Variable name>;
			value = <Variable value>;
		},
	)

swupdate will internally generate a script that will be passed to the
U-Boot handler for adjusting the environment.


Board specific settings
-----------------------

Each setting can be placed under a custom tag matching the board
name. This mechanism can be used to override particular setting in
board specific fashion.

Assuming that the hardware information file `/etc/hwrevision` contains
the following entry::

  my-board 0.1.0

and the following description::

	software =
	{
	        version = "0.1.0";

	        my-board = {
	                uboot: (
	                {
	                        name = "bootpart";
	                        value = "0:2";
	                }
	                );
	        };

	        uboot: (
	        {
	                name = "bootpart";
	                value = "0:1";
	        }
	        );
	}

swupdate will set `bootpart` to `0:2` in U-Boot's environment for this
board. For all other boards, `bootpart` will be set to `0:1`. Board
specific settings take precedence over default scoped settings.


Software collections and operation modes
----------------------------------------

Software collections and operations modes extend the description file
syntax to provide an overlay grouping all previous configuration
tags. The mechanism is similar to `Board specific settings`_ and can
be used for implementing a dual copy strategy or delivering both
stable and unstable images within a single update file.

The mechanism uses a custom user-defined tags placed within `software`
scope. The tag names must not be any of: `version`,
`hardware-compatibility`, `uboot`, `files`, `scripts`, `partitions`,
`images`

An example description file:

::

	software =
	{
	        version = "0.1";

	        hardware-compatibility = [ "revA" ];

	        /* differentiate running image modes/sets */
	        stable:
	        {
	                main:
	                {
	                        images: (
	                        {
	                                filename = "rootfs.ext3";
	                                device = "/dev/mmcblk0p2";
	                        }
	                        );

	                        uboot: (
	                        {
	                                name = "bootpart";
	                                value = "0:2";
	                        }
	                        );
	                };
	                alt:
	                {
	                        images: (
	                        {
	                                filename = "rootfs.ext3";
	                                device = "/dev/mmcbkl0p1";
	                        }
	                        );

	                        uboot: (
	                        {
	                                name = "bootpart";
	                                value = "0:1";
	                        }
	                        );
	                };

	        };
	}

The configuration describes a single software collection named
`stable`. Two distinct image locations are specified for this
collection: `/dev/mmcblk0p1` and `/dev/mmcblk0p2` for `main` mode and
`alt` mode respectively.

This feature can be used to implement a dual copy strategy by
specifying the collection and mode explicitly.

