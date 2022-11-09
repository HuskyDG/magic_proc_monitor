MODDIR="${0%/*}"
MAGISKTMP="$(magisk --path)" || MAGISKTMP=/sbin
#copy binary to Magisk path
rm -rf "$MAGISKTMP/am_proc_start" && \
cp -af "$MODDIR/system/bin/am_proc_start" "$MAGISKTMP/am_proc_start"