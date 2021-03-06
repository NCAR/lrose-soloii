# lrose-soloii Docker Development

## Build

```
docker build .
```

## Tag

Take the image ID from the build script, and assign it to a versioned tag and, optionally, the `latest` tag. *E.g.*

```
docker tag a180d022c742 ncareol/soloii:1.20-002
docker tag a180d022c742 ncareol/soloii:latest
```

The current tag convention is to use the software's version number, followed by a hyphen and a release index that is specific to the lineage of the **Docker** image for this version of the software.

*E.g.* `1.20-002` indicates:

- `1.20`: the image is built w/ version `1.20` of the `lrose-soloii` software
- `-002`: the image is the second **Docker** image released for version `1.20` of `lrose-soloii`

## Release

Push the tagged images to **Docker Hub**:

```
docker push ncareol/soloii:1.20-002
docker push ncareol/soloii:latest
```

Update [`CHANGELOG.md`](CHANGELOG.md) to document the features or changes provided by the new version. Similarly, document these changes in the **Tags** section of the **Description** on **Docker Hub**, <https://hub.docker.com/r/ncareol/soloii/>.

