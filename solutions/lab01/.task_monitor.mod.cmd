savedcmd_task_monitor.mod := printf '%s\n'   task_monitor.o | awk '!x[$$0]++ { print("./"$$0) }' > task_monitor.mod
