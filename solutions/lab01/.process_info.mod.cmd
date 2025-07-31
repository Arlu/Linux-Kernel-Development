savedcmd_process_info.mod := printf '%s\n'   process_info.o | awk '!x[$$0]++ { print("./"$$0) }' > process_info.mod
