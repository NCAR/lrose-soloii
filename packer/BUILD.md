# soloii Packer: Build

Instructions for building soloii Virtual Appliance using [**Packer**](https://www.packer.io).

## Install

Install packer: <https://www.packer.io/downloads.html>

## Template

The template is adapted from [`chef/bento`](https://github.com/chef/bento)'s [`centos-7.3-x86_64.json`](https://github.com/chef/bento/blob/master/centos-7.3-x86_64.json) template.

The template, `lrose-soloii.json`, configures how the VM is built. Some of this configuration references shell scripts in `scripts/`. Three shell scripts are used to respectively set up the desktop environment, install **Docker** and set up soloii's invocation via **Docker**:

- `desktop.sh`
- `soloii-docker.sh`
- `docker.sh`

## Validate template

```sh
packer validate lrose-soloii.json
```

## Build VM from template

```sh
packer build lrose-soloii.json
```

## Publish

Copy the generated `.ova` file to `/net/vagrant/rsf/soloii`. The `.ova` will subsequently be available @ <http://lrose.eol.ucar.edu/soloii/>
