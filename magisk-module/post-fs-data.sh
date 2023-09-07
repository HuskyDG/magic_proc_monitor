MODDIR="${0%/*}"
chmod 777 "$MODDIR/proc_monitor"
"$MODDIR/proc_monitor" --start
exit 0