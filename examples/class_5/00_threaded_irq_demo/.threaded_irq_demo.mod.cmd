savedcmd_threaded_irq_demo.mod := printf '%s\n'   threaded_irq_demo.o | awk '!x[$$0]++ { print("./"$$0) }' > threaded_irq_demo.mod
