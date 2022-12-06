api_level_arch_detect
[ ! -d "$MODPATH/libs/$ABI" ] && abort "! $ABI not supported"
cp -af "$MODPATH/libs/$ABI/proc_monitor" "$MODPATH"
rm -rf "$MODPATH/libs"
chmod -R 755 "$MODPATH/proc_monitor"
