# soloii Virtual-Appliance Usage

## Download

Download latest `.ova` file from <http://lrose.eol.ucar.edu/soloii/>

File integrity can be assessed after download using the `.ova` file's corresponding `.sha256` file.

## Import

To run the virtual machine, the `.ova` file must be imported by **VirtualBox**. This can be done by:

1. Selecting the **File** menu
2. Selecting **Import Appliance**
3. Clicking the folder icon
4. Using the file browser to select the `.ova`
5. Clicking the **Import** button

Virtual machine settings can be modified by double-clicking on a parameter, **e.g.** *Name*, *CPU*, *RAM*, and entering a new parameter before clicking **Import**, and can be modified later, as well.

## Run

After importing the virtual machine, select it in **Virtual Box's** machine list, the left menu. Then click **Start** from the top menu.

The virtual machine is built to autologin into the **GNOME** desktop environment as the `lrose` user.

The password for both the `root` and `lrose` users is `lrose`.

**soloii** can be started by double-clicking the **soloii** icon on the desktop.

## Shared folders

To share data between the host and virtual machine, **VirtualBox**'s [**Shared Folders**](https://www.virtualbox.org/manual/ch04.html#sharedfolders) can be used. To add a Shared Folder to a virtual machine:

1. Open **VirtualBox**'s **Manager** window
2. Select the virtual machine from the menu on the left
3. Click **Settings**
4. Select **Shared Folders** from the menu along the top
5. Click the folder icon w/ the **plus (+)** sign on it
6. Select **Other** from the **Folder Path** dropdown menu
7. Use the file browser to select the host directory that you want to share w/ the virtual machine
8. Optionally, enter a **Folder Name** if you want to override the one that is automatically provided
9. Select the **Auto-mount** checkbox
10. Select the **Make Permanent** checkbox
11. Click **OK**

If the virtual machine is running, it will then need to be rebooted.

The **Shared Folder** will then be accessible @ the path `/media/sf_<folder-name>`.

