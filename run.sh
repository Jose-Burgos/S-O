#!/bin/bash
# qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev pa,id=snd0 -device ich9-intel-hda  -device hda-duplex,audiodev=snd0
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
