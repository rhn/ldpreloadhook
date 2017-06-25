VultureMap
==========

Remaps keys for Vulture's Eye.

Currently, it allows to use directional keys in a sane way when no keypad is available.

```
i o p
 \|/
k-l-;
 /|\
, . /
```

To use these as directions, hold the Windows key. The option `number_pad` must be 0.

Requirements
------------

- SDL1.2 development headers
- gcc
- Vulture's Eye

Installation
------------

```
cd vulturemap
make
```

If your binary is setuid/setgid, `LD_PRELOAD` will not just work. Some additional preparation is required.

### Fedora

```
sudo useradd -a -G vultures $MYUSER
cp /usr/games/vultureseye/vultureseye vulturemap/
su $MYUSER # to use new groups
```

Usage
-----

The hook requires th

```
LD_PRELOAD="./hook.so" vultureseye
```
