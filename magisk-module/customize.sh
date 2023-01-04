if ! $BOOTMODE; then
    abort "! Install from Recovery is not supported"
fi

api_level_arch_detect
[ ! -d "$MODPATH/libs/$ABI" ] && abort "! $ABI not supported"
if [ "$(magisk --sqlite "SELECT value FROM settings WHERE (key='zygisk')")" == "value=1" ]; then
    ui_print "- Install as Zygisk module"
    mkdir "$MODPATH/zygisk"
    cp -af "$MODPATH/libs/$ABI/proc_monitor" "$MODPATH/zygisk/$ABI.so"
    cp -af "$MODPATH/libs/$ABI32/proc_monitor" "$MODPATH/zygisk/$ABI32.so"
    rm -rf "$MODPATH/service.sh"
else
    ui_print "- Install as normal module"
    cp -af "$MODPATH/libs/$ABI/proc_monitor" "$MODPATH/proc_monitor"
fi
rm -rf "$MODPATH/libs"