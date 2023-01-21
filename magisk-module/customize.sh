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

[ ! -d "$TMPDIR/libs/$ABI" ] && abort "! $ABI not supported"
if [ "$(magisk --sqlite "SELECT value FROM settings WHERE (key='zygisk')")" == "value=1" ]; then
    ui_print "- Install as Zygisk module"
    mkdir "$MODPATH/zygisk"
    cp -af "$TMPDIR/libs/$ABI/proc_monitor" "$MODPATH/zygisk/$ABI.so"
    cp -af "$TMPDIR/libs/$ABI32/proc_monitor" "$MODPATH/zygisk/$ABI32.so"
    sed -Ei 's/^description=(\[.*][[:space:]]*)?/description=[ Zygisk version ] /g' "$MODPATH/module.prop"
else
    ui_print "- Install as normal module"
    cp -af "$TMPDIR/libs/$ABI/proc_monitor" "$MODPATH/proc_monitor"
    unzip -o "$ZIPFILE" service.sh -d "$MODPATH"
fi
