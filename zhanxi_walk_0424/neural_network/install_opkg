#!/bin/bash

echo src/gz all http://repo.opkg.net/edison/repo/all > /etc/opkg/base-feeds.conf
echo src/gz edison http://repo.opkg.net/edison/repo/edison >> /etc/opkg/base-feeds.conf
echo src/gz core2-32 http://repo.opkg.net/edison/repo/core2-32 >> /etc/opkg/base-feeds.conf

opkg update
opkg install vim
opkg install git
