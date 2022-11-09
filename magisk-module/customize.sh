mkdir -p "$MODPATH/system/bin"
api_level_arch_detect
[ ! -d "$MODPATH/libs/$ABI" ] && abort "! $ABI not supported"
cp -af "$MODPATH/libs/$ABI/"* "$MODPATH/system/bin"
rm -rf "$MODPATH/libs"
chcon -R u:object_r:system_file:s0 "$MODPATH/system"
chmod -R 755 "$MODPATH/system/bin"
