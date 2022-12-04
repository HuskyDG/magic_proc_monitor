MODDIR="${0%/*}"
MAGISKTMP="$(magisk --path)" || MAGISKTMP=/sbin
"$MODDIR/bin/proc_monitor" --start "$MAGISKTMP"