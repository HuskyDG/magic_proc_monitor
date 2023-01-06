# Magisk Process Monitor Tool (Dynamic Mount)

- Simple tool for monitoring app process start and allow modules to run script in app process namespace with or without inject into app process. In short, run script everytime lanching an app (every app can run different logic, see [dynmount.sh](./magisk-module/dynmount.sh))

## Requirements

- Can be installed as Magisk or Zygisk module. For the normal version, it will use `logcat` for monitoring app process start. For zygisk, require Zygisk API 3+ (mean Magisk version code >= 24300)
- Zygisk mode has less performance impact, compare with `logcat` method

## How to use

- Every [dynmount.sh](./magisk-module/dynmount.sh) script in module folder will be run when any app process start.
- <https://github.com/Magisk-Modules-Alt-Repo/data_isolation_support> is a good example module.

## About Zygisk

- Zygisk has a DenyList function. After enabling Enforce DenyList option, Zygisk will not load Zygisk modules for the apps in the list. For process monitor tool, scripts will not be able to run for app on denylist with enforced denylist
- Note that the DenyList is not a hide feature, it can't even hide the presence of Zygisk itself. To solve this solution, please use hiding module or switch to Magisk Delta and use MagiskHide for hiding.

## Credit

- <https://gist.github.com/vvb2060/a3d40084cd9273b65a15f8a351b4eb0e#file-am_proc_start-cpp>
