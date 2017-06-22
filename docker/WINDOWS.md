# lrose-soloii

## Docker Usage on Windows

Install an **X11** server, such as

- [MobaXterm](http://mobaxterm.mobatek.net)
- [Cygwin/X](http://x.cygwin.com/)

Determine `ip` address of **Windows** host:

In a shell, run `ipconfig` to determine your **Windows** host's domain name or IP address:

```
ipconfig
```

In a shell, run `docker` to determine the IP address of the **Docker** virtual machine

```sh
docker run alpine hostname -i
```

Start **MobaXterm**. In its shell, authorize the IP address of the **Docker** virtual machine

```sh
xhost + <docker ip address>
```

Back in the **Windows** shell, run `soloii` from **Docker**:

```sh
docker run -e DISPLAY=<windows ip address>:0.0 -v \\path\to\data:/data ncareol/lrose-soloii /data
```
