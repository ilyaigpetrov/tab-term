#!/bin/sh

mkdir -p ~/.mozilla/native-messaging-hosts/
# FireFox
cp ./app-manifest-firefox.json ~/.mozilla/native-messaging-hosts/hello_world_host.json
# Chrome
cp ./app-manifest-chrome.json ~/.config/google-chrome/NativeMessagingHosts/hello_world_host.json