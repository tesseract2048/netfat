netfat
======

kernel extension for Wi-Fi connect / disconnect event notifications

## Startup

``kextutil -vvv -t netfat.kext``

## Usage

``sysctl -a | grep netfat``:

```
kern.netfat_connect_trigger: 1
kern.netfat_disconnect_trigger: 0
```

netfat_connect_trigger will be set to 1 once wifi is connected,

netfat_disconnect_trigger will be set to 1 once wifi is disconnected.

you can write to these sysctl items.