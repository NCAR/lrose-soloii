lrose-soloii Virtual Appliance

The password for both the root and lrose users is lrose.

To run soloii:

- Double-click the soloii icon on the Desktop

- Or run soloii-docker from the command line, w/ an optional argument of a path to a directory containing SWP files.

To share data between the host and virtual machine, VirtualBox's Shared Folders can be used. To add a Shared Folder to a virtual machine:

1. Open VirtualBox's Manager window
2. Select the virtual machine from the menu on the left
3. Click Settings
4. Select Shared Folders from the menu along the top
5. Click the folder icon w/ the plus (+) sign on it
6. Select Other from the Folder Path dropdown menu
7. Use the file browser to select the host directory that you want to share w/ the virtual machine
8. Optionally, enter a Folder Name if you want to override the one that is automatically provided
9. Select the Auto-mount checkbox
10. Select the Make Permanent checkbox
11. Click OK
12. If the virtual machine is running, it will then need to be rebooted.

The Shared Folder will then be accessible @ the path /media/sf_<folder-name>.

The Virtual Machine window can be resized by clicking and dragging on the corners and edges of the VM window.
