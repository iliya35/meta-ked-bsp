# This is a class to place CI build information into the image
# 
# This class generates two files '/ci-buildinfo' and '/build.cienv' into the
# root filesystem with information about the CI build. If it is no CI build
# like a local developer build, default files are created which indicate that
# this build is not from CI and therefore not reproducible.
#
# The CI build is detected when a file 'tmp/deploy/*.cienv' exists. This file
# contains all CI environment variables. These variables are used to create the
# ci-buildinfo file.
#
# The 'tmp/deploy/*.cienv' represents the environment variables of a gitlab CI
# build. So this class works best with a gitlab CI pipeline.

# Generates the CI build info files
do_generate_ci_buildinfo() {
	local buildinfo_file=${IMAGE_ROOTFS}/etc/ci-buildinfo
	local buildenv_file=${IMAGE_ROOTFS}/etc/build.cienv
	local cienv_file=${PWD}/tmp/deploy/*.cienv

	local yocto_build_machine=${MACHINE}
	local yocto_build_image=${PN}

	mkdir -p ${IMAGE_ROOTFS}/etc

	if test ! -f ${cienv_file}; then
		bbnote "DEVELOPMENT build detected: Building with dummy CI buildinfo" 
		{
			echo "# This is a dummy file"
			echo "# It seems this software was not built by CI"
			echo "#####################################"
			echo "# This software is not reproducible!"
			echo "#####################################"
		} > ${buildenv_file}

		local ci_project_name="not available"
		local ci_project_url="${PWD}"
		local ci_commit_sha="not available"
		local ci_commit_ref_name="not available"
		local ci_pipeline_created_at="$(date)"
		local ci_pipeline_url="${PWD}"
		local ci_job_url="${PWD}:${MACHINE}:${PN}"
	else
		bbnote "CI build detected: Building with real CI buildinfo"
		cp ${cienv_file} ${buildenv_file}

		local ci_project_name=$(sed -n "s/CI_PROJECT_NAME=//p" ${cienv_file}) 
		local ci_project_url=$(sed -n "s/CI_PROJECT_URL=//p" ${cienv_file}) 
		local ci_commit_sha=$(sed -n "s/CI_COMMIT_SHA=//p" ${cienv_file}) 
		local ci_commit_ref_name=$(sed -n "s/CI_COMMIT_REF_NAME=//p" ${cienv_file}) 
		local ci_pipeline_created_at=$(sed -n "s/CI_PIPELINE_CREATED_AT=//p" ${cienv_file}) 
		local ci_pipeline_url=$(sed -n "s/CI_PIPELINE_URL=//p" ${cienv_file}) 
		local ci_job_url=$(sed -n "s/CI_JOB_URL=//p" ${cienv_file}) 
	fi

	{
		# create content file
		echo "Build configuration"
		echo "  Machine      : ${yocto_build_machine}"
		echo "  Image        : ${yocto_build_image}"
		echo ""
		echo "Source code identification"
		echo "  Project name : ${ci_project_name}"
		echo "  Project      : ${ci_project_url}"
		echo "  Commit       : ${ci_commit_sha}"
		echo "  Reference    : ${ci_commit_ref_name}"
		echo ""
		echo "Build identification"
		echo "  Date         : ${ci_pipeline_created_at}"
		echo "  Pipeline     : ${ci_pipeline_url}"
		echo "  Build job    : ${ci_job_url}"
	 } > ${buildinfo_file}

	echo "Generated build info"
	echo "--------------------------------------------------------"
	cat ${buildinfo_file}
	echo "--------------------------------------------------------"

}
addtask generate_ci_buildinfo after do_rootfs before do_image
