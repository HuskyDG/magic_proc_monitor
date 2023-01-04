# Magisk Process Monitor Tool (Dynamic Mount)

- Simple tool for monitoring app process start and allow modules to run script in app process namespace with or without inject into app process. In short, run script everytime lanching an app (every app can run different logic, see [dynmount.sh](./magisk-module/dynmount.sh))

## Requirements

- Can be installed as Magisk or Zygisk module. For the normal version, it will use `logcat` for monitoring app process start. For zygisk, require Zygisk API 3+ (mean Magisk version code >= 24300)

## How to use

- Every [dynmount.sh](./magisk-module/dynmount.sh) script in module folder will be run when any app process start.
- <https://github.com/Magisk-Modules-Alt-Repo/data_isolation_support> is a good example module.

## Credit

- <https://gist.github.com/vvb2060/a3d40084cd9273b65a15f8a351b4eb0e#file-am_proc_start-cpp>
