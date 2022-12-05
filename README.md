# Magisk Process Monitor Tool (Dynamic Mount)

- Simple tool for monitoring app process start and allow modules to run script in app process namespace without inject into app process

## How to use

- Every [dynmount.sh](./magisk-module/dynmount.sh) script in module folder will be run when an app process start, before and after enter the mount namespace of app process (`prepareEnterMntNs` and `EnterMntNs`)
- You can dynamically mount and unmount something which only apply for that app process when `EnterMntNs`. You can make some cool mods such as blocking access to some folder by mount `tmpfs` on some folder or bind mount the modified apk over the original one, etc...
- <https://github.com/Magisk-Modules-Alt-Repo/data_isolation_support> is a good example module.

## Credit

- <https://gist.github.com/vvb2060/a3d40084cd9273b65a15f8a351b4eb0e#file-am_proc_start-cpp>
