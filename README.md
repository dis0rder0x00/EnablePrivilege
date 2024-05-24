# EnablePrivilege
Have a local user with some disabled privileges? This might help!

```
Usage:
.\EnablePrivilege.exe [<privilege> : all] <command>

Example usages:
.\EnablePrivilege.exe SeDebugPrivilege "cmd.exe"
.\EnablePrivilege.exe all "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe"
.\EnablePrivilege.exe all "cmd.exe /C whoami /priv"
```

## Example Output
```
C:\Users\user1\Desktop>whoami /priv

PRIVILEGES INFORMATION
----------------------

Privilege Name                Description                          State
============================= ==================================== ========
SeShutdownPrivilege           Shut down the system                 Disabled
SeChangeNotifyPrivilege       Bypass traverse checking             Enabled
SeUndockPrivilege             Remove computer from docking station Disabled
SeIncreaseWorkingSetPrivilege Increase a process working set       Disabled
SeTimeZonePrivilege           Change the time zone                 Disabled

C:\Users\user1\Desktop>EnablePrivilege.exe all "cmd.exe /C whoami /priv"

PRIVILEGES INFORMATION
----------------------

Privilege Name                Description                          State
============================= ==================================== =======
SeShutdownPrivilege           Shut down the system                 Enabled
SeChangeNotifyPrivilege       Bypass traverse checking             Enabled
SeUndockPrivilege             Remove computer from docking station Enabled
SeIncreaseWorkingSetPrivilege Increase a process working set       Enabled
SeTimeZonePrivilege           Change the time zone                 Enabled

C:\Users\user1\Desktop>
```


