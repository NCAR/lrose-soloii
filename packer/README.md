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
