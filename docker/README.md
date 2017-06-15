# lrose-soloii

## Docker

This directory, `docker/` contains files required for building the `ncareol/lrose-soloii` **Docker** image, hosted @ <http://hub.docker.com/r/ncareol/lrose-soloii>.

### Usage

For Usage information and instructions, see [`USAGE.md`](USAGE.md)

### Build

```
docker build .
```

### Tag

Take the image ID from the build script, and assign it to a versioned tag and, optionally, the `latest` tag. *E.g.*

```
docker tag a180d022c742 ncareol/lrose-soloii:1.20-002
docker tag a180d022c742 ncareol/lrose-soloii:latest
```

The current tag convention is to use the software's version number, followed by a hyphen and a release index the is specific to the lineage of the **Docker** image.

*E.g.* `1.20-002` indicates:

- `1.20`: the image is built w/ version `1.20` of the `lrose-soloii` software
- `-002`: this is the second Docker image released for version `1.20` of `lrose-soloii`

### Release

Push the tagged images to **Docker Hub**:

```
docker push ncareol/lrose-soloii:1.20-002
docker push ncareol/lrose-soloii:latest
```

Update [`CHANGELOG.md`](CHANGELOG.md) to document the features or changes provided by the new version. Similarly, document these changes in the **Tags** section of the **Description** on **Docker Hub**, <https://hub.docker.com/r/ncareol/lrose-soloii/>.
