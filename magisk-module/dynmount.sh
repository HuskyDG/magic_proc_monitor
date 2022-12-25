MODDIR="${0%/*}"

# API_VERSION = 1
STAGE="$1" # prepareEnterMntNs or EnterMntNs
PID="$2" # PID of app process
UID="$3" # UID of app process
PROC="$4" # Process name. Example: com.google.android.gms.unstable
USERID="$5" # USER ID of app
# API_VERSION = 2
# Enable ash standalone
# Enviroment variables: MAGISKTMP, API_VERSION
# API_VERSION = 3
STAGE="$1" # prepareEnterMntNs or EnterMntNs or OnSetUID

RUN_SCRIPT(){
    if [ "$STAGE" == "prepareEnterMntNs" ]; then
        prepareEnterMntNs
    elif [ "$STAGE" == "EnterMntNs" ]; then
        EnterMntNs
    elif [ "$STAGE" == "OnSetUID" ]; then
        OnSetUID
    fi
}

prepareEnterMntNs(){
    # this function run on app pre-initialize

    # su 2000 -c "cmd notification post -S bigtext -t 'Process monitor' 'Tag' 'pid="$PID" uid="$UID" "$PROC"'"

    # call exit 0 to let script to be run in EnterMntNs
    exit 1 # close script
}


EnterMntNs(){
    # this function will be run when mount namespace of app process is unshared
    # call exit 0 to let script to be run in OnSetUID
    exit 1 # close script
}


OnSetUID(){
    # this function will be run when UID is changed from 0 to $UID
    exit 1 # close script
}

RUN_SCRIPT


