#!/bin/sh

# Set defaults
MACHINE="auto"
DELAYED_START=1
DO_LOAD50=0
DO_LOAD100=0
DO_MEMTEST=0
VERBOSE=0

STRESS_SCHEDULE_TIME=10
STRESS_TIME=0

STARTUP_DELAY_VALUE="5m"
MEMTEST_SIZE=100M
MEMTEST_LOOPS=3

if test -f /etc/ktnstress.conf; then
    . /etc/ktnstress.conf
fi

ui_show_help()
{
    echo "ktnstress.sh [OPTIONS]"
    echo ""
    echo "Stress test selection"
    echo "--load50    Stress test with 50% load"
    echo "--load100   Stress test with 100% load [default]"
    echo "--mem       Execute memtest"
    echo "--nodelay   Execute test immediately"
    echo ""
    echo "Machine configuration"
    echo "-m MACHINE  Use machine configuration. Valid settings are"
    echo "     'ul' and 'mp1'. Without this parameter the machine is"
    echo "     automatically detected"
    echo ""
    echo "-v      Verbose output"
    echo ""
    echo "Without parameters Stress test selection is determined from"
    echo "the dip switches and MACHINE is automatically detected"
    echo ""
}

ui_validate_params()
{
    case ${MACHINE} in
    ul|mp1)
        ;;
    *)
        ui_die_with_error 1 "MACHINE configuration '${MACHINE}' is not supported"
    esac
}

ui_die_with_error()
{
    local exit_code="${1}"
    shift
    echo "ERROR: $*"
    exit ${exit_code}
}

# Debug echo which prints to stderr
decho()
{
    echo $* > /dev/stderr
}

vecho()
{
    if test "${VERBOSE}" -ne 1; then
        return
    fi
    echo $*
}

ui_parse_cmdline()
{
    OPT_MACHINE=""
    OPT_DO_LOAD50=""
    OPT_DO_LOAD100=""
    OPT_DO_MEMTEST=""

    while test "$1" != ""; do
    
	case ${1} in
	-h | --help)
	    ui_show_help
	    exit 0
	    ;;
	--load50) 
	    OPT_DO_LOAD50=1
	    OPT_DO_LOAD100=0
	    ;;
	--load100) 
	    OPT_DO_LOAD100=1
	    OPT_DO_LOAD50=0
	    ;;
	--mem)
	    OPT_DO_MEMTEST=1
	    ;;
    --nodelay)
        OPT_NODELAY=1
        ;;
	-m)
	    OPT_MACHINE=$2
        shift
	    ;;
	-v)
	    VERBOSE=1
	    ;;
	-*)
	    ui_die_with_error 1 "Unknown option" "${1}"
	    ;;
	*)
	    ui_die_with_error 1 "Non option value" "${1}"
	esac
	shift
    done

    if test "${VERBOSE}" -eq 1; then
        echo "=== Command line options ==="
        echo "  OPT_MACHINE    = ${OPT_MACHINE}"
        echo "  OPT_NODELAY    = ${OPT_NODELAY}"
        echo "  OPT_DO_LOAD50  = ${OPT_DO_LOAD50}"
        echo "  OPT_DO_LOAD100 = ${OPT_DO_LOAD100}"
        echo "  OPT_DO_MEMTEST = ${OPT_DO_MEMTEST}"
    fi
}

detect_machine()
{
    DET_MACHINE="NOTSUPPORTED"
    if grep -iq stm32mp /proc/device-tree/compatible ; then
        DET_MACHINE="mp1"
    fi
    if grep -iq "imx6ul-" /proc/device-tree/compatible ; then
        DET_MACHINE="ul"
    fi
    if grep -iq "imx6ull-" /proc/device-tree/compatible ; then
        DET_MACHINE="ul"
    fi
    # TODO: Detection
    if false; then
        DET_MACHINE="imx8"
    fi
    # TODO: Detection
    if false; then
        DET_MACHINE="dual"
    fi
}

detect_configuration()
{
    DIP1=
    DIP2=
    DIP3=
    DIP4=

    case ${MACHINE} in
    ul)
        # Inputs are inverted
        DIP1=$(gpioget gpiochip0 24 | tr 01 10)
        DIP2=$(gpioget gpiochip0 25 | tr 01 10)
        DIP3=$(gpioget gpiochip0 26 | tr 01 10)
        DIP4=
        ;;
    mp1)
        # Inputs are inverted
        DIP1=$(gpioget $(gpiofind CFG0) | tr 01 10)
        DIP2=$(gpioget $(gpiofind CFG1) | tr 01 10)
        DIP3=$(gpioget $(gpiofind CFG2) | tr 01 10)
        DIP4=
        ;;
    *)
        ui_die_with_error 1 "Unsupported machine ${MACHINE}"
        ;;
    esac
    DET_DO_LOAD50=${DIP1}
    DET_DO_LOAD100=${DIP2}
    DET_DO_MEMTEST=${DIP3}

    if test "${VERBOSE}" -eq 1; then
        echo "detected configuration:"
        echo "  MACHINE        = ${MACHINE}"
        echo "  DET_DO_LOAD50  = ${DET_DO_LOAD50}"
        echo "  DET_DO_LOAD100 = ${DET_DO_LOAD100}"
        echo "  DET_DO_MEMTEST = ${DET_DO_MEMTEST}"
    fi
}

calculate_configuration()
{
    if test -z "${OPT_MACHINE}"; then
        MACHINE=${DET_MACHINE}
    else
        MACHINE=${OPT_MACHINE}
    fi
    if test -n "${OPT_NODELAY}"; then
        DELAYED_START=0
    fi
    if test -z "${OPT_DO_LOAD50}"; then
        DO_LOAD50=${DET_DO_LOAD50}
    else
        DO_LOAD50=${OPT_DO_LOAD50}
    fi
    if test -z "${OPT_DO_LOAD100}"; then
        DO_LOAD100=${DET_DO_LOAD100}
    else
        DO_LOAD100=${OPT_DO_LOAD100}
    fi
    if test -z "${OPT_DO_MEMTEST}"; then
        DO_MEMTEST=${DET_DO_MEMTEST}
    else
        DO_MEMTEST=${OPT_DO_MEMTEST}
    fi

    if test "${VERBOSE}" -eq 1; then
        echo "calculated configuration:"
        echo "  MACHINE        = ${MACHINE}"
        echo "  DELAYED_START  = ${DELAYED_START}"
        echo "  DO_LOAD50      = ${DO_LOAD50}"
        echo "  DO_LOAD100     = ${DO_LOAD100}"
        echo "  DO_MEMTEST     = ${DO_MEMTEST}"
    fi
}

enable_gpios()
{
    VALUE=${1}
    case ${MACHINE} in
    imx8)
        gpioset gpiochip0 3=${VALUE}
        gpioset gpiochip0 7=${VALUE}
        gpioset gpiochip0 9=${VALUE}
        gpioset gpiochip0 11=${VALUE}
        ;;
    ul)
        gpioset gpiochip4 5=${VALUE}
        gpioset gpiochip4 1=${VALUE}
        ;;
    mp1)
        gpioset $(gpiofind DOUT1)=${VALUE}
        gpioset $(gpiofind DOUT2)=${VALUE}
        ;;
    *)
        ui_die_with_error 1 "Unsupported machine ${MACHINE}"
        ;;
    esac
}

detect_cores()
{
    CORES=$(grep processor /proc/cpuinfo | wc -l)
}

do_test_load50()
{
    # Use only half of the cores for 50% load
    detect_cores
    MATRIX=$(( ${CORES} / 2 ))

    if test "${MATRIX}" -eq 0; then
        # We have only one core, so do some scheduling
        echo "Only one core detected! Starting load50 in scheduled mode (${STRESS_SCHEDULE_TIME})"
        ( 
            while true; do
                stress-ng --matrix 1 -t ${STRESS_SCHEDULE_TIME}
                sleep ${STRESS_SCHEDULE_TIME}
            done
        )&
    else
        echo "${MATRIX} of ${CORES} cores activated for stress test"
        vecho "Starting load50: stress-ng --matrix ${MATRIX} -t ${STRESS_TIME}"
        stress-ng --matrix ${MATRIX} -t ${STRESS_TIME} &
    fi
}

do_test_load100()
{
    vecho "Starting load100: stress-ng --matrix 0 -t ${STRESS_TIME}"
    stress-ng --matrix 0 -t ${STRESS_TIME} &
}

do_memtest()
{
    LOGFILE=/home/root/memtest_$(date "+%F_%T").log
    vecho "Starting memtest: memtest ${MEMTEST_SIZE} ${MEMTEST_LOOPS}"
    memtester ${MEMTEST_SIZE} ${MEMTEST_LOOPS} 2>&1 | tee ${LOGFILE}
}

ui_start_tests()
{
    if test ${DO_LOAD50} -ne 0; then
        enable_gpios 1
        echo "Start test with 50% system load"
        do_test_load50
    fi
    if test ${DO_LOAD100} -ne 0; then
        enable_gpios 1
        echo "Start test with 100% system load"
        do_test_load100
    fi
    if test ${DO_MEMTEST} -ne 0; then
        echo "Starting memtest"
        do_memtest
    fi

    # Wait for all background jobs to finish
    echo "Waiting for the tests to finish ..."
    wait
    echo "All tests finished now"
}

main()
{

    ui_parse_cmdline $*

    detect_machine
    calculate_configuration

    detect_configuration
    calculate_configuration
    ui_validate_params

    echo "Starting ktnstress"

    enable_gpios 0

    if test "${DELAYED_START}" -eq 1; then
        echo "Wait for startup delay to expire (${STARTUP_DELAY_VALUE})..."
        sleep ${STARTUP_DELAY_VALUE}
        echo "Delay expired, starting tests now!"
    fi

    ui_start_tests
}

main $*
