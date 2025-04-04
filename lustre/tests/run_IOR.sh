#!/bin/bash

TMP=${TMP:-/tmp}

TESTLOG_PREFIX=${TESTLOG_PREFIX:-$TMP/recovery-mds-scale}
TESTNAME=${TESTNAME:-""}
[ -n "$TESTNAME" ] && TESTLOG_PREFIX=$TESTLOG_PREFIX.$TESTNAME

LOG=$TESTLOG_PREFIX.$(basename $0 .sh)_stdout.$(hostname -s).log
DEBUGLOG=$(echo $LOG | sed 's/\(.*\)stdout/\1debug/')

error () {
	echo "$@"
	exit 17
}

mkdir -p ${LOG%/*}

rm -f $LOG $DEBUGLOG
exec 2>$DEBUGLOG
set -x

. $(dirname $0)/functions.sh

IOR=${IOR:-"$(which IOR)"}

assert_env MOUNT END_RUN_FILE LOAD_PID_FILE IOR

trap signaled TERM

# if MACHINEFILE set and exists -- use it
if [ -z $MACHINEFILE ] || [ ! -e $MACHINEFILE ]; then
    MACHINEFILE=$TMP/$(basename $0)-$(hostname).machines
    echo $(hostname) >$MACHINEFILE
fi

THREADS_PER_CLIENT=${THREADS_PER_CLIENT:-3}
NUM_CLIENTS=$(cat $MACHINEFILE | wc -l)

# recovery-*-scale scripts use this to signal the client loads to die
echo $$ >$LOAD_PID_FILE

TESTDIR=${TESTDIR:-$MOUNT/d0.ior-$(hostname)}

while [ ! -e "$END_RUN_FILE" ]; do
	echoerr "$(date +'%F %H:%M:%S'): IOR run starting"
	rm -rf $TESTDIR
	client_load_mkdir $TESTDIR
	if [ $? -ne 0 ]; then
		echoerr "$(date +'%F %H:%M:%S'): failed to create $TESTDIR"
		echo $(hostname) >> $END_RUN_FILE
		break
	fi

	# need this only if TESTDIR is not default
	chmod -R 777 $TESTDIR

	sync

	run_ior fpp $TESTDIR  1>>$LOG &

	load_pid=$!
	wait $load_pid
	if [ ${PIPESTATUS[0]} -eq 0 ]; then
        	echoerr "$(date +'%F %H:%M:%S'): IOR succeeded"
        	cd $TMP
	else
		enospc_detected $DEBUGLOG &&
			echoerr "$(date +'%F %H:%M:%S'): IOR ENOSPC, ignored" &&
			continue

		echoerr "$(date +'%F %H:%M:%S'): IOR failed"
		if [ -z "$ERRORS_OK" ]; then
			echo $(hostname) >> $END_RUN_FILE
		fi
	fi
done

echoerr "$(date +'%F %H:%M:%S'): IOR run exiting"
