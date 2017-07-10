# ncareol/soloii CHANGELOG


- [`1.20-005`](https://github.com/NCAR/lrose-soloii/commit/b015893)
  - install `fonts-ISO8859-2` to avoid 'Unable to load font %s' error when running image on **Linux**

- [`1.20-004`](https://github.com/NCAR/lrose-soloii/commit/1b25230)
  - display 'starting' dialog and `-h` hint when starting `soloii`

- [`1.20-003`](https://github.com/NCAR/lrose-soloii/commit/16a999f)
  - remove `CMD -h` so that image can be run without arguments and still invoke `soloii`

- [`1.20-002`](https://github.com/NCAR/lrose-soloii/commit/f6691ff)
  - use [`ncareol/centos:5.11-0`](https://hub.docker.com/r/ncareol/centos/) as base image
  - add firewall to block all incoming connections, via `iptables`

- [`1.20-001`](https://github.com/NCAR/lrose-soloii/commit/6ef755a)
  - add `ENTRYPOINT`, `/usr/bin/soloii-entrypoint`
    - default `CMD`, `-h`, to display help output
    - optional argument: path to existing directory

- [`1.20-000`](https://github.com/NCAR/lrose-soloii/commit/14b35cf483aa37cb0202ed0c3682998827ddcd3f)
  - initial working release, proof of concept
