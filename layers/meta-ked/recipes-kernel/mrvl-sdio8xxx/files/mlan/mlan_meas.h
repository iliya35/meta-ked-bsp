/**
 *  @file mlan_meas.h
 *
 *  @brief Interface for the measurement module implemented in mlan_meas.c
 *
 *  Driver interface functions and type declarations for the measurement module
 *    implemented in mlan_meas.c
 *
 *  @sa mlan_meas.c
 *
 *
 *  Copyright 2014-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code (Materials) are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

/*************************************************************
Change Log:
    03/25/2009: initial version
************************************************************/

#ifndef _MLAN_MEAS_H_
#define _MLAN_MEAS_H_

#include  "mlan_fw.h"

/* Send a given measurement request to the firmware, report back the result */
extern int

wlan_meas_util_send_req(mlan_private *pmpriv,
			HostCmd_DS_MEASUREMENT_REQUEST *pmeas_req,
			t_u32 wait_for_resp_timeout, pmlan_ioctl_req pioctl_req,
			HostCmd_DS_MEASUREMENT_REPORT *pmeas_rpt);

/* Setup a measurement command before it is sent to the firmware */
extern int wlan_meas_cmd_process(mlan_private *pmpriv,
				 HostCmd_DS_COMMAND *pcmd_ptr,
				 const t_void *pinfo_buf);

/* Handle a given measurement command response from the firmware */
extern int wlan_meas_cmdresp_process(mlan_private *pmpriv,
				     const HostCmd_DS_COMMAND *resp);

#endif /* _MLAN_MEAS_H_ */
