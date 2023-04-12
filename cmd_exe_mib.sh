#!/bin/bash

set index=\"2.108.115\"   # "ls"
set -x

snmpset -v2c -c public localhost:4700 cmdExecutionCmdConfigRowStatus.0 = destroy || echo OK
snmpset -v2c -c public localhost:4700 cmdExecutionCmdRowStatus.0 = destroy || echo OK
snmpset -v2c -c public localhost:4700 cmdExecutionCmdRowStatus.1 = destroy || echo OK

# FIXME! set -e

snmpset -v2c -c public localhost:4700 cmdExecutionCmdConfigRowStatus."${index}" = createAndWait \
 cmdExecutionCmdConfigLine."${index}" = "ls -lrta config" cmdExecutionCmdConfigRowStatus."${index}" = active

snmptable -Cib -v2c -c public localhost:4700 cmdExecutionCmdConfigTable

snmpset -v2c -c public localhost:4700 cmdExecutionCmdRowStatus.0 = createAndWait \
  cmdExecutionCmdName.0 = "${index}" cmdExecutionCmdRowStatus.0 = active

snmptable -Cib -v2c -c public localhost:4700 cmdExecutionCmdTable
snmpbulkwalk -v2c -c public localhost:4700 cmdExecutionOutputLine

snmpset -v2c -c public localhost:4700 cmdExecutionCmdRowStatus.0 = destroy
snmpset -v2c -c public localhost:4700 cmdExecutionCmdRowStatus.1 = createAndWait \
  cmdExecutionCmdName.1 = "${index}" || echo OK

snmpbulkwalk -v2c -c public localhost:4700 cmdExecut

