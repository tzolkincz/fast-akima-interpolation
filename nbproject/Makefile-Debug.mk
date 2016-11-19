#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=icc
CCC=icc
CXX=icc
FC=ifort
AS=as

# Macros
CND_PLATFORM=Intel-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/fast_akima.o \
	${OBJECTDIR}/glucose/glucose_impl.o \
	${OBJECTDIR}/helpers.o \
	${OBJECTDIR}/interpolator.o \
	${OBJECTDIR}/lib/glucose/CommonApprox.o \
	${OBJECTDIR}/lib/glucose/GlucoseLevels.o \
	${OBJECTDIR}/lib/glucose/iface/ApproxIface.o \
	${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl.o \
	${OBJECTDIR}/lib/glucose/rtl/referencedImpl.o \
	${OBJECTDIR}/perf_test.o \
	${OBJECTDIR}/scalar_akima.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/tests/base_test.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-march=native -O3 -ltbb
CXXFLAGS=-march=native -O3 -ltbb

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfastakimainterpol.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfastakimainterpol.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfastakimainterpol.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfastakimainterpol.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfastakimainterpol.a

${OBJECTDIR}/fast_akima.o: fast_akima.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/fast_akima.o fast_akima.cpp

${OBJECTDIR}/glucose/glucose_impl.o: glucose/glucose_impl.cpp 
	${MKDIR} -p ${OBJECTDIR}/glucose
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glucose/glucose_impl.o glucose/glucose_impl.cpp

${OBJECTDIR}/helpers.o: helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/helpers.o helpers.cpp

${OBJECTDIR}/interpolator.o: interpolator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/interpolator.o interpolator.cpp

${OBJECTDIR}/lib/glucose/CommonApprox.o: lib/glucose/CommonApprox.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/CommonApprox.o lib/glucose/CommonApprox.cpp

${OBJECTDIR}/lib/glucose/GlucoseLevels.o: lib/glucose/GlucoseLevels.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/GlucoseLevels.o lib/glucose/GlucoseLevels.cpp

${OBJECTDIR}/lib/glucose/iface/ApproxIface.o: lib/glucose/iface/ApproxIface.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose/iface
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/iface/ApproxIface.o lib/glucose/iface/ApproxIface.cpp

${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl.o: lib/glucose/rtl/LogicalClockImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose/rtl
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl.o lib/glucose/rtl/LogicalClockImpl.cpp

${OBJECTDIR}/lib/glucose/rtl/referencedImpl.o: lib/glucose/rtl/referencedImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose/rtl
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/rtl/referencedImpl.o lib/glucose/rtl/referencedImpl.cpp

${OBJECTDIR}/perf_test.o: perf_test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/perf_test.o perf_test.cpp

${OBJECTDIR}/scalar_akima.o: scalar_akima.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/scalar_akima.o scalar_akima.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/base_test.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/tests/base_test.o: tests/base_test.cpp 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} "$@.d"
	$(COMPILE.cc) -g -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/tests/base_test.o tests/base_test.cpp


${OBJECTDIR}/fast_akima_nomain.o: ${OBJECTDIR}/fast_akima.o fast_akima.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/fast_akima.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/fast_akima_nomain.o fast_akima.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/fast_akima.o ${OBJECTDIR}/fast_akima_nomain.o;\
	fi

${OBJECTDIR}/glucose/glucose_impl_nomain.o: ${OBJECTDIR}/glucose/glucose_impl.o glucose/glucose_impl.cpp 
	${MKDIR} -p ${OBJECTDIR}/glucose
	@NMOUTPUT=`${NM} ${OBJECTDIR}/glucose/glucose_impl.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/glucose/glucose_impl_nomain.o glucose/glucose_impl.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/glucose/glucose_impl.o ${OBJECTDIR}/glucose/glucose_impl_nomain.o;\
	fi

${OBJECTDIR}/helpers_nomain.o: ${OBJECTDIR}/helpers.o helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/helpers.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/helpers_nomain.o helpers.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/helpers.o ${OBJECTDIR}/helpers_nomain.o;\
	fi

${OBJECTDIR}/interpolator_nomain.o: ${OBJECTDIR}/interpolator.o interpolator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/interpolator.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/interpolator_nomain.o interpolator.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/interpolator.o ${OBJECTDIR}/interpolator_nomain.o;\
	fi

${OBJECTDIR}/lib/glucose/CommonApprox_nomain.o: ${OBJECTDIR}/lib/glucose/CommonApprox.o lib/glucose/CommonApprox.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/glucose/CommonApprox.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/CommonApprox_nomain.o lib/glucose/CommonApprox.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/glucose/CommonApprox.o ${OBJECTDIR}/lib/glucose/CommonApprox_nomain.o;\
	fi

${OBJECTDIR}/lib/glucose/GlucoseLevels_nomain.o: ${OBJECTDIR}/lib/glucose/GlucoseLevels.o lib/glucose/GlucoseLevels.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/glucose/GlucoseLevels.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/GlucoseLevels_nomain.o lib/glucose/GlucoseLevels.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/glucose/GlucoseLevels.o ${OBJECTDIR}/lib/glucose/GlucoseLevels_nomain.o;\
	fi

${OBJECTDIR}/lib/glucose/iface/ApproxIface_nomain.o: ${OBJECTDIR}/lib/glucose/iface/ApproxIface.o lib/glucose/iface/ApproxIface.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose/iface
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/glucose/iface/ApproxIface.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/iface/ApproxIface_nomain.o lib/glucose/iface/ApproxIface.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/glucose/iface/ApproxIface.o ${OBJECTDIR}/lib/glucose/iface/ApproxIface_nomain.o;\
	fi

${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl_nomain.o: ${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl.o lib/glucose/rtl/LogicalClockImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose/rtl
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl_nomain.o lib/glucose/rtl/LogicalClockImpl.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl.o ${OBJECTDIR}/lib/glucose/rtl/LogicalClockImpl_nomain.o;\
	fi

${OBJECTDIR}/lib/glucose/rtl/referencedImpl_nomain.o: ${OBJECTDIR}/lib/glucose/rtl/referencedImpl.o lib/glucose/rtl/referencedImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/lib/glucose/rtl
	@NMOUTPUT=`${NM} ${OBJECTDIR}/lib/glucose/rtl/referencedImpl.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/lib/glucose/rtl/referencedImpl_nomain.o lib/glucose/rtl/referencedImpl.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/lib/glucose/rtl/referencedImpl.o ${OBJECTDIR}/lib/glucose/rtl/referencedImpl_nomain.o;\
	fi

${OBJECTDIR}/perf_test_nomain.o: ${OBJECTDIR}/perf_test.o perf_test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/perf_test.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/perf_test_nomain.o perf_test.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/perf_test.o ${OBJECTDIR}/perf_test_nomain.o;\
	fi

${OBJECTDIR}/scalar_akima_nomain.o: ${OBJECTDIR}/scalar_akima.o scalar_akima.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/scalar_akima.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/scalar_akima_nomain.o scalar_akima.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/scalar_akima.o ${OBJECTDIR}/scalar_akima_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfastakimainterpol.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
