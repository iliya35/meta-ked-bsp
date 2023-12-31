/** @file bt_main.c
 *
 *  @brief This file contains the major functions in BlueTooth
 *  driver. It includes init, exit, open, close and main
 *  thread etc..
 *
 *
 *  Copyright 2014-2020 NXP
 *
 *  This software file (the File) is distributed by NXP
 *  under the terms of the GNU General Public License Version 2, June 1991
 *  (the License).  You may use, redistribute and/or modify the File in
 *  accordance with the terms and conditions of the License, a copy of which
 *  is available by writing to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 *  worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 *  THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 *  ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 *  this warranty disclaimer.
 *
 */
/**
  * @mainpage M-BT Linux Driver
  *
  * @section overview_sec Overview
  *
  * The M-BT is a Linux reference driver for NXP Bluetooth chipset.
  *
  * @section copyright_sec Copyright
  *
  * Copyright 2014-2020 NXP
  *
  */

#include <linux/module.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#endif

#include <linux/mmc/sdio_func.h>

#include "bt_drv.h"
#include "mbt_char.h"
#include "bt_sdio.h"

/** Version */
#define VERSION "C4X14114"

/** Driver version */
static char mbt_driver_version[] = "SD8977-%s-" VERSION "-(" "FP" FPNUM ")"
#ifdef DEBUG_LEVEL2
	"-dbg"
#endif
	" ";

/** Declare and initialize fw_version */
static char fw_version[32] = "0.0.0.p0";

#define AID_SYSTEM        1000	/* system server */

#define AID_BLUETOOTH     1002	/* bluetooth subsystem */

#define AID_NET_BT_STACK  3008	/* bluetooth stack */

/** Define module name */

#define MODULE_NAME  "bt_fm_nfc"

/** Declaration of chardev class */
static struct class *chardev_class;

/** Interface specific variables */

/**
 * The global variable of a pointer to bt_private
 * structure variable
 **/
bt_private *m_priv[MAX_BT_ADAPTER];

/** Default Driver mode */
static int drv_mode = (DRV_MODE_BT);

/** fw reload flag */
int bt_fw_reload;
/** fw serial download flag */
int bt_fw_serial = 1;

/** Firmware flag */
static int fw = 1;
/** default powermode */
static int psmode = 1;
/** default BLE deep sleep */
static int deep_sleep = 1;
/** init cmds file */
static char *init_cmds;
/** Init config file (MAC address, register etc.) */
static char *init_cfg;
/** Calibration config file (MAC address, init powe etc.) */
static char *cal_cfg;
/** Calibration config file EXT */
static char *cal_cfg_ext;
/** Init MAC address */
static char *bt_mac;
static int btindrst = -1;

/** Setting mbt_drvdbg value based on DEBUG level */
#ifdef DEBUG_LEVEL1
#ifdef DEBUG_LEVEL2
#define DEFAULT_DEBUG_MASK  (0xffffffff & ~DBG_EVENT)
#else
#define DEFAULT_DEBUG_MASK  (DBG_MSG | DBG_FATAL | DBG_ERROR)
#endif /* DEBUG_LEVEL2 */
u32 mbt_drvdbg = DEFAULT_DEBUG_MASK;
#endif

#ifdef CONFIG_OF
static int dts_enable = 1;
#endif

#ifdef SDIO_SUSPEND_RESUME
/** PM keep power */
int mbt_pm_keep_power = 1;
#endif

static int btpmic = 0;

/** Offset of sequence number in event */
#define OFFSET_SEQNUM 4

/**
 *  @brief handle received packet
 *  @param priv    A pointer to bt_private structure
 *  @param skb     A pointer to rx skb
 *
 *  @return        N/A
 */
void
bt_recv_frame(bt_private *priv, struct sk_buff *skb)
{
	struct hci_dev *hdev = NULL;
	if (priv->bt_dev.m_dev[BT_SEQ].spec_type == BLUEZ_SPEC)
		hdev = (struct hci_dev *)priv->bt_dev.m_dev[BT_SEQ].dev_pointer;
	if (hdev) {
		skb->dev = (void *)hdev;
		hdev->stat.byte_rx += skb->len;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
		hci_recv_frame(skb);
#else
		hci_recv_frame(hdev, skb);
#endif
	}
	return;
}

/**
 *  @brief Alloc bt device
 *
 *  @return    pointer to structure mbt_dev or NULL
 */
struct mbt_dev *
alloc_mbt_dev(void)
{
	struct mbt_dev *mbt_dev;
	ENTER();

	mbt_dev = kzalloc(sizeof(struct mbt_dev), GFP_KERNEL);
	if (!mbt_dev) {
		LEAVE();
		return NULL;
	}

	LEAVE();
	return mbt_dev;
}

/**
 *  @brief Frees m_dev
 *
 *  @return    N/A
 */
void
free_m_dev(struct m_dev *m_dev)
{
	ENTER();
	kfree(m_dev->dev_pointer);
	m_dev->dev_pointer = NULL;
	LEAVE();
}

/**
 *  @brief clean up m_devs
 *
 *  @return    N/A
 */
void
clean_up_m_devs(bt_private *priv)
{
	struct m_dev *m_dev = NULL;
	struct hci_dev *hdev = NULL;

	ENTER();
	if (priv->bt_dev.m_dev[BT_SEQ].dev_pointer) {
		m_dev = &(priv->bt_dev.m_dev[BT_SEQ]);
		PRINTM(MSG, "BT: Delete %s\n", m_dev->name);
		if (m_dev->spec_type == BLUEZ_SPEC) {
			hdev = (struct hci_dev *)m_dev->dev_pointer;
			/** check if dev->name has been assigned */
			if (strstr(hdev->name, "hci"))
				hci_unregister_dev(hdev);
			hci_free_dev(hdev);
		}
		priv->bt_dev.m_dev[BT_SEQ].dev_pointer = NULL;
	}
	LEAVE();
	return;
}

/**
 *  @brief This function verify the received event pkt
 *
 *  Event format:
 *  +--------+--------+--------+--------+--------+
 *  | Event  | Length |  ncmd  |      Opcode     |
 *  +--------+--------+--------+--------+--------+
 *  | 1-byte | 1-byte | 1-byte |      2-byte     |
 *  +--------+--------+--------+--------+--------+
 *
 *  @param priv    A pointer to bt_private structure
 *  @param skb     A pointer to rx skb
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
check_evtpkt(bt_private *priv, struct sk_buff *skb)
{
	struct hci_event_hdr *hdr = (struct hci_event_hdr *)skb->data;
	struct hci_ev_cmd_complete *ec;
	u16 opcode, ocf;
	int ret = BT_STATUS_SUCCESS;
	ENTER();
	if (!priv->bt_dev.sendcmdflag) {
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	if (hdr->evt == HCI_EV_CMD_COMPLETE) {
		ec = (struct hci_ev_cmd_complete *)
			(skb->data + HCI_EVENT_HDR_SIZE);
		opcode = __le16_to_cpu(ec->opcode);
		ocf = hci_opcode_ocf(opcode);
		PRINTM(CMD,
		       "BT: CMD_COMPLTE opcode=0x%x, ocf=0x%x, send_cmd_opcode=0x%x\n",
		       opcode, ocf, priv->bt_dev.send_cmd_opcode);
		if (opcode != priv->bt_dev.send_cmd_opcode) {
			ret = BT_STATUS_FAILURE;
			goto exit;
		}
		switch (ocf) {
		case BT_CMD_MODULE_CFG_REQ:
		case BT_CMD_BLE_DEEP_SLEEP:
		case BT_CMD_CONFIG_MAC_ADDR:
		case BT_CMD_CSU_WRITE_REG:
		case BT_CMD_LOAD_CONFIG_DATA:
		case BT_CMD_LOAD_CONFIG_DATA_EXT:
		case BT_CMD_AUTO_SLEEP_MODE:
		case BT_CMD_HOST_SLEEP_CONFIG:
		case BT_CMD_SDIO_PULL_CFG_REQ:
		case BT_CMD_SET_EVT_FILTER:
			// case BT_CMD_ENABLE_DEVICE_TESTMODE:
		case BT_CMD_PMIC_CONFIGURE:
		case BT_CMD_INDEPENDENT_RESET:
			priv->bt_dev.sendcmdflag = FALSE;
			priv->adapter->cmd_complete = TRUE;
			wake_up_interruptible(&priv->adapter->cmd_wait_q);
			break;
		case BT_CMD_GET_FW_VERSION:
			{
				u8 *pos = (skb->data + HCI_EVENT_HDR_SIZE +
					   sizeof(struct hci_ev_cmd_complete) +
					   1);
				snprintf(fw_version, sizeof(fw_version),
					 "%u.%u.%u.p%u", pos[2], pos[1], pos[0],
					 pos[3]);
				priv->bt_dev.sendcmdflag = FALSE;
				priv->adapter->cmd_complete = TRUE;
				wake_up_interruptible(&priv->adapter->
						      cmd_wait_q);
				break;
			}
		case BT_CMD_HISTOGRAM:
			{
				u8 *status =
					skb->data + HCI_EVENT_HDR_SIZE +
					sizeof(struct hci_ev_cmd_complete);
				u8 *pos =
					(skb->data + HCI_EVENT_HDR_SIZE +
					 sizeof(struct hci_ev_cmd_complete) +
					 1);
				if (*status == 0) {
					priv->hist_data_len =
						hdr->plen -
						sizeof(struct
						       hci_ev_cmd_complete) - 1;
					if (priv->hist_data_len >
					    sizeof(priv->hist_data))
						priv->hist_data_len =
							sizeof(priv->hist_data);
					memcpy(priv->hist_data, pos,
					       priv->hist_data_len);
					PRINTM(CMD, "histogram len=%d\n",
					       priv->hist_data_len);
				}
				priv->bt_dev.sendcmdflag = FALSE;
				priv->adapter->cmd_complete = TRUE;
				wake_up_interruptible(&priv->adapter->
						      cmd_wait_q);
				break;
			}
		case BT_CMD_RESET:
		case BT_CMD_ENABLE_WRITE_SCAN:
			{
				priv->bt_dev.sendcmdflag = FALSE;
				priv->adapter->cmd_complete = TRUE;
				if (priv->adapter->wait_event_timeout == TRUE) {
					wake_up(&priv->adapter->cmd_wait_q);
					priv->adapter->wait_event_timeout =
						FALSE;
				} else
					wake_up_interruptible(&priv->adapter->
							      cmd_wait_q);
			}
			break;
		case BT_CMD_HOST_SLEEP_ENABLE:
			priv->bt_dev.sendcmdflag = FALSE;
			break;
		default:
			/** Ignore command not defined but send by driver */
			if (opcode == priv->bt_dev.send_cmd_opcode) {
				priv->bt_dev.sendcmdflag = FALSE;
				priv->adapter->cmd_complete = TRUE;
				wake_up_interruptible(&priv->adapter->
						      cmd_wait_q);
			} else {
				ret = BT_STATUS_FAILURE;
			}
			break;
		}
	} else
		ret = BT_STATUS_FAILURE;
exit:
	if (ret == BT_STATUS_SUCCESS)
		kfree_skb(skb);
	LEAVE();
	return ret;
}

/**
*  @brief This function stores the FW dumps received from events
*
*  @param priv    A pointer to bt_private structure
*  @param skb     A pointer to rx skb
*
*  @return        N/A
*/
void
bt_store_firmware_dump(bt_private *priv, u8 *buf, u32 len)
{
	struct file *pfile_fwdump = NULL;
	loff_t pos = 0;
	u16 seqnum = 0;
	struct timeval t;
	u32 sec;

	ENTER();

	seqnum = __le16_to_cpu(*(u16 *) (buf + OFFSET_SEQNUM));

	if (priv->adapter->fwdump_fname && seqnum != 1) {
		pfile_fwdump =
			filp_open((const char *)priv->adapter->fwdump_fname,
				  O_CREAT | O_WRONLY | O_APPEND, 0644);
		if (IS_ERR(pfile_fwdump)) {
			PRINTM(MSG, "Cannot create firmware dump file.\n");
			LEAVE();
			return;
		}
	} else {
		if (!priv->adapter->fwdump_fname) {
			gfp_t flag;
			flag = (in_atomic() ||
				irqs_disabled())? GFP_ATOMIC : GFP_KERNEL;
			priv->adapter->fwdump_fname = kzalloc(64, flag);
		} else
			memset(priv->adapter->fwdump_fname, 0, 64);

		get_monotonic_time(&t);
		sec = (u32)t.tv_sec;
		sprintf(priv->adapter->fwdump_fname, "%s%u",
			"/var/log/bt_fwdump_", sec);
		pfile_fwdump =
			filp_open(priv->adapter->fwdump_fname,
				  O_CREAT | O_WRONLY | O_APPEND, 0644);
		if (IS_ERR(pfile_fwdump)) {
			sprintf(priv->adapter->fwdump_fname, "%s%u",
				"/data/bt_fwdump_", sec);
			pfile_fwdump =
				filp_open((const char *)priv->adapter->
					  fwdump_fname,
					  O_CREAT | O_WRONLY | O_APPEND, 0644);
		}
	}

	if (IS_ERR(pfile_fwdump)) {
		PRINTM(MSG, "Cannot create firmware dump file\n");
		LEAVE();
		return;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
	vfs_write(pfile_fwdump, buf, len, &pos);
#else
	kernel_write(pfile_fwdump, buf, len, &pos);
#endif
	filp_close(pfile_fwdump, NULL);
	LEAVE();
	return;
}

/**
 *  @brief This function process the received event
 *
 *  Event format:
 *  +--------+--------+--------+--------+-----+
 *  |   EC   | Length |           Data        |
 *  +--------+--------+--------+--------+-----+
 *  | 1-byte | 1-byte |          n-byte       |
 *  +--------+--------+--------+--------+-----+
 *
 *  @param priv    A pointer to bt_private structure
 *  @param skb     A pointer to rx skb
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_process_event(bt_private *priv, struct sk_buff *skb)
{
	int ret = BT_STATUS_SUCCESS;
#ifdef DEBUG_LEVEL1
	struct m_dev *m_dev = &(priv->bt_dev.m_dev[BT_SEQ]);
#endif
	BT_EVENT *pevent;

	ENTER();
	if (!m_dev) {
		PRINTM(CMD, "BT: bt_process_event without m_dev\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pevent = (BT_EVENT *)skb->data;
	if (pevent->EC != 0xff) {
		PRINTM(CMD, "BT: Not NXP Event=0x%x\n", pevent->EC);
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	switch (pevent->data[0]) {
	case BT_CMD_HISTOGRAM:
		break;
	case BT_CMD_AUTO_SLEEP_MODE:
		if (pevent->data[2] == BT_STATUS_SUCCESS) {
			if (pevent->data[1] == BT_PS_ENABLE)
				priv->adapter->psmode = 1;
			else
				priv->adapter->psmode = 0;
			PRINTM(CMD, "BT: PS Mode %s:%s\n", m_dev->name,
			       (priv->adapter->psmode) ? "Enable" : "Disable");

		} else {
			PRINTM(CMD, "BT: PS Mode Command Fail %s\n",
			       m_dev->name);
		}
		break;
	case BT_CMD_HOST_SLEEP_CONFIG:
		if (pevent->data[3] == BT_STATUS_SUCCESS) {
			PRINTM(CMD, "BT: %s: gpio=0x%x, gap=0x%x\n",
			       m_dev->name, pevent->data[1], pevent->data[2]);
		} else {
			PRINTM(CMD, "BT: %s: HSCFG Command Fail\n",
			       m_dev->name);
		}
		break;
	case BT_CMD_HOST_SLEEP_ENABLE:
		if (pevent->data[1] == BT_STATUS_SUCCESS) {
			priv->adapter->hs_state = HS_ACTIVATED;
			if (priv->adapter->suspend_fail == FALSE) {
#ifdef SDIO_SUSPEND_RESUME
#ifdef MMC_PM_KEEP_POWER
#ifdef MMC_PM_FUNC_SUSPENDED
				bt_is_suspended(priv);
#endif
#endif
#endif
				if (priv->adapter->wait_event_timeout) {
					wake_up(&priv->adapter->cmd_wait_q);
					priv->adapter->wait_event_timeout =
						FALSE;
				} else
					wake_up_interruptible(&priv->adapter->
							      cmd_wait_q);

			}
			if (priv->adapter->psmode)
				priv->adapter->ps_state = PS_SLEEP;
			PRINTM(CMD, "BT: EVENT %s: HS ACTIVATED!\n",
			       m_dev->name);

		} else {
			PRINTM(CMD, "BT: %s: HS Enable Fail\n", m_dev->name);
		}
		break;
	case BT_CMD_MODULE_CFG_REQ:
		if ((priv->bt_dev.sendcmdflag == TRUE) &&
		    ((pevent->data[1] == MODULE_BRINGUP_REQ)
		     || (pevent->data[1] == MODULE_SHUTDOWN_REQ))) {
			if (pevent->data[1] == MODULE_BRINGUP_REQ) {
				PRINTM(CMD, "BT: EVENT %s:%s\n", m_dev->name,
				       (pevent->data[2] && (pevent->data[2] !=
							    MODULE_CFG_RESP_ALREADY_UP))
				       ? "Bring up Fail" : "Bring up success");
				priv->bt_dev.devType = pevent->data[3];
				PRINTM(CMD, "devType:%s\n",
				       (pevent->data[3] ==
					DEV_TYPE_AMP) ? "AMP controller" :
				       "BR/EDR controller");
				priv->bt_dev.devFeature = pevent->data[4];
				PRINTM(CMD, "devFeature:  %s,    %s,    %s"
				       "\n",
				       ((pevent->
					 data[4] & DEV_FEATURE_BT) ?
					"BT Feature" : "No BT Feature"),
				       ((pevent->
					 data[4] & DEV_FEATURE_BTAMP) ?
					"BTAMP Feature" : "No BTAMP Feature"),
				       ((pevent->
					 data[4] & DEV_FEATURE_BLE) ?
					"BLE Feature" : "No BLE Feature")
					);
			}
			if (pevent->data[1] == MODULE_SHUTDOWN_REQ) {
				PRINTM(CMD, "BT: EVENT %s:%s\n", m_dev->name,
				       (pevent->data[2]) ? "Shut down Fail"
				       : "Shut down success");

			}
			if (pevent->data[2]) {
				priv->bt_dev.sendcmdflag = FALSE;
				priv->adapter->cmd_complete = TRUE;
				wake_up_interruptible(&priv->adapter->
						      cmd_wait_q);
			}
		} else {
			PRINTM(CMD, "BT_CMD_MODULE_CFG_REQ resp for APP\n");
			ret = BT_STATUS_FAILURE;
		}
		break;
	case BT_EVENT_POWER_STATE:
		if (pevent->data[1] == BT_PS_SLEEP)
			priv->adapter->ps_state = PS_SLEEP;
		PRINTM(CMD, "BT: EVENT %s:%s\n", m_dev->name,
		       (priv->adapter->ps_state) ? "PS_SLEEP" : "PS_AWAKE");

		break;
	case BT_CMD_SDIO_PULL_CFG_REQ:
		if (pevent->data[pevent->length - 1] == BT_STATUS_SUCCESS)
			PRINTM(CMD, "BT: %s: SDIO pull configuration success\n",
			       m_dev->name);

		else {
			PRINTM(CMD, "BT: %s: SDIO pull configuration fail\n",
			       m_dev->name);

		}
		break;
	default:
		PRINTM(CMD, "BT: Unknown Event=%d %s\n", pevent->data[0],
		       m_dev->name);
		ret = BT_STATUS_FAILURE;
		break;
	}
exit:
	if (ret == BT_STATUS_SUCCESS)
		kfree_skb(skb);
	LEAVE();
	return ret;
}

/**
 *  @brief This function save the dump info to file
 *
 *  @param dir_name     directory name
 *  @param file_name    file_name
 *  @param buf			buffer
 *  @param buf_len		buffer length
 *
 *  @return   		    0 --success otherwise fail
 */
int
bt_save_dump_info_to_file(char *dir_name, char *file_name, u8 *buf, u32 buf_len)
{
	int ret = BT_STATUS_SUCCESS;
	struct file *pfile = NULL;
	u8 name[64];
	loff_t pos;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
	mm_segment_t fs;
#endif

	ENTER();

	if (!dir_name || !file_name || !buf) {
		PRINTM(ERROR, "Can't save dump info to file\n");
		ret = BT_STATUS_FAILURE;
		goto done;
	}

	memset(name, 0, sizeof(name));
	snprintf((char *)name, sizeof(name), "%s/%s", dir_name, file_name);
	pfile = filp_open((const char *)name, O_CREAT | O_RDWR, 0644);
	if (IS_ERR(pfile)) {
		PRINTM(MSG,
		       "Create file %s error, try to save dump file in /var\n",
		       name);
		memset(name, 0, sizeof(name));
		snprintf((char *)name, sizeof(name), "%s/%s", "/var",
			 file_name);
		pfile = filp_open((const char *)name, O_CREAT | O_RDWR, 0644);
	}
	if (IS_ERR(pfile)) {
		PRINTM(ERROR, "Create Dump file for %s error\n", name);
		ret = BT_STATUS_FAILURE;
		goto done;
	}

	PRINTM(MSG, "Dump data %s saved in %s\n", file_name, name);

	pos = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
	fs = get_fs();
	set_fs(KERNEL_DS);
	vfs_write(pfile, (const char __user *)buf, buf_len, &pos);
	set_fs(fs);
#else
	kernel_write(pfile, (const char __user *)buf, buf_len, &pos);
#endif
	filp_close(pfile, NULL);

	PRINTM(MSG, "Dump data %s saved in %s successfully\n", file_name, name);

done:
	LEAVE();
	return ret;
}

#define DEBUG_HOST_READY		0xEE
#define DEBUG_FW_DONE			0xFF
#define DUMP_MAX_POLL_TRIES			200

#define DEBUG_DUMP_CTRL_REG               0xF0
#define DEBUG_DUMP_START_REG              0xF1
#define DEBUG_DUMP_END_REG                0xF8

typedef enum {
	DUMP_TYPE_ITCM = 0,
	DUMP_TYPE_DTCM = 1,
	DUMP_TYPE_SQRAM = 2,
	DUMP_TYPE_APU_REGS = 3,
	DUMP_TYPE_CIU_REGS = 4,
	DUMP_TYPE_ICU_REGS = 5,
	DUMP_TYPE_MAC_REGS = 6,
	DUMP_TYPE_EXTEND_7 = 7,
	DUMP_TYPE_EXTEND_8 = 8,
	DUMP_TYPE_EXTEND_9 = 9,
	DUMP_TYPE_EXTEND_10 = 10,
	DUMP_TYPE_EXTEND_11 = 11,
	DUMP_TYPE_EXTEND_12 = 12,
	DUMP_TYPE_EXTEND_13 = 13,
	DUMP_TYPE_EXTEND_LAST = 14
} dumped_mem_type;

#define MAX_NAME_LEN               8
#define MAX_FULL_NAME_LEN               32

/** Memory type mapping structure */
typedef struct {
	/** memory name */
	u8 mem_name[MAX_NAME_LEN];
	/** memory pointer */
	u8 *mem_Ptr;
	/** file structure */
	struct file *pfile_mem;
	/** donbe flag */
	u8 done_flag;
	/** dump type */
	u8 type;
} memory_type_mapping;

memory_type_mapping bt_mem_type_mapping_tbl[] = {
	{"ITCM", NULL, NULL, 0xF0, FW_DUMP_TYPE_MEM_ITCM},
	{"DTCM", NULL, NULL, 0xF1, FW_DUMP_TYPE_MEM_DTCM},
	{"SQRAM", NULL, NULL, 0xF2, FW_DUMP_TYPE_MEM_SQRAM},
	{"APU", NULL, NULL, 0xF3, FW_DUMP_TYPE_REG_APU},
	{"CIU", NULL, NULL, 0xF4, FW_DUMP_TYPE_REG_CIU},
	{"ICU", NULL, NULL, 0xF5, FW_DUMP_TYPE_REG_ICU},
	{"MAC", NULL, NULL, 0xF6, FW_DUMP_TYPE_REG_MAC},
	{"EXT7", NULL, NULL, 0xF7},
	{"EXT8", NULL, NULL, 0xF8},
	{"EXT9", NULL, NULL, 0xF9},
	{"EXT10", NULL, NULL, 0xFA},
	{"EXT11", NULL, NULL, 0xFB},
	{"EXT12", NULL, NULL, 0xFC},
	{"EXT13", NULL, NULL, 0xFD},
	{"EXTLAST", NULL, NULL, 0xFE},
};

typedef enum {
	RDWR_STATUS_SUCCESS = 0,
	RDWR_STATUS_FAILURE = 1,
	RDWR_STATUS_DONE = 2
} rdwr_status;

/**
 *  @brief This function read/write firmware via cmd52
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return         MLAN_STATUS_SUCCESS
 */
rdwr_status
bt_cmd52_rdwr_firmware(bt_private *priv, u8 doneflag)
{
	int ret = 0;
	int tries = 0;
	u8 ctrl_data = 0;
	u8 dbg_dump_ctrl_reg = 0;

	dbg_dump_ctrl_reg = DEBUG_DUMP_CTRL_REG;

	ret = sd_write_reg(priv, dbg_dump_ctrl_reg, DEBUG_HOST_READY);
	if (ret) {
		PRINTM(ERROR, "SDIO Write ERR\n");
		return RDWR_STATUS_FAILURE;
	}
	for (tries = 0; tries < DUMP_MAX_POLL_TRIES; tries++) {
		ret = sd_read_reg(priv, dbg_dump_ctrl_reg, &ctrl_data);
		if (ret) {
			PRINTM(ERROR, "SDIO READ ERR\n");
			return RDWR_STATUS_FAILURE;
		}
		if (ctrl_data == DEBUG_FW_DONE)
			break;
		if (doneflag && ctrl_data == doneflag)
			return RDWR_STATUS_DONE;
		if (ctrl_data != DEBUG_HOST_READY) {
			PRINTM(INFO,
			       "The ctrl reg was changed, re-try again!\n");
			ret = sd_write_reg(priv, dbg_dump_ctrl_reg,
					   DEBUG_HOST_READY);
			if (ret) {
				PRINTM(ERROR, "SDIO Write ERR\n");
				return RDWR_STATUS_FAILURE;
			}
		}
		udelay(100);
	}
	if (ctrl_data == DEBUG_HOST_READY) {
		PRINTM(ERROR, "Fail to pull ctrl_data\n");
		return RDWR_STATUS_FAILURE;
	}

	return RDWR_STATUS_SUCCESS;
}

/**
 *  @brief This function dump firmware memory to file
 *
 *  @param phandle   A pointer to moal_handle
 *
 *  @return         N/A
 */
void
bt_dump_firmware_info_v2(bt_private *priv)
{
	int ret = 0;
	unsigned int reg, reg_start, reg_end;
	u8 *dbg_ptr = NULL;
	u8 dump_num = 0;
	u8 idx = 0;
	u8 doneflag = 0;
	rdwr_status stat;
	u8 i = 0;
	u8 read_reg = 0;
	u32 memory_size = 0;
	u8 path_name[64], file_name[32];
	u8 *end_ptr = NULL;
	u8 dbg_dump_start_reg = 0;
	u8 dbg_dump_end_reg = 0;

	if (!priv) {
		PRINTM(ERROR, "Could not dump firmwware info\n");
		return;
	}

	memset(path_name, 0, sizeof(path_name));
	strcpy((char *)path_name, "/data");
	PRINTM(MSG, "Create DUMP directory success:dir_name=%s\n", path_name);

	dbg_dump_start_reg = DEBUG_DUMP_START_REG;
	dbg_dump_end_reg = DEBUG_DUMP_END_REG;

	sbi_wakeup_firmware(priv);
	priv->fw_dump = TRUE;
	/* start dump fw memory */
	PRINTM(MSG, "==== DEBUG MODE OUTPUT START ====\n");
	/* read the number of the memories which will dump */
	if (RDWR_STATUS_FAILURE == bt_cmd52_rdwr_firmware(priv, doneflag))
		goto done;
	reg = dbg_dump_start_reg;
	ret = sd_read_reg(priv, reg, &dump_num);
	if (ret) {
		PRINTM(MSG, "SDIO READ MEM NUM ERR\n");
		goto done;
	}
	if (dump_num >
	    (sizeof(bt_mem_type_mapping_tbl) / sizeof(memory_type_mapping))) {
		PRINTM(MSG, "Invalid dump_num=%d\n", dump_num);
		return;
	}

	/* read the length of every memory which will dump */
	for (idx = 0; idx < dump_num; idx++) {
		if (RDWR_STATUS_FAILURE ==
		    bt_cmd52_rdwr_firmware(priv, doneflag))
			goto done;
		memory_size = 0;
		reg = dbg_dump_start_reg;
		for (i = 0; i < 4; i++) {
			ret = sd_read_reg(priv, reg, &read_reg);
			if (ret) {
				PRINTM(MSG, "SDIO READ ERR\n");
				goto done;
			}
			memory_size |= (read_reg << i * 8);
			reg++;
		}
		if (memory_size == 0) {
			PRINTM(MSG, "Firmware Dump Finished!\n");
			break;
		} else {
			PRINTM(MSG, "%s_SIZE=0x%x\n",
			       bt_mem_type_mapping_tbl[idx].mem_name,
			       memory_size);
			bt_mem_type_mapping_tbl[idx].mem_Ptr =
				vmalloc(memory_size + 1);
			if ((ret != BT_STATUS_SUCCESS) ||
			    !bt_mem_type_mapping_tbl[idx].mem_Ptr) {
				PRINTM(ERROR,
				       "Error: vmalloc %s buffer failed!!!\n",
				       bt_mem_type_mapping_tbl[idx].mem_name);
				goto done;
			}
			dbg_ptr = bt_mem_type_mapping_tbl[idx].mem_Ptr;
			end_ptr = dbg_ptr + memory_size;
		}
		doneflag = bt_mem_type_mapping_tbl[idx].done_flag;
		PRINTM(MSG, "Start %s output, please wait...\n",
		       bt_mem_type_mapping_tbl[idx].mem_name);
		do {
			stat = bt_cmd52_rdwr_firmware(priv, doneflag);
			if (RDWR_STATUS_FAILURE == stat)
				goto done;

			reg_start = dbg_dump_start_reg;
			reg_end = dbg_dump_end_reg;
			for (reg = reg_start; reg <= reg_end; reg++) {
				ret = sd_read_reg(priv, reg, dbg_ptr);
				if (ret) {
					PRINTM(MSG, "SDIO READ ERR\n");
					goto done;
				}
				if (dbg_ptr < end_ptr)
					dbg_ptr++;
				else
					PRINTM(MSG,
					       "pre-allocced buf is not enough\n");
			}
			if (RDWR_STATUS_DONE == stat) {
				PRINTM(MSG, "%s done:"
				       "size = 0x%x\n",
				       bt_mem_type_mapping_tbl[idx].mem_name,
				       (unsigned int)(dbg_ptr -
						      bt_mem_type_mapping_tbl
						      [idx].mem_Ptr));
				memset(file_name, 0, sizeof(file_name));
				snprintf((char *)file_name, sizeof(file_name),
					 "%s%s", "file_bt_",
					 bt_mem_type_mapping_tbl[idx].mem_name);
				if (BT_STATUS_SUCCESS !=
				    bt_save_dump_info_to_file((char *)path_name,
							      (char *)file_name,
							      bt_mem_type_mapping_tbl
							      [idx].mem_Ptr,
							      memory_size))
					PRINTM(MSG,
					       "Can't save dump file %s in %s\n",
					       file_name, path_name);
				vfree(bt_mem_type_mapping_tbl[idx].mem_Ptr);
				bt_mem_type_mapping_tbl[idx].mem_Ptr = NULL;
				break;
			}
		} while (1);
	}
	PRINTM(MSG, "==== DEBUG MODE OUTPUT END ====\n");
	/* end dump fw memory */
done:
	priv->fw_dump = FALSE;
	for (idx = 0; idx < dump_num; idx++) {
		if (bt_mem_type_mapping_tbl[idx].mem_Ptr) {
			vfree(bt_mem_type_mapping_tbl[idx].mem_Ptr);
			bt_mem_type_mapping_tbl[idx].mem_Ptr = NULL;
		}
	}
	PRINTM(MSG, "==== DEBUG MODE END ====\n");
	return;
}

/**
 *  @brief This function shows debug info for timeout of command sending.
 *
 *  @param adapter  A pointer to bt_private
 *  @param cmd      Timeout command id
 *
 *  @return         N/A
 */
static void
bt_cmd_timeout_func(bt_private *priv, u16 cmd)
{
	bt_adapter *adapter = priv->adapter;
	ENTER();

	adapter->num_cmd_timeout++;

	PRINTM(ERROR, "Version = %s\n", adapter->drv_ver);
	PRINTM(ERROR, "Timeout Command id = 0x%x\n", cmd);
	PRINTM(ERROR, "Number of command timeout = %d\n",
	       adapter->num_cmd_timeout);
	PRINTM(ERROR, "Interrupt counter = %d\n", adapter->IntCounter);
	PRINTM(ERROR, "Power Save mode = %d\n", adapter->psmode);
	PRINTM(ERROR, "Power Save state = %d\n", adapter->ps_state);
	PRINTM(ERROR, "Host Sleep state = %d\n", adapter->hs_state);
	PRINTM(ERROR, "hs skip count = %d\n", adapter->hs_skip);
	PRINTM(ERROR, "suspend_fail flag = %d\n", adapter->suspend_fail);
	PRINTM(ERROR, "suspended flag = %d\n", adapter->is_suspended);
	PRINTM(ERROR, "Number of wakeup tries = %d\n", adapter->WakeupTries);
	PRINTM(ERROR, "Host Cmd complet state = %d\n", adapter->cmd_complete);
	PRINTM(ERROR, "Last irq recv = %d\n", adapter->irq_recv);
	PRINTM(ERROR, "Last irq processed = %d\n", adapter->irq_done);
	PRINTM(ERROR, "tx pending = %d\n", adapter->skb_pending);
	PRINTM(ERROR, "sdio int status = %d\n", adapter->sd_ireg);
	bt_dump_sdio_regs(priv);
	LEAVE();
}

/**
 *  @brief This function queue frame
 *
 *  @param priv    A pointer to bt_private structure
 *  @param skb     A pointer to sk_buff structure
 *
 *  @return    N/A
 */
static void
bt_queue_frame(bt_private *priv, struct sk_buff *skb)
{
	skb_queue_tail(&priv->adapter->tx_queue, skb);
}

/**
 *  @brief This function send reset cmd to firmware
 *
 *  @param priv    A pointer to bt_private structure
 *
 *  @return	       BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_send_reset_command(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_HCI_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_HCI_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_HCI_CMD *)skb->data;
	pcmd->ocf_ogf = __cpu_to_le16((RESET_OGF << 10) | BT_CMD_RESET);
	pcmd->length = 0x00;
	pcmd->cmd_type = 0x00;
	bt_cb(skb)->pkt_type = HCI_COMMAND_PKT;
	skb_put(skb, 3);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	PRINTM(CMD, "Queue Reset Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Reset timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_RESET);
	} else {
		PRINTM(CMD, "BT: Reset Command done\n");
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function sends module cfg cmd to firmware
 *
 *  Command format:
 *  +--------+--------+--------+--------+--------+--------+--------+
 *  |     OCF OGF     | Length |                Data               |
 *  +--------+--------+--------+--------+--------+--------+--------+
 *  |     2-byte      | 1-byte |               4-byte              |
 *  +--------+--------+--------+--------+--------+--------+--------+
 *
 *  @param priv    A pointer to bt_private structure
 *  @param subcmd  sub command
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_send_module_cfg_cmd(bt_private *priv, int subcmd)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "BT: No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_MODULE_CFG_REQ);
	pcmd->length = 1;
	pcmd->data[0] = subcmd;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	PRINTM(CMD, "Queue module cfg Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	wake_up_interruptible(&priv->MainThread.waitQ);
	/*
	   On some Android platforms certain delay is needed for HCI daemon to
	   remove this module and close itself gracefully. Otherwise it hangs.
	   This 10ms delay is a workaround for such platforms as the root cause
	   has not been found yet. */
	mdelay(10);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: module_cfg_cmd(%#x): timeout sendcmdflag=%d\n",
		       subcmd, priv->bt_dev.sendcmdflag);
		bt_cmd_timeout_func(priv, BT_CMD_MODULE_CFG_REQ);
	} else {
		PRINTM(CMD, "BT: module cfg Command done\n");
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function to get histogram
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_get_histogram(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf = __cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_HISTOGRAM);
	pcmd->length = 0;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue Histogram cmd(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	priv->hist_data_len = 0;
	memset(priv->hist_data, 0, sizeof(priv->hist_data));
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: histogram timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_HISTOGRAM);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function enables power save mode
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_enable_ps(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_AUTO_SLEEP_MODE);
	if (priv->bt_dev.psmode)
		pcmd->data[0] = BT_PS_ENABLE;
	else
		pcmd->data[0] = BT_PS_DISABLE;
	if (priv->bt_dev.idle_timeout) {
		pcmd->length = 3;
		pcmd->data[1] = (u8)(priv->bt_dev.idle_timeout & 0x00ff);
		pcmd->data[2] = (priv->bt_dev.idle_timeout & 0xff00) >> 8;
	} else {
		pcmd->length = 1;
	}
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue PSMODE Command(0x%x):%d\n",
	       __le16_to_cpu(pcmd->ocf_ogf), pcmd->data[0]);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: psmode timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_AUTO_SLEEP_MODE);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function sends hscfg command
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_send_hscfg_cmd(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_HOST_SLEEP_CONFIG);
	pcmd->length = 2;
	pcmd->data[0] = (priv->bt_dev.gpio_gap & 0xff00) >> 8;
	pcmd->data[1] = (u8)(priv->bt_dev.gpio_gap & 0x00ff);
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue HSCFG Command(0x%x),gpio=0x%x,gap=0x%x\n",
	       __le16_to_cpu(pcmd->ocf_ogf), pcmd->data[0], pcmd->data[1]);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: HSCFG timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_HOST_SLEEP_CONFIG);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function sends sdio pull ctrl command
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_send_sdio_pull_ctrl_cmd(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_SDIO_PULL_CFG_REQ);
	pcmd->length = 4;
	pcmd->data[0] = (priv->bt_dev.sdio_pull_cfg & 0x000000ff);
	pcmd->data[1] = (priv->bt_dev.sdio_pull_cfg & 0x0000ff00) >> 8;
	pcmd->data[2] = (priv->bt_dev.sdio_pull_cfg & 0x00ff0000) >> 16;
	pcmd->data[3] = (priv->bt_dev.sdio_pull_cfg & 0xff000000) >> 24;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD,
	       "Queue SDIO PULL CFG Command(0x%x), PullUp=0x%x%x,PullDown=0x%x%x\n",
	       __le16_to_cpu(pcmd->ocf_ogf), pcmd->data[1], pcmd->data[0],
	       pcmd->data[3], pcmd->data[2]);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: SDIO PULL CFG timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_SDIO_PULL_CFG_REQ);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function sends command to configure PMIC
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_pmic_configure(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_PMIC_CONFIGURE);
	pcmd->length = 0;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue PMIC Configure Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: PMIC Configure timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_PMIC_CONFIGURE);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function enables host sleep
 *
 *  @param priv    A pointer to bt_private structure
 *  @param is_shutdown  indicate shutdown mode
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_enable_hs(bt_private *priv, bool is_shutdown)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	priv->adapter->suspend_fail = FALSE;
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_HOST_SLEEP_ENABLE);
	pcmd->length = 0;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->adapter->wait_event_timeout = is_shutdown;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	PRINTM(CMD, "Queue hs enable Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (is_shutdown) {
		if (!os_wait_timeout
		    (priv->adapter->cmd_wait_q, priv->adapter->hs_state,
		     WAIT_UNTIL_HS_STATE_CHANGED)) {
			PRINTM(MSG, "BT: Enable host sleep timeout:\n");
			priv->adapter->wait_event_timeout = FALSE;
			bt_cmd_timeout_func(priv, BT_CMD_HOST_SLEEP_ENABLE);
		}
	} else {
		if (!os_wait_interruptible_timeout
		    (priv->adapter->cmd_wait_q, priv->adapter->hs_state,
		     WAIT_UNTIL_HS_STATE_CHANGED)) {
			PRINTM(MSG, "BT: Enable host sleep timeout:\n");
			bt_cmd_timeout_func(priv, BT_CMD_HOST_SLEEP_ENABLE);
		}
	}
	OS_INT_DISABLE;
	if ((priv->adapter->hs_state == HS_ACTIVATED) ||
	    (priv->adapter->is_suspended == TRUE)) {
		OS_INT_RESTORE;
		PRINTM(MSG, "BT: suspend success! skip=%d\n",
		       priv->adapter->hs_skip);
	} else {
		priv->adapter->suspend_fail = TRUE;
		OS_INT_RESTORE;
		priv->adapter->hs_skip++;
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG,
		       "BT: suspend skipped! "
		       "state=%d skip=%d ps_state= %d WakeupTries=%d\n",
		       priv->adapter->hs_state, priv->adapter->hs_skip,
		       priv->adapter->ps_state, priv->adapter->WakeupTries);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function Set Evt Filter
 *
 *  @param priv    A pointer to bt_private structure
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_set_evt_filter(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf = __cpu_to_le16((0x03 << 10) | BT_CMD_SET_EVT_FILTER);
	pcmd->length = 0x03;
	pcmd->data[0] = 0x02;
	pcmd->data[1] = 0x00;
	pcmd->data[2] = 0x03;
	bt_cb(skb)->pkt_type = HCI_COMMAND_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue Set Evt Filter Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout(priv->adapter->cmd_wait_q,
					   priv->adapter->cmd_complete,
					   WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Set Evt Filter timeout\n");
		bt_cmd_timeout_func(priv, BT_CMD_SET_EVT_FILTER);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function Enable Write Scan - Page and Inquiry
 *
 *  @param priv    A pointer to bt_private structure
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_enable_write_scan(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf = __cpu_to_le16((0x03 << 10) | BT_CMD_ENABLE_WRITE_SCAN);
	pcmd->length = 0x01;
	pcmd->data[0] = 0x03;
	bt_cb(skb)->pkt_type = HCI_COMMAND_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue Enable Write Scan Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout(priv->adapter->cmd_wait_q,
					   priv->adapter->cmd_complete,
					   WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Enable Write Scan timeout\n");
		bt_cmd_timeout_func(priv, BT_CMD_ENABLE_WRITE_SCAN);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function Enable Device under test mode
 *
 *  @param priv    A pointer to bt_private structure
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_enable_device_under_testmode(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((0x06 << 10) | BT_CMD_ENABLE_DEVICE_TESTMODE);
	pcmd->length = 0x00;
	bt_cb(skb)->pkt_type = HCI_COMMAND_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	PRINTM(CMD, "Queue enable device under testmode Command(0x%x)\n",
	       __le16_to_cpu(pcmd->ocf_ogf));
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout(priv->adapter->cmd_wait_q,
					   priv->adapter->cmd_complete,
					   WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Enable Device under TEST mode timeout\n");
		bt_cmd_timeout_func(priv, BT_CMD_ENABLE_DEVICE_TESTMODE);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function enables test mode and send cmd
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_enable_test_mode(bt_private *priv)
{
	int ret = BT_STATUS_SUCCESS;

	ENTER();

	/** Set Evt Filter Command */
	ret = bt_set_evt_filter(priv);
	if (ret != BT_STATUS_SUCCESS) {
		PRINTM(ERROR, "BT test_mode: Set Evt filter fail\n");
		goto exit;
	}

	/** Enable Write Scan Command */
	ret = bt_enable_write_scan(priv);
	if (ret != BT_STATUS_SUCCESS) {
		PRINTM(ERROR, "BT test_mode: Enable Write Scan fail\n");
		goto exit;
	}

	/** Enable Device under test mode */
	ret = bt_enable_device_under_testmode(priv);
	if (ret != BT_STATUS_SUCCESS)
		PRINTM(ERROR,
		       "BT test_mode: Enable device under testmode fail\n");

exit:
	LEAVE();
	return ret;
}

#define DISABLE_RESET  0x0
#define ENABLE_OUTBAND_RESET 0x1
#define ENABLE_INBAND_RESET  0x02
#define DEFAULT_GPIO 0xff
/**
 *  @brief This function set GPIO pin
 *
 *  @param priv    A pointer to bt_private structure
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_set_independent_reset(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	u8 mode, gpio;
	BT_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_INDEPENDENT_RESET);
	mode = btindrst & 0xff;
	gpio = (btindrst & 0xff00) >> 8;
	if (mode == ENABLE_OUTBAND_RESET) {
		pcmd->data[0] = ENABLE_OUTBAND_RESET;
		if (!gpio)
			pcmd->data[1] = DEFAULT_GPIO;
		else
			pcmd->data[1] = gpio;
	} else if (mode == ENABLE_INBAND_RESET) {
		pcmd->data[0] = ENABLE_INBAND_RESET;
		pcmd->data[1] = DEFAULT_GPIO;
	} else if (mode == DISABLE_RESET) {
		pcmd->data[0] = DISABLE_RESET;
		pcmd->data[1] = DEFAULT_GPIO;
	} else {
		PRINTM(WARN, "Unsupport mode\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	PRINTM(CMD, "BT: independant reset mode=%d gpio=%d\n", mode, gpio);
	pcmd->length = 2;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout(priv->adapter->cmd_wait_q,
					   priv->adapter->cmd_complete,
					   WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Independent reset : timeout!\n");
		bt_cmd_timeout_func(priv, BT_CMD_INDEPENDENT_RESET);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function sets ble deepsleep mode
 *
 *  @param priv    A pointer to bt_private structure
 *  @param mode    TRUE/FALSE
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_set_ble_deepsleep(bt_private *priv, int mode)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_BLE_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_BLE_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_BLE_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_BLE_DEEP_SLEEP);
	pcmd->length = 1;
	pcmd->deepsleep = mode;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, sizeof(BT_BLE_CMD));
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	PRINTM(CMD, "BT: Set BLE deepsleep = %d (0x%x)\n", mode,
	       __le16_to_cpu(pcmd->ocf_ogf));
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Set BLE deepsleep timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_BLE_DEEP_SLEEP);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function gets FW version
 *
 *  @param priv    A pointer to bt_private structure
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_get_fw_version(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_HCI_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_HCI_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_HCI_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_GET_FW_VERSION);
	pcmd->length = 0x01;
	pcmd->cmd_type = 0x00;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, 4);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout(priv->adapter->cmd_wait_q,
					   priv->adapter->cmd_complete,
					   WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Get FW version: timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_GET_FW_VERSION);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function sets mac address
 *
 *  @param priv    A pointer to bt_private structure
 *  @param mac     A pointer to mac address
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_set_mac_address(bt_private *priv, u8 *mac)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_HCI_CMD *pcmd;
	int i = 0;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_HCI_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_HCI_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_CONFIG_MAC_ADDR);
	pcmd->length = 8;
	pcmd->cmd_type = MRVL_VENDOR_PKT;
	pcmd->cmd_len = 6;
	for (i = 0; i < 6; i++)
		pcmd->data[i] = mac[5 - i];
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, sizeof(BT_HCI_CMD));
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	PRINTM(CMD, "BT: Set mac addr " MACSTR " (0x%x)\n", MAC2STR(mac),
	       __le16_to_cpu(pcmd->ocf_ogf));
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(MSG, "BT: Set mac addr: timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_CONFIG_MAC_ADDR);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function load the calibrate EXT data
 *
 *  @param priv    A pointer to bt_private structure
 *  @param config_data     A pointer to calibrate data
 *  @param mac     A pointer to mac address
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_process_commands(bt_private *priv, u8 *cmd_data, u32 cmd_len)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	u16 ogf = 0;

	ENTER();
	PRINTM(CMD, "BT: init cmds: len=%d\n", cmd_len);
	if (cmd_len > BT_CMD_DATA_LEN) {
		PRINTM(WARN, "cfg_data_len is too long exceed %d.\n",
		       BT_CMD_DATA_LEN);
		ret = BT_STATUS_FAILURE;
		goto exit;
	}

	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	memcpy(skb->data, cmd_data, cmd_len);
	pcmd = (BT_CMD *)skb->data;

	ogf = hci_opcode_ogf(pcmd->ocf_ogf);
	if (ogf == VENDOR_OGF)
		bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	else
		bt_cb(skb)->pkt_type = HCI_COMMAND_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;

	DBG_HEXDUMP(DAT_D, "init_cmds", skb->data, skb->len);
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(ERROR, "BT: Load init cmds: timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_LOAD_CONFIG_DATA_EXT);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function load the calibrate data
 *
 *  @param priv    A pointer to bt_private structure
 *  @param config_data     A pointer to calibrate data
 *  @param mac     A pointer to mac address
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_load_cal_data(bt_private *priv, u8 *config_data, u8 *mac)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;
	int i = 0;
	/* u8 config_data[28] = {0x37 0x01 0x1c 0x00 0xFF 0xFF 0xFF 0xFF 0x01
	   0x7f 0x04 0x02 0x00 0x00 0xBA 0xCE 0xC0 0xC6 0x2D 0x00 0x00 0x00
	   0x00 0x00 0x00 0x00 0xF0}; */

	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_LOAD_CONFIG_DATA);
	pcmd->length = 0x20;
	pcmd->data[0] = 0x00;
	pcmd->data[1] = 0x00;
	pcmd->data[2] = 0x00;
	pcmd->data[3] = 0x1C;
	/* swip cal-data byte */
	for (i = 4; i < 32; i++)
		pcmd->data[i] = *(config_data + ((i / 4) * 8 - 1 - i));
	if (mac != NULL) {
		pcmd->data[2] = 0x01;	/* skip checksum */
		for (i = 24; i < 30; i++)
			pcmd->data[i] = mac[29 - i];
	}
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;

	DBG_HEXDUMP(DAT_D, "calirate data: ", pcmd->data, 32);
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(ERROR, "BT: Load calibrate data: timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_LOAD_CONFIG_DATA);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function load the calibrate EXT data
 *
 *  @param priv    A pointer to bt_private structure
 *  @param config_data     A pointer to calibrate data
 *  @param mac     A pointer to mac address
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_load_cal_data_ext(bt_private *priv, u8 *config_data, u32 cfg_data_len)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CMD *pcmd;

	ENTER();

	if (cfg_data_len > BT_CMD_DATA_LEN) {
		PRINTM(WARN, "cfg_data_len is too long exceed %d.\n",
		       BT_CMD_DATA_LEN);
		ret = BT_STATUS_FAILURE;
		goto exit;
	}

	skb = bt_skb_alloc(sizeof(BT_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_LOAD_CONFIG_DATA_EXT);
	pcmd->length = cfg_data_len;

	memcpy(pcmd->data, config_data, cfg_data_len);
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, BT_CMD_HEADER_SIZE + pcmd->length);
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;

	DBG_HEXDUMP(DAT_D, "calirate ext data", pcmd->data, pcmd->length);
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(ERROR, "BT: Load calibrate ext data: timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_LOAD_CONFIG_DATA_EXT);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function writes value to CSU registers
 *
 *  @param priv    A pointer to bt_private structure
 *  @param type    reg type
 *  @param offset  register address
 *  @param value   register value to write
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_write_reg(bt_private *priv, u8 type, u32 offset, u16 value)
{
	struct sk_buff *skb = NULL;
	int ret = BT_STATUS_SUCCESS;
	BT_CSU_CMD *pcmd;
	ENTER();
	skb = bt_skb_alloc(sizeof(BT_CSU_CMD), GFP_ATOMIC);
	if (skb == NULL) {
		PRINTM(WARN, "No free skb\n");
		ret = BT_STATUS_FAILURE;
		goto exit;
	}
	pcmd = (BT_CSU_CMD *)skb->data;
	pcmd->ocf_ogf =
		__cpu_to_le16((VENDOR_OGF << 10) | BT_CMD_CSU_WRITE_REG);
	pcmd->length = 7;
	pcmd->type = type;
	pcmd->offset[0] = (offset & 0x000000ff);
	pcmd->offset[1] = (offset & 0x0000ff00) >> 8;
	pcmd->offset[2] = (offset & 0x00ff0000) >> 16;
	pcmd->offset[3] = (offset & 0xff000000) >> 24;
	pcmd->value[0] = (value & 0x00ff);
	pcmd->value[1] = (value & 0xff00) >> 8;
	bt_cb(skb)->pkt_type = MRVL_VENDOR_PKT;
	skb_put(skb, sizeof(BT_CSU_CMD));
	skb->dev = (void *)(&(priv->bt_dev.m_dev[BT_SEQ]));
	skb_queue_head(&priv->adapter->tx_queue, skb);
	priv->bt_dev.sendcmdflag = TRUE;
	priv->bt_dev.send_cmd_opcode = __le16_to_cpu(pcmd->ocf_ogf);
	priv->adapter->cmd_complete = FALSE;
	PRINTM(CMD, "BT: Set CSU reg type=%d reg=0x%x value=0x%x\n",
	       type, offset, value);
	wake_up_interruptible(&priv->MainThread.waitQ);
	if (!os_wait_interruptible_timeout
	    (priv->adapter->cmd_wait_q, priv->adapter->cmd_complete,
	     WAIT_UNTIL_CMD_RESP)) {
		ret = BT_STATUS_FAILURE;
		PRINTM(ERROR, "BT: Set CSU reg timeout:\n");
		bt_cmd_timeout_func(priv, BT_CMD_CSU_WRITE_REG);
	}
exit:
	LEAVE();
	return ret;
}

/**
 *  @brief This function used to restore tx_queue
 *
 *  @param priv    A pointer to bt_private structure
 *  @return        N/A
 */
void
bt_restore_tx_queue(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	while (!skb_queue_empty(&priv->adapter->pending_queue)) {
		skb = skb_dequeue(&priv->adapter->pending_queue);
		if (skb)
			bt_queue_frame(priv, skb);
	}
	wake_up_interruptible(&priv->MainThread.waitQ);
}

/**
 *  @brief This function used to send command to firmware
 *
 *  Command format:
 *  +--------+--------+--------+--------+--------+--------+--------+
 *  |     OCF OGF     | Length |                Data               |
 *  +--------+--------+--------+--------+--------+--------+--------+
 *  |     2-byte      | 1-byte |               4-byte              |
 *  +--------+--------+--------+--------+--------+--------+--------+
 *
 *  @param priv    A pointer to bt_private structure
 *  @return        BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
bt_prepare_command(bt_private *priv)
{
	int ret = BT_STATUS_SUCCESS;
	ENTER();
	if (priv->bt_dev.hscfgcmd) {
		priv->bt_dev.hscfgcmd = 0;
		ret = bt_send_hscfg_cmd(priv);
	}
	if (priv->bt_dev.pscmd) {
		priv->bt_dev.pscmd = 0;
		ret = bt_enable_ps(priv);
	}
	if (priv->bt_dev.sdio_pull_ctrl) {
		priv->bt_dev.sdio_pull_ctrl = 0;
		ret = bt_send_sdio_pull_ctrl_cmd(priv);
	}
	if (priv->bt_dev.hscmd) {
		priv->bt_dev.hscmd = 0;
		if (priv->bt_dev.hsmode)
			ret = bt_enable_hs(priv, FALSE);
		else {
			ret = sbi_wakeup_firmware(priv);
			priv->adapter->hs_state = HS_DEACTIVATED;
		}
	}
	if (priv->bt_dev.test_mode) {
		priv->bt_dev.test_mode = 0;
		ret = bt_enable_test_mode(priv);
	}
	LEAVE();
	return ret;
}

/**
 * @brief Update tx state
 *
 * @param priv          A pointer to bt_private structure
 * @param skb			A pointer to sk_buff structure
 *
 * @return                N/A
 */
static void
update_stat_byte_tx(bt_private *priv, struct sk_buff *skb)
{
	((struct hci_dev *)priv->bt_dev.m_dev[BT_SEQ].dev_pointer)->stat.
		byte_tx += skb->len;
}

/**
 * @brief Update tx error state
 *
 * @param priv          A pointer to bt_private structure
 * @param skb			A pointer to sk_buff structure
 *
 * @return                N/A
 */
static void
update_stat_err_tx(bt_private *priv, struct sk_buff *skb)
{
	((struct hci_dev *)priv->bt_dev.m_dev[BT_SEQ].dev_pointer)->stat.
		err_tx++;
}

/** @brief This function processes a single packet
 *
 *  @param priv    A pointer to bt_private structure
 *  @param skb     A pointer to skb which includes TX packet
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
static int
send_single_packet(bt_private *priv, struct sk_buff *skb)
{
	int ret;
	struct sk_buff *new_skb = NULL;
	ENTER();
	if (!skb || !skb->data) {
		LEAVE();
		return BT_STATUS_FAILURE;
	}

	if (!skb->len || ((skb->len + BT_HEADER_LEN) > BT_UPLD_SIZE)) {
		PRINTM(ERROR, "Tx Error: Bad skb length %d : %d\n", skb->len,
		       BT_UPLD_SIZE);
		LEAVE();
		return BT_STATUS_FAILURE;
	}
	if (skb_headroom(skb) < BT_HEADER_LEN) {
		new_skb = skb_realloc_headroom(skb, BT_HEADER_LEN);
		if (!new_skb) {
			PRINTM(ERROR, "TX error: realloc_headroom failed %d\n",
			       BT_HEADER_LEN);
			kfree_skb(skb);
			LEAVE();
			return BT_STATUS_FAILURE;
		} else {
			if (new_skb != skb)
				dev_kfree_skb_any(skb);
			skb = new_skb;
		}
	}
	/* This is SDIO/PCIE specific header length: byte[3][2][1], * type:
	   byte[0] (HCI_COMMAND = 1, ACL_DATA = 2, SCO_DATA = 3, 0xFE = Vendor)
	 */
	skb_push(skb, BT_HEADER_LEN);
	skb->data[0] = (skb->len & 0x0000ff);
	skb->data[1] = (skb->len & 0x00ff00) >> 8;
	skb->data[2] = (skb->len & 0xff0000) >> 16;
	skb->data[3] = bt_cb(skb)->pkt_type;
	if (bt_cb(skb)->pkt_type == MRVL_VENDOR_PKT)
		PRINTM(CMD, "DNLD_CMD: ocf_ogf=0x%x len=%d\n",
		       __le16_to_cpu(*((u16 *) & skb->data[4])), skb->len);
	ret = sbi_host_to_card(priv, skb->data, skb->len);
	if (ret == BT_STATUS_FAILURE)
		update_stat_err_tx(priv, skb);
	else
		update_stat_byte_tx(priv, skb);
	if (ret != BT_STATUS_PENDING)
		kfree_skb(skb);
	LEAVE();
	return ret;
}

#ifdef CONFIG_OF
/**
 *  @brief This function read the initial parameter from device tress
 *
 *
 *  @return         N/A
 */
static void
bt_init_from_dev_tree(void)
{
	struct device_node *dt_node = NULL;
	struct property *prop;
	u32 data;
	const char *string_data;

	ENTER();

	if (!dts_enable) {
		PRINTM(CMD, "DTS is disabled!");
		return;
	}

	dt_node = of_find_node_by_name(NULL, "sd8xxx-bt");
	if (!dt_node) {
		LEAVE();
		return;
	}
	for_each_property_of_node(dt_node, prop) {
#ifdef DEBUG_LEVEL1
		if (!strncmp(prop->name, "mbt_drvdbg", strlen("mbt_drvdbg"))) {
			if (!of_property_read_u32(dt_node, prop->name, &data)) {
				PRINTM(CMD, "mbt_drvdbg=0x%x\n", data);
				mbt_drvdbg = data;
			}
		}
#endif
		else if (!strncmp(prop->name, "init_cmds", strlen("init_cmds"))) {
			if (!of_property_read_string
			    (dt_node, prop->name, &string_data)) {
				init_cmds = (char *)string_data;
				PRINTM(CMD, "init_cmds=%s\n", init_cmds);
			}
		} else if (!strncmp(prop->name, "init_cfg", strlen("init_cfg"))) {
			if (!of_property_read_string
			    (dt_node, prop->name, &string_data)) {
				init_cfg = (char *)string_data;
				PRINTM(CMD, "init_cfg=%s\n", init_cfg);
			}
		} else if (!strncmp
			   (prop->name, "cal_cfg_ext", strlen("cal_cfg_ext"))) {
			if (!of_property_read_string
			    (dt_node, prop->name, &string_data)) {
				cal_cfg_ext = (char *)string_data;
				PRINTM(CMD, "cal_cfg_ext=%s\n", cal_cfg_ext);
			}
		} else if (!strncmp(prop->name, "cal_cfg", strlen("cal_cfg"))) {
			if (!of_property_read_string
			    (dt_node, prop->name, &string_data)) {
				cal_cfg = (char *)string_data;
				PRINTM(CMD, "cal_cfg=%s\n", cal_cfg);
			}
		} else if (!strncmp(prop->name, "bt_mac", strlen("bt_mac"))) {
			if (!of_property_read_string
			    (dt_node, prop->name, &string_data)) {
				bt_mac = (char *)string_data;
				PRINTM(CMD, "bt_mac=%s\n", bt_mac);
			}
		} else if (!strncmp(prop->name, "btindrst", strlen("btindrst"))) {
			if (!of_property_read_u32(dt_node, prop->name, &data)) {
				btindrst = data;
				PRINTM(CMD, "btindrst=%d\n", btindrst);
			}
		} else if (!strncmp(prop->name, "btpmic", strlen("btpmic"))) {
			if (!of_property_read_u32(dt_node, prop->name, &data)) {
				btpmic = data;
				PRINTM(CMD, "btpmic=%d\n", btpmic);
			}
		}
	}
	LEAVE();
	return;
}
#endif

/**
 *  @brief This function initializes the adapter structure
 *  and set default value to the member of adapter.
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    N/A
 */
static void
bt_init_adapter(bt_private *priv)
{
	ENTER();
#ifdef CONFIG_OF
	bt_init_from_dev_tree();
#endif
	skb_queue_head_init(&priv->adapter->tx_queue);
	skb_queue_head_init(&priv->adapter->pending_queue);
	priv->adapter->tx_lock = FALSE;
	priv->adapter->ps_state = PS_AWAKE;
	priv->adapter->suspend_fail = FALSE;
	priv->adapter->is_suspended = FALSE;
	priv->adapter->hs_skip = 0;
	priv->adapter->num_cmd_timeout = 0;
	priv->adapter->fwdump_fname = NULL;
	init_waitqueue_head(&priv->adapter->cmd_wait_q);
	LEAVE();
}

/**
 *  @brief This function initializes firmware
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
static int
bt_init_fw(bt_private *priv)
{
	int ret = BT_STATUS_SUCCESS;
	ENTER();
	if (fw == 0) {
		sbi_enable_host_int(priv);
		goto done;
	}
	sbi_disable_host_int(priv);
	if (sbi_download_fw(priv)) {
		PRINTM(ERROR, " FW failed to be download!\n");
		ret = BT_STATUS_FAILURE;
		goto done;
	}
done:
	LEAVE();
	return ret;
}

#define FW_POLL_TRIES 100
#define FW_RESET_REG  0x0EE
#define FW_RESET_VAL  0x99

/**
 *  @brief This function reload firmware
 *
 *  @param priv   A pointer to bt_private
 *  @param mode   FW reload mode
 *
 *  @return       0--success, otherwise failure
 */
static int
bt_reload_fw(bt_private *priv, int mode)
{
	int ret = 0, tries = 0;
	u8 value = 1;
	u32 reset_reg = FW_RESET_REG;
	u8 reset_val = FW_RESET_VAL;

	ENTER();
	if ((mode != FW_RELOAD_SDIO_INBAND_RESET) &&
	    (mode != FW_RELOAD_NO_EMULATION)) {
		PRINTM(ERROR, "Invalid fw reload mode=%d\n", mode);
		return -EFAULT;
	}

    /** flush pending tx_queue */
	skb_queue_purge(&priv->adapter->tx_queue);

	priv->adapter->tx_lock = FALSE;
	priv->adapter->ps_state = PS_AWAKE;
	priv->adapter->suspend_fail = FALSE;
	priv->adapter->is_suspended = FALSE;
	priv->adapter->hs_skip = 0;
	priv->adapter->num_cmd_timeout = 0;
	priv->adapter->fwdump_fname = NULL;

	if (mode == FW_RELOAD_SDIO_INBAND_RESET) {
		sbi_disable_host_int(priv);
	    /** Wake up firmware firstly */
		sbi_wakeup_firmware(priv);

	/** wait SOC fully wake up */
		for (tries = 0; tries < FW_POLL_TRIES; ++tries) {
			ret = sd_write_reg(priv, reset_reg, 0xba);
			if (!ret) {
				ret = sd_read_reg(priv, reset_reg, &value);
				if (!ret && (value == 0xba)) {
					PRINTM(MSG, "Fw wake up\n");
					break;
				}
			}
			udelay(1000);
		}

		ret = sd_write_reg(priv, reset_reg, reset_val);
		if (ret) {
			PRINTM(ERROR, "Failed to write register.\n");
			goto done;
		}

	    /** Poll register around 1 ms */
		for (; tries < FW_POLL_TRIES; ++tries) {
			ret = sd_read_reg(priv, reset_reg, &value);
			if (ret) {
				PRINTM(ERROR, "Failed to read register.\n");
				goto done;
			}
			if (value == 0)
			    /** FW is ready */
				break;
			udelay(1000);
		}
		if (value) {
			PRINTM(ERROR,
			       "Failed to poll FW reset register %X=0x%x\n",
			       reset_reg, value);
			ret = -EFAULT;
			goto done;
		}
	}

	sbi_enable_host_int(priv);
	/** reload FW */
	ret = bt_init_fw(priv);
	if (ret) {
		PRINTM(ERROR, "Re download firmware failed.\n");
		ret = -EFAULT;
	}
	LEAVE();
	return ret;
done:
	sbi_enable_host_int(priv);
	LEAVE();
	return ret;
}

/**
 *  @brief This function request to reload firmware
 *
 *  @param priv   A pointer to bt_private
 *  @param mode   fw reload mode.
 *
 *  @return         N/A
 */
void
bt_request_fw_reload(bt_private *priv, int mode)
{
	ENTER();
	if (mode == FW_RELOAD_WITH_EMULATION) {
		bt_fw_reload = FW_RELOAD_WITH_EMULATION;
		PRINTM(MSG, "BT: FW reload with re-emulation...\n");
		LEAVE();
		return;
	}
	/** Reload FW */
	priv->fw_reload = TRUE;
	if (bt_reload_fw(priv, mode)) {
		PRINTM(ERROR, "FW reload fail\n");
		goto done;
	}
	priv->fw_reload = FALSE;
	/** Other operation here? */
done:
	LEAVE();
	return;
}

/**
 *  @brief This function frees the structure of adapter
 *
 *  @param priv    A pointer to bt_private structure
 *  @return    N/A
 */
void
bt_free_adapter(bt_private *priv)
{
	bt_adapter *adapter = priv->adapter;
	ENTER();
	skb_queue_purge(&priv->adapter->tx_queue);
	kfree(adapter->tx_buffer);
	kfree(adapter->hw_regs_buf);
	/* Free allocated memory for fwdump filename */
	if (adapter->fwdump_fname) {
		kfree(adapter->fwdump_fname);
		adapter->fwdump_fname = NULL;
	}
	/* Free the adapter object itself */
	kfree(adapter);
	priv->adapter = NULL;

	LEAVE();
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
/**
 *  @brief This function handles the BT ioctl
 *
 *  @param hdev     A pointer to hci_dev structure
 *  @cmd            ioctl cmd
 *  @arg            argument
 *  @return    -ENOIOCTLCMD
 */
static int
bt_ioctl(struct hci_dev *hdev, unsigned int cmd, unsigned long arg)
{
	ENTER();
	LEAVE();
	return -ENOIOCTLCMD;
}
#endif

/**
 *  @brief This function handles the wrapper_dev ioctl
 *
 *  @param hev     A pointer to wrapper_dev structure
 *  @cmd            ioctl cmd
 *  @arg            argument
 *  @return    -ENOIOCTLCMD
 */
static int
mdev_ioctl(struct m_dev *m_dev, unsigned int cmd, void *arg)
{
	bt_private *priv = NULL;
	int ret = 0;

	ENTER();

	if (!m_dev || !m_dev->driver_data) {
		PRINTM(ERROR, "Ioctl for unknown device (m_dev=NULL)\n");
		ret = -ENODEV;
		goto done;
	}
	priv = (bt_private *)m_dev->driver_data;
	if (!test_bit(HCI_RUNNING, &m_dev->flags)) {
		PRINTM(ERROR, "HCI_RUNNING not set, flag=0x%lx\n",
		       m_dev->flags);
		ret = -EBUSY;
		goto done;
	}
	PRINTM(INFO, "IOCTL: cmd=%d\n", cmd);
	switch (cmd) {
	case MBTCHAR_IOCTL_BT_FW_DUMP:
		bt_dump_sdio_regs(priv);
		bt_dump_firmware_info_v2(priv);
		break;
	default:
		break;
	}

done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function handles BT destruct
 *
 *  @param hdev    A pointer to hci_dev structure
 *
 *  @return    N/A
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0)
static void
bt_destruct(struct hci_dev *hdev)
{
	ENTER();
	LEAVE();
	return;
}
#endif

/**
 *  @brief This function handles wrapper device destruct
 *
 *  @param m_dev   A pointer to m_dev structure
 *
 *  @return    N/A
 */
static void
mdev_destruct(struct m_dev *m_dev)
{
	ENTER();
	LEAVE();
	return;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
/**
 *  @brief This function handles the BT transmit
 *
 *  @param skb     A pointer to sk_buff structure
 *
 *  @return    BT_STATUS_SUCCESS or other error no.
 */
static int
bt_send_frame(struct sk_buff *skb)
#else
/**
 *  @brief This function handles the BT transmit
 *
 *  @param hdev    A pointer to hci_dev structure
 *  @param skb     A pointer to sk_buff structure
 *
 *  @return    BT_STATUS_SUCCESS or other error no.
 */
static int
bt_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
#endif
{
	bt_private *priv = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
	struct hci_dev *hdev = (struct hci_dev *)skb->dev;
#else
	skb->dev = (void *)hdev;
#endif

	ENTER();
	PRINTM(DATA, "bt_send_frame %s: Type=%d, len=%d\n", hdev->name,
	       bt_cb(skb)->pkt_type, skb->len);
	DBG_HEXDUMP(CMD_D, "bt_send_frame", skb->data, skb->len);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
	if (!hdev || !hci_get_drvdata(hdev)) {
#else
	if (!hdev || !hdev->driver_data) {
#endif
		PRINTM(ERROR, "Frame for unknown HCI device (hdev=NULL)\n");
		LEAVE();
		return -ENODEV;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
	priv = (bt_private *)hci_get_drvdata(hdev);
#else
	priv = (bt_private *)hdev->driver_data;
#endif
	if (!test_bit(HCI_RUNNING, &hdev->flags)) {
		PRINTM(ERROR, "Fail test HCI_RUNNING, flag=0x%lx\n",
		       hdev->flags);
		LEAVE();
		return -EBUSY;
	}
	switch (bt_cb(skb)->pkt_type) {
	case HCI_COMMAND_PKT:
		hdev->stat.cmd_tx++;
		break;
	case HCI_ACLDATA_PKT:
		hdev->stat.acl_tx++;
		break;
	case HCI_SCODATA_PKT:
		hdev->stat.sco_tx++;
		break;
	}
	if (priv->adapter->tx_lock == TRUE)
		skb_queue_tail(&priv->adapter->pending_queue, skb);
	else
		bt_queue_frame(priv, skb);

	wake_up_interruptible(&priv->MainThread.waitQ);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the wrapper device transmit
 *
 *  @param m_dev   A pointer to m_dev structure
 *  @param skb     A pointer to sk_buff structure
 *
 *  @return    BT_STATUS_SUCCESS or other error no.
 */
static int
mdev_send_frame(struct m_dev *m_dev, struct sk_buff *skb)
{
	bt_private *priv = NULL;

	ENTER();
	if (!m_dev || !m_dev->driver_data) {
		PRINTM(ERROR, "Frame for unknown HCI device (m_dev=NULL)\n");
		LEAVE();
		return -ENODEV;
	}
	priv = (bt_private *)m_dev->driver_data;
	if (!test_bit(HCI_RUNNING, &m_dev->flags)) {
		PRINTM(ERROR, "Fail test HCI_RUNNING, flag=0x%lx\n",
		       m_dev->flags);
		LEAVE();
		return -EBUSY;
	}
	switch (bt_cb(skb)->pkt_type) {
	case HCI_COMMAND_PKT:
		m_dev->stat.cmd_tx++;
		break;
	case HCI_ACLDATA_PKT:
		m_dev->stat.acl_tx++;
		break;
	case HCI_SCODATA_PKT:
		m_dev->stat.sco_tx++;
		break;
	}

	if (priv->adapter->tx_lock == TRUE)
		skb_queue_tail(&priv->adapter->pending_queue, skb);
	else
		bt_queue_frame(priv, skb);
	wake_up_interruptible(&priv->MainThread.waitQ);

	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function flushes the transmit queue
 *
 *  @param hdev     A pointer to hci_dev structure
 *
 *  @return    BT_STATUS_SUCCESS
 */
static int
bt_flush(struct hci_dev *hdev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
	bt_private *priv = (bt_private *)hci_get_drvdata(hdev);
#else
	bt_private *priv = (bt_private *)hdev->driver_data;
#endif
	ENTER();
	skb_queue_purge(&priv->adapter->tx_queue);
	skb_queue_purge(&priv->adapter->pending_queue);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function flushes the transmit queue
 *
 *  @param m_dev     A pointer to m_dev structure
 *
 *  @return    BT_STATUS_SUCCESS
 */
static int
mdev_flush(struct m_dev *m_dev)
{
	bt_private *priv = (bt_private *)m_dev->driver_data;
	ENTER();
	skb_queue_purge(&priv->adapter->tx_queue);
	skb_queue_purge(&priv->adapter->pending_queue);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function closes the bluetooth device
 *
 *  @param hdev    A pointer to hci_dev structure
 *
 *  @return    BT_STATUS_SUCCESS
 */
static int
bt_close(struct hci_dev *hdev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
	bt_private *priv = (bt_private *)hci_get_drvdata(hdev);
#else
	bt_private *priv = (bt_private *)hdev->driver_data;
#endif

	ENTER();
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 3, 0)
	if (!test_and_clear_bit(HCI_RUNNING, &hdev->flags)) {
		LEAVE();
		return BT_STATUS_SUCCESS;
	}
#endif
	skb_queue_purge(&priv->adapter->tx_queue);

	module_put(THIS_MODULE);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function closes the wrapper device
 *
 *  @param m_dev   A pointer to m_dev structure
 *
 *  @return    BT_STATUS_SUCCESS
 */
static int
mdev_close(struct m_dev *m_dev)
{

	ENTER();
	mdev_req_lock(m_dev);
	if (!test_and_clear_bit(HCI_UP, &m_dev->flags)) {
		mdev_req_unlock(m_dev);
		LEAVE();
		return 0;
	}

	if (m_dev->flush)
		m_dev->flush(m_dev);
	/* wait up pending read and unregister char dev */
	wake_up_interruptible(&m_dev->req_wait_q);
	/* Drop queues */
	skb_queue_purge(&m_dev->rx_q);
	if (!test_and_clear_bit(HCI_RUNNING, &m_dev->flags)) {
		mdev_req_unlock(m_dev);
		LEAVE();
		return 0;
	}
	module_put(THIS_MODULE);
	m_dev->flags = 0;
	mdev_req_unlock(m_dev);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function opens the bluetooth device
 *
 *  @param hdev    A pointer to hci_dev structure
 *
 *  @return    BT_STATUS_SUCCESS  or other
 */
static int
bt_open(struct hci_dev *hdev)
{
	ENTER();
	if (try_module_get(THIS_MODULE) == 0)
		return BT_STATUS_FAILURE;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 3, 0)
	set_bit(HCI_RUNNING, &hdev->flags);
#endif
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function opens the wrapper device
 *
 *  @param m_dev   A pointer to m_dev structure
 *
 *  @return    BT_STATUS_SUCCESS  or other
 */
static int
mdev_open(struct m_dev *m_dev)
{
	ENTER();

	if (try_module_get(THIS_MODULE) == 0)
		return BT_STATUS_FAILURE;

	set_bit(HCI_RUNNING, &m_dev->flags);

	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function queries the wrapper device
 *
 *  @param m_dev   A pointer to m_dev structure
 *  @param arg     arguement
 *
 *  @return    BT_STATUS_SUCCESS  or other
 */
void
mdev_query(struct m_dev *m_dev, void *arg)
{
	struct mbt_dev *mbt_dev = (struct mbt_dev *)m_dev->dev_pointer;

	ENTER();
	if (copy_to_user(arg, &mbt_dev->type, sizeof(mbt_dev->type)))
		PRINTM(ERROR, "IOCTL_QUERY_TYPE: Fail copy to user\n");

	LEAVE();
}

/**
 *  @brief This function initializes the wrapper device
 *
 *  @param m_dev   A pointer to m_dev structure
 *
 *  @return    BT_STATUS_SUCCESS  or other
 */
void
init_m_dev(struct m_dev *m_dev)
{
	m_dev->dev_pointer = NULL;
	m_dev->driver_data = NULL;
	m_dev->dev_type = 0;
	m_dev->spec_type = 0;
	skb_queue_head_init(&m_dev->rx_q);
	init_waitqueue_head(&m_dev->req_wait_q);
	init_waitqueue_head(&m_dev->rx_wait_q);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)
	init_MUTEX(&m_dev->req_lock);
#else
	sema_init(&m_dev->req_lock, 1);
#endif
	spin_lock_init(&m_dev->rxlock);
	memset(&m_dev->stat, 0, sizeof(struct hci_dev_stats));
	m_dev->open = mdev_open;
	m_dev->close = mdev_close;
	m_dev->flush = mdev_flush;
	m_dev->send = mdev_send_frame;
	m_dev->destruct = mdev_destruct;
	m_dev->ioctl = mdev_ioctl;
	m_dev->query = mdev_query;
	m_dev->owner = THIS_MODULE;

}

/**
 *  @brief This function handles the major job in bluetooth driver.
 *  it handles the event generated by firmware, rx data received
 *  from firmware and tx data sent from kernel.
 *
 *  @param data    A pointer to bt_thread structure
 *  @return        BT_STATUS_SUCCESS
 */
static int
bt_service_main_thread(void *data)
{
	bt_thread *thread = data;
	bt_private *priv = thread->priv;
	bt_adapter *adapter = priv->adapter;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
	wait_queue_t wait;
#else
	wait_queue_entry_t wait;
#endif
	struct sk_buff *skb;
	ENTER();
	bt_activate_thread(thread);
	init_waitqueue_entry(&wait, current);
	current->flags |= PF_NOFREEZE;

	for (;;) {
		add_wait_queue(&thread->waitQ, &wait);
		OS_SET_THREAD_STATE(TASK_INTERRUPTIBLE);
		if (priv->adapter->WakeupTries ||
		    ((!priv->adapter->IntCounter) &&
		     (!priv->bt_dev.tx_dnld_rdy ||
		      skb_queue_empty(&priv->adapter->tx_queue))
		    )) {
			PRINTM(INFO, "Main: Thread sleeping...\n");
			schedule();
		}
		OS_SET_THREAD_STATE(TASK_RUNNING);
		remove_wait_queue(&thread->waitQ, &wait);
		if (kthread_should_stop() || adapter->SurpriseRemoved) {
			PRINTM(INFO, "main-thread: break from main thread: "
			       "SurpriseRemoved=0x%x\n",
			       adapter->SurpriseRemoved);
			break;
		}

		PRINTM(INFO, "Main: Thread waking up...\n");

		if (priv->adapter->IntCounter) {
			OS_INT_DISABLE;
			adapter->IntCounter = 0;
			OS_INT_RESTORE;
			sbi_get_int_status(priv);
		} else if ((priv->adapter->ps_state == PS_SLEEP) &&
			   (!skb_queue_empty(&priv->adapter->tx_queue)
			   )) {
			priv->adapter->WakeupTries++;
			sbi_wakeup_firmware(priv);
			continue;
		}
		if (priv->adapter->ps_state == PS_SLEEP)
			continue;
		if (priv->bt_dev.tx_dnld_rdy == TRUE) {
			if (!skb_queue_empty(&priv->adapter->tx_queue)) {
				skb = skb_dequeue(&priv->adapter->tx_queue);
				if (skb)
					send_single_packet(priv, skb);
			}
		}
	}
	bt_deactivate_thread(thread);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the interrupt. it will change PS
 *  state if applicable. it will wake up main_thread to handle
 *  the interrupt event as well.
 *
 *  @param m_dev   A pointer to m_dev structure
 *  @return        N/A
 */
void
bt_interrupt(struct m_dev *m_dev)
{
	bt_private *priv = (bt_private *)m_dev->driver_data;
	ENTER();
	if (!priv || !priv->adapter) {
		LEAVE();
		return;
	}
	PRINTM(INTR, "*\n");
	priv->adapter->ps_state = PS_AWAKE;
	if (priv->adapter->hs_state == HS_ACTIVATED) {
		PRINTM(CMD, "BT: %s: HS DEACTIVATED in ISR!\n", m_dev->name);
		priv->adapter->hs_state = HS_DEACTIVATED;
	}
	priv->adapter->WakeupTries = 0;
	priv->adapter->IntCounter++;
	wake_up_interruptible(&priv->MainThread.waitQ);
	LEAVE();
}

/**
 * @brief  Dynamic release of bt private
 *
 * @param kobj          A pointer to kobject structure
 *
 * @return                N/A
 */
static void
bt_private_dynamic_release(struct kobject *kobj)
{
	bt_private *priv = container_of(kobj, bt_private, kobj);
	ENTER();
	PRINTM(INFO, "free bt priv\n");
	kfree(priv);
	LEAVE();
}

static struct kobj_type ktype_bt_private_dynamic = {
	.release = bt_private_dynamic_release,
};

/**
 * @brief  Allocation of bt private
 *
 * @param           	N/A
 *
 * @return              bt_private
 */
static bt_private *
bt_alloc_priv(void)
{
	bt_private *priv;
	ENTER();
	priv = kzalloc(sizeof(bt_private), GFP_KERNEL);
	if (priv) {
		kobject_init(&priv->kobj, &ktype_bt_private_dynamic);
		PRINTM(INFO, "alloc bt priv\n");
	}
	LEAVE();
	return priv;
}

/**
 * @brief  Get bt private structure
 *
 * @param priv          A pointer to bt_private structure
 *
 * @return              kobject structure
 */
struct kobject *
bt_priv_get(bt_private *priv)
{
	PRINTM(INFO, "bt priv get object");
	return kobject_get(&priv->kobj);
}

/**
 * @brief  Get bt private structure
 *
 * @param priv          A pointer to bt_private structure
 *
 * @return              N/A
 */
void
bt_priv_put(bt_private *priv)
{
	PRINTM(INFO, "bt priv put object");
	kobject_put(&priv->kobj);
}

/**
 *  @brief This function send init commands to firmware
 *
 *  @param priv   A pointer to bt_private structure
 *  @return       BT_STATUS_SUCESS/BT_STATUS_FAILURE
 */
int
bt_init_cmd(bt_private *priv)
{
	int ret = BT_STATUS_SUCCESS;

	ENTER();

	ret = bt_send_module_cfg_cmd(priv, MODULE_BRINGUP_REQ);
	if (ret < 0) {
		PRINTM(FATAL, "Module cfg command send failed!\n");
		goto done;
	}
	if (btindrst != -1) {
		ret = bt_set_independent_reset(priv);
		if (ret < 0) {
			PRINTM(FATAL, "Independent reset failed!\n");
			goto done;
		}
	}
	if (btpmic) {
		if (BT_STATUS_SUCCESS != bt_pmic_configure(priv)) {
			PRINTM(FATAL, "BT: PMIC Configure failed \n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	ret = bt_set_ble_deepsleep(priv, deep_sleep ? TRUE : FALSE);
	if (ret < 0) {
		PRINTM(FATAL, "%s BLE deepsleep failed!\n",
		       deep_sleep ? "Enable" : "Disable");
		goto done;
	}
	if (psmode) {
		priv->bt_dev.psmode = TRUE;
		priv->bt_dev.idle_timeout = DEFAULT_IDLE_TIME;
		ret = bt_enable_ps(priv);
		if (ret < 0) {
			PRINTM(FATAL, "Enable PS mode failed!\n");
			goto done;
		}
	}
#if defined(SDIO_SUSPEND_RESUME)
	priv->bt_dev.gpio_gap = DEF_GPIO_GAP;
	ret = bt_send_hscfg_cmd(priv);
	if (ret < 0) {
		PRINTM(FATAL, "Send HSCFG failed!\n");
		goto done;
	}
#endif
	priv->bt_dev.sdio_pull_cfg = 0xffffffff;
	priv->bt_dev.sdio_pull_ctrl = 0;
	wake_up_interruptible(&priv->MainThread.waitQ);

done:
	LEAVE();
	return ret;
}

/**
 *  @brief This function reinit firmware after redownload firmware
 *
 *  @param priv   A pointer to bt_private structure
 *  @return       BT_STATUS_SUCESS/BT_STATUS_FAILURE
 */
int
bt_reinit_fw(bt_private *priv)
{
	int ret = BT_STATUS_SUCCESS;
	priv->adapter->tx_lock = FALSE;
	priv->adapter->ps_state = PS_AWAKE;
	priv->adapter->suspend_fail = FALSE;
	priv->adapter->is_suspended = FALSE;
	priv->adapter->hs_skip = 0;
	priv->adapter->num_cmd_timeout = 0;

	ret = bt_init_cmd(priv);
	if (ret < 0) {
		PRINTM(FATAL, "BT init command failed!\n");
		goto done;
	}
	/* block all the packet from bluez */
	if (init_cfg || cal_cfg || bt_mac || cal_cfg_ext)
		priv->adapter->tx_lock = TRUE;

	if (init_cfg) {
		if (BT_STATUS_SUCCESS != bt_init_config(priv, init_cfg)) {
			PRINTM(FATAL,
			       "BT: Set user init data and param failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	if (cal_cfg) {
		if (BT_STATUS_SUCCESS != bt_cal_config(priv, cal_cfg, bt_mac)) {
			PRINTM(FATAL, "BT: Set cal data failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}

	if (bt_mac) {
		PRINTM(INFO,
		       "Set BT mac_addr from insmod parametre bt_mac = %s\n",
		       bt_mac);
		if (BT_STATUS_SUCCESS != bt_init_mac_address(priv, bt_mac)) {
			PRINTM(FATAL,
			       "BT: Fail to set mac address from insmod parametre\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}

	if (cal_cfg_ext) {
		if (BT_STATUS_SUCCESS != bt_cal_config_ext(priv, cal_cfg_ext)) {
			PRINTM(FATAL, "BT: Set cal ext data failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	if (init_cfg || cal_cfg || bt_mac || cal_cfg_ext) {
		priv->adapter->tx_lock = FALSE;
		bt_restore_tx_queue(priv);
	}

	if (init_cmds) {
		if (BT_STATUS_SUCCESS != bt_init_cmds(priv, init_cmds)) {
			PRINTM(FATAL, "BT: Set user init commands failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	bt_get_fw_version(priv);
	snprintf((char *)priv->adapter->drv_ver, MAX_VER_STR_LEN,
		 mbt_driver_version, fw_version);
done:
	return ret;
}

/**
 *  @brief Module configuration and register device
 *
 *  @param priv      A Pointer to bt_private structure
 *  @return      BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
int
sbi_register_conf_dpc(bt_private *priv)
{
	int ret = BT_STATUS_SUCCESS;
	struct hci_dev *hdev = NULL;
	unsigned char dev_type = 0;

	ENTER();

	priv->bt_dev.tx_dnld_rdy = TRUE;
	if (priv->fw_reload) {
		bt_reinit_fw(priv);
		LEAVE();
		return ret;
	}

	if (drv_mode & DRV_MODE_BT) {
		hdev = hci_alloc_dev();
		if (!hdev) {
			PRINTM(FATAL, "Can not allocate HCI device\n");
			ret = -ENOMEM;
			goto err_kmalloc;
		}
		hdev->open = bt_open;
		hdev->close = bt_close;
		hdev->flush = bt_flush;
		hdev->send = bt_send_frame;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0)
		hdev->destruct = bt_destruct;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
		hdev->ioctl = bt_ioctl;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
		hci_set_drvdata(hdev, priv);
#else
		hdev->driver_data = priv;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 4, 0)
		hdev->owner = THIS_MODULE;
#endif
		init_m_dev(&(priv->bt_dev.m_dev[BT_SEQ]));
		priv->bt_dev.m_dev[BT_SEQ].dev_type = BT_TYPE;
		priv->bt_dev.m_dev[BT_SEQ].spec_type = BLUEZ_SPEC;
		priv->bt_dev.m_dev[BT_SEQ].dev_pointer = (void *)hdev;
		priv->bt_dev.m_dev[BT_SEQ].driver_data = priv;
	}

	dev_type = HCI_SDIO;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
	if (hdev)
		hdev->bus = dev_type;
#else
	if (hdev)
		hdev->type = dev_type;
#endif /* >= 2.6.34 */

	ret = bt_init_cmd(priv);
	if (ret < 0) {
		PRINTM(FATAL, "BT init command failed!\n");
		goto done;
	}

	/** Process device tree init parameters before register hci device.
	 *  Since uplayer device has not yet registered, no need to block tx queue.
	 * */
	if (init_cfg) {
		if (BT_STATUS_SUCCESS != bt_init_config(priv, init_cfg)) {
			PRINTM(FATAL,
			       "BT: Set user init data and param failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	if (cal_cfg) {
		if (BT_STATUS_SUCCESS != bt_cal_config(priv, cal_cfg, bt_mac)) {
			PRINTM(FATAL, "BT: Set cal data failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	} else if (bt_mac) {
		PRINTM(INFO,
		       "Set BT mac_addr from insmod parametre bt_mac = %s\n",
		       bt_mac);
		if (BT_STATUS_SUCCESS != bt_init_mac_address(priv, bt_mac)) {
			PRINTM(FATAL,
			       "BT: Fail to set mac address from insmod parametre\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	if (cal_cfg_ext) {
		if (BT_STATUS_SUCCESS != bt_cal_config_ext(priv, cal_cfg_ext)) {
			PRINTM(FATAL, "BT: Set cal ext data failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}
	if (init_cmds) {
		if (BT_STATUS_SUCCESS != bt_init_cmds(priv, init_cmds)) {
			PRINTM(FATAL, "BT: Set user init commands failed\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
	}

	/* Get FW version */
	bt_get_fw_version(priv);
	snprintf((char *)priv->adapter->drv_ver, MAX_VER_STR_LEN,
		 mbt_driver_version, fw_version);

	if (hdev) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
		hdev->dev_type = priv->bt_dev.devType;
#endif
		ret = hci_register_dev(hdev);
		if (ret < 0) {
			PRINTM(FATAL, "Can not register HCI device\n");
			ret = BT_STATUS_FAILURE;
			goto done;
		}
		snprintf((char *)priv->bt_dev.m_dev[BT_SEQ].name,
			 sizeof(priv->bt_dev.m_dev[BT_SEQ].name), hdev->name);
		bt_proc_init(priv, &(priv->bt_dev.m_dev[BT_SEQ]), BT_SEQ);
	}

done:
	LEAVE();
	return ret;
err_kmalloc:
	LEAVE();
	return ret;
}

/**
 *  @brief This function adds the card. it will probe the
 *  card, allocate the bt_priv and initialize the device.
 *
 *  @param card    A pointer to card
 *  @return        A pointer to bt_private structure
 */

bt_private *
bt_add_card(void *card)
{
	bt_private *priv = NULL;
	int index = 0;

	ENTER();

	priv = bt_alloc_priv();
	if (!priv) {
		PRINTM(FATAL, "Can not allocate priv\n");
		LEAVE();
		return NULL;
	}
	/* Save the handle */
	for (index = 0; index < MAX_BT_ADAPTER; index++) {
		if (m_priv[index] == NULL)
			break;
	}
	if (index < MAX_BT_ADAPTER) {
		m_priv[index] = priv;
	} else {
		PRINTM(ERROR, "Exceeded maximum cards supported!\n");
		goto err_kmalloc;
	}
	/* allocate buffer for bt_adapter */
	priv->adapter = kzalloc(sizeof(bt_adapter), GFP_KERNEL);
	if (!priv->adapter) {
		PRINTM(FATAL, "Allocate buffer for bt_adapter failed!\n");
		goto err_kmalloc;
	}
	priv->adapter->tx_buffer =
		kzalloc(MAX_TX_BUF_SIZE + DMA_ALIGNMENT, GFP_KERNEL);
	if (!priv->adapter->tx_buffer) {
		PRINTM(FATAL, "Allocate buffer for transmit\n");
		goto err_kmalloc;
	}
	priv->adapter->tx_buf =
		(u8 *)ALIGN_ADDR(priv->adapter->tx_buffer, DMA_ALIGNMENT);
	priv->adapter->hw_regs_buf =
		kzalloc(SD_BLOCK_SIZE + DMA_ALIGNMENT, GFP_KERNEL);
	if (!priv->adapter->hw_regs_buf) {
		PRINTM(FATAL, "Allocate buffer for INT read buf failed!\n");
		goto err_kmalloc;
	}
	priv->adapter->hw_regs =
		(u8 *)ALIGN_ADDR(priv->adapter->hw_regs_buf, DMA_ALIGNMENT);
	bt_init_adapter(priv);

	PRINTM(INFO, "Starting kthread...\n");
	priv->MainThread.priv = priv;
	spin_lock_init(&priv->driver_lock);

	bt_create_thread(bt_service_main_thread, &priv->MainThread,
			 "bt_main_service");

	/* wait for mainthread to up */
	while (!priv->MainThread.pid)
		os_sched_timeout(1);

	/** user config file */
	init_waitqueue_head(&priv->init_user_conf_wait_q);

	priv->bt_dev.card = card;

	((struct sdio_mmc_card *)card)->priv = priv;
	priv->adapter->sd_ireg = 0;
	/*
	 * Register the device. Fillup the private data structure with
	 * relevant information from the card and request for the required
	 * IRQ.
	 */
	if (sbi_register_dev(priv) < 0) {
		PRINTM(FATAL, "Failed to register bt device!\n");
		goto err_registerdev;
	}
	if (bt_init_fw(priv)) {
		PRINTM(FATAL, "BT Firmware Init Failed\n");
		goto err_init_fw;
	}
	LEAVE();
	return priv;

err_init_fw:
	clean_up_m_devs(priv);
	bt_proc_remove(priv);
	PRINTM(INFO, "Unregister device\n");
	sbi_unregister_dev(priv);
err_registerdev:
	((struct sdio_mmc_card *)card)->priv = NULL;
	/* Stop the thread servicing the interrupts */
	priv->adapter->SurpriseRemoved = TRUE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	while (priv->MainThread.pid)
		os_sched_timeout(1);
err_kmalloc:
	if (priv->adapter)
		bt_free_adapter(priv);
	for (index = 0; index < MAX_BT_ADAPTER; index++) {
		if (m_priv[index] == priv) {
			m_priv[index] = NULL;
			break;
		}
	}
	bt_priv_put(priv);
	LEAVE();
	return NULL;
}

/**
 *  @brief This function send hardware remove event
 *
 *  @param priv    A pointer to bt_private
 *  @return        N/A
 */
void
bt_send_hw_remove_event(bt_private *priv)
{
	struct sk_buff *skb = NULL;
	struct hci_dev *hdev = NULL;
	ENTER();
	if (!priv->bt_dev.m_dev[BT_SEQ].dev_pointer) {
		LEAVE();
		return;
	}
	if (priv->bt_dev.m_dev[BT_SEQ].spec_type == BLUEZ_SPEC)
		hdev = (struct hci_dev *)priv->bt_dev.m_dev[BT_SEQ].dev_pointer;
#define HCI_HARDWARE_ERROR_EVT  0x10
#define HCI_HARDWARE_REMOVE     0x24
	skb = bt_skb_alloc(3, GFP_ATOMIC);
	skb->data[0] = HCI_HARDWARE_ERROR_EVT;
	skb->data[1] = 1;
	skb->data[2] = HCI_HARDWARE_REMOVE;
	bt_cb(skb)->pkt_type = HCI_EVENT_PKT;
	skb_put(skb, 3);
	if (hdev) {
		skb->dev = (void *)hdev;
		PRINTM(MSG, "Send HW ERROR event\n");
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
		hci_recv_frame(skb);
#else
		hci_recv_frame(hdev, skb);
#endif
		os_sched_timeout(5);
		hdev->stat.byte_rx += 3;
	}
	LEAVE();
	return;
}

/**
 *  @brief This function removes the card.
 *
 *  @param card    A pointer to card
 *  @return        BT_STATUS_SUCCESS
 */
int
bt_remove_card(void *card)
{
	bt_private *priv = (bt_private *)card;
	int index;
	ENTER();
	if (!priv) {
		LEAVE();
		return BT_STATUS_SUCCESS;
	}

	priv->adapter->SurpriseRemoved = TRUE;

	bt_send_hw_remove_event(priv);
	wake_up_interruptible(&priv->adapter->cmd_wait_q);
	priv->adapter->SurpriseRemoved = TRUE;
	wake_up_interruptible(&priv->MainThread.waitQ);
	while (priv->MainThread.pid) {
		os_sched_timeout(1);
		wake_up_interruptible(&priv->MainThread.waitQ);
	}

	bt_proc_remove(priv);
	PRINTM(INFO, "Unregister device\n");
	sbi_unregister_dev(priv);
	clean_up_m_devs(priv);
	PRINTM(INFO, "Free Adapter\n");
	bt_free_adapter(priv);
	for (index = 0; index < MAX_BT_ADAPTER; index++) {
		if (m_priv[index] == priv) {
			m_priv[index] = NULL;
			break;
		}
	}
	bt_priv_put(priv);
	LEAVE();
	return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function initializes module.
 *
 *  @return    BT_STATUS_SUCCESS or BT_STATUS_FAILURE
 */
static int
bt_init_module(void)
{
	int ret = BT_STATUS_SUCCESS;
	int index;
	ENTER();
	PRINTM(MSG, "BT: Loading driver\n");
	/* Init the bt_private pointer array first */
	for (index = 0; index < MAX_BT_ADAPTER; index++)
		m_priv[index] = NULL;
	bt_root_proc_init();

	/** create char device class */
	chardev_class = class_create(THIS_MODULE, MODULE_NAME);
	if (IS_ERR(chardev_class)) {
		PRINTM(ERROR, "Unable to allocate class\n");
		bt_root_proc_remove();
		ret = PTR_ERR(chardev_class);
		goto done;
	}

	if (sbi_register() == NULL) {
		bt_root_proc_remove();
		ret = BT_STATUS_FAILURE;
		goto done;
	}
done:
	if (ret)
		PRINTM(MSG, "BT: Driver loading failed\n");
	else
		PRINTM(MSG, "BT: Driver loaded successfully\n");

	LEAVE();
	return ret;
}

/**
 *  @brief This function cleans module
 *
 *  @return        N/A
 */
static void
bt_exit_module(void)
{
	bt_private *priv;
	int index;
	ENTER();
	PRINTM(MSG, "BT: Unloading driver\n");
	for (index = 0; index < MAX_BT_ADAPTER; index++) {
		priv = m_priv[index];
		if (!priv)
			continue;
		if (priv && !priv->adapter->SurpriseRemoved) {
			if (BT_STATUS_SUCCESS == bt_send_reset_command(priv))
				bt_send_module_cfg_cmd(priv,
						       MODULE_SHUTDOWN_REQ);
		}
		sbi_disable_host_int(priv);

	}

	sbi_unregister();

	bt_root_proc_remove();
	class_destroy(chardev_class);
	PRINTM(MSG, "BT: Driver unloaded\n");
	LEAVE();
}

module_init(bt_init_module);
module_exit(bt_exit_module);

MODULE_AUTHOR("NXP");
MODULE_DESCRIPTION("NXP Bluetooth Driver Ver. " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
module_param(fw, int, 0);
MODULE_PARM_DESC(fw, "0: Skip firmware download; otherwise: Download firmware");
module_param(psmode, int, 0);
MODULE_PARM_DESC(psmode, "1: Enable powermode; 0: Disable powermode");
module_param(deep_sleep, int, 0);
MODULE_PARM_DESC(deep_sleep, "1: Enable deep sleep; 0: Disable deep sleep");
#ifdef CONFIG_OF
module_param(dts_enable, int, 0);
MODULE_PARM_DESC(dts_enable, "0: Disable DTS; 1: Enable DTS");
#endif
#ifdef	DEBUG_LEVEL1
module_param(mbt_drvdbg, uint, 0);
MODULE_PARM_DESC(mbt_drvdbg, "BIT3:DBG_DATA BIT4:DBG_CMD 0xFF:DBG_ALL");
#endif
#ifdef SDIO_SUSPEND_RESUME
module_param(mbt_pm_keep_power, int, 0);
MODULE_PARM_DESC(mbt_pm_keep_power, "1: PM keep power; 0: PM no power");
#endif
module_param(init_cfg, charp, 0);
MODULE_PARM_DESC(init_cfg, "BT init config file name");
module_param(cal_cfg, charp, 0);
MODULE_PARM_DESC(cal_cfg, "BT calibrate file name");
module_param(cal_cfg_ext, charp, 0);
MODULE_PARM_DESC(cal_cfg_ext, "BT calibrate ext file name");
module_param(bt_mac, charp, 0660);
MODULE_PARM_DESC(bt_mac, "BT init mac address");
module_param(init_cmds, charp, 0);
MODULE_PARM_DESC(init_cmds, "BT init commands file name");
module_param(drv_mode, int, 0);
MODULE_PARM_DESC(drv_mode, "Bit 0: BT/AMP/BLE;");
module_param(bt_fw_reload, int, 0);
MODULE_PARM_DESC(bt_fw_reload,
		 "0: disable fw_reload; 1: enable fw reload feature");
module_param(btindrst, int, 0);
MODULE_PARM_DESC(btindrst,
		 "Independent reset configuration; high byte:GPIO pin number;low byte:0x0:disable, 0x1:out-band reset, 0x2:in-band reset.");
module_param(btpmic, int, 0);
MODULE_PARM_DESC(btpmic,
		 "1: Send pmic configure cmd to firmware; 0: No pmic configure cmd sent to firmware (default)");
module_param(bt_fw_serial, int, 0);
MODULE_PARM_DESC(bt_fw_serial,
		 "0: Support parallel download FW; 1: Support serial download FW");
