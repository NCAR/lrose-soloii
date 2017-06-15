# lrose-soloii

## Packer

This directory, `packer/` contains files required for building the `ncareol/lrose-soloii` **VirtualBox** image.

Install packer: <https://www.packer.io/downloads.html>

### Template

The template is adapted from [`chef/bento`](https://github.com/chef/bento)'s [`centos-5.11-x86_64.json`](https://github.com/chef/bento/blob/master/centos-5.11-x86_64.json) template.

The template, `lrose-soloii.json`, configures how the VM is built. Some of this configuration references shell scripts in `scripts/`. Two shell scripts are used to respectively install `soloii` and set up the desktop environment:

- `lrose-soloii.sh`
- `desktop.sh`

### Validate template

```sh
packer validate lrose-soloii.json
```

### Build VM from template

```sh
packer build lrose-soloii.json
```

## Virtual Machine

The virtual machine is built to autologin into the GNOME desktop environment as the `lrose` user.

### Import

To run the virtual machine, the `.ova` file must be imported by **VirtualBox**. This can be done by:

- selecting the **File** menu
- selecting **Import Appliance**
- clicking the folder icon
- using the file browser to select the `.ova`
- clicking the **Import** button

Virtual machine settings can be modified by double-clicking on a parameter, **e.g.** *Name*, *CPU*, *RAM*, and entering a new parameter before clicking **Import**, and can be modified later, we well.

### Shared folders

To share data between the host and virtual machine, **VirtualBox**'s Shared Folders can be used. To add a Shared Folder to a virtual machine:

- open **VirtualBox**'s **Manager** window
- select the virtual machine from the menu on the left
- click **Settings**
- select **Shared Folders** from the menu along the top
- click the folder icon w/ the **plus (+)** sign on it
- select **Other** from the **Folder Path** dropdown menu
- use the file browser to select the host directory that you want to share w/ the virtual machine
- optionally, enter a **Folder Name** is you want to override the one that is automatically provided
- select the **Auto-mount** checkbox
- select the **Make Permanent** checkbox
- click **OK**

If the virtual machine is running, it will then need to be rebooted.

The shared folder will then be accessible @ the path `/media/sf_<folder-name>`.
