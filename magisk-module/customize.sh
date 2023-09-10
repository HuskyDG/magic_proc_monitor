SKIPUNZIP=1

if ! $BOOTMODE; then
    abort "! Install from Recovery is not supported"
fi



unzip -o "$ZIPFILE" -x module.prop service.sh -d "$TMPDIR"
unzip -o "$ZIPFILE" module.prop -d "$MODPATH"

ABI="$(getprop ro.product.cpu.abi)"

# Fix ABI detection
if [ "$ABI" == "armeabi-v7a" ]; then
  ABI32=armeabi-v7a
elif [ "$ABI" == "arm64" ]; then
  ABI32=armeabi-v7a
elif [ "$ABI" == "x86" ]; then
  ABI32=x86
elif [ "$ABI" == "x64" ] || [ "$ABI" == "x86_64" ]; then
  ABI=x86_64
  ABI32=x86
fi

FLAVOR=Normal

[ ! -d "$TMPDIR/libs/$ABI" ] && abort "! $ABI not supported"
{
    randomc="$(tr -dc A-Za-z0-9 </dev/urandom | head -c 15)"
    ui_print "- Patch service: ${randomc}"
    if [ "$FLAVOR" == "Zygisk" ]; then
        ui_print "- Install Zygisk libraries"
        mkdir "$MODPATH/zygisk"
        cp -af "$TMPDIR/libs/$ABI/proc_monitor" "$MODPATH/zygisk/$ABI.so"
        cp -af "$TMPDIR/libs/$ABI32/proc_monitor" "$MODPATH/zygisk/$ABI32.so"
        sed -i "s/proc_monitor__PLACEHOLDERNAME/proc_monitor__${randomc}/g" "$MODPATH/zygisk/$ABI.so"
        sed -i "s/proc_monitor__PLACEHOLDERNAME/proc_monitor__${randomc}/g" "$MODPATH/zygisk/$ABI32.so"
    else
        ui_print "- Install execute binary"
        cp -af "$TMPDIR/libs/$ABI/proc_monitor" "$MODPATH/proc_monitor"
        sed -i "s/proc_monitor__PLACEHOLDERNAME/proc_monitor__${randomc}/g" "$MODPATH/proc_monitor"
    fi
    unzip -o "$ZIPFILE" post-fs-data.sh -d "$MODPATH"
}

cp -af "$TMPDIR/libs/$ABI/busybox" "$MODPATH/busybox"
chmod 777 "$MODPATH/busybox"
