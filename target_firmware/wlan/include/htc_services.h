/*
 * Copyright (c) 2007 Atheros Communications Inc.
 * All rights reserved.
 */

#ifndef __HTC_SERVICES_H__
#define __HTC_SERVICES_H__

/* Current service IDs */

typedef enum {
	RSVD_SERVICE_GROUP  = 0,
	WMI_SERVICE_GROUP   = 1, 
    
	HTC_TEST_GROUP = 254,
	HTC_SERVICE_GROUP_LAST = 255
} HTC_SERVICE_GROUP_IDS;

#define MAKE_SERVICE_ID(group,index) \
            (int)(((int)group << 8) | (int)(index))

/* NOTE: service ID of 0x0000 is reserved and should never be used */
#define HTC_CTRL_RSVD_SVC MAKE_SERVICE_ID(RSVD_SERVICE_GROUP,1)
#define HTC_LOOPBACK_RSVD_SVC MAKE_SERVICE_ID(RSVD_SERVICE_GROUP,2)
#define WMI_CONTROL_SVC   MAKE_SERVICE_ID(WMI_SERVICE_GROUP,0)

#define WMI_BEACON_SVC	  MAKE_SERVICE_ID(WMI_SERVICE_GROUP,1) 
#define WMI_CAB_SVC	  MAKE_SERVICE_ID(WMI_SERVICE_GROUP,2) 

#define WMI_UAPSD_SVC	  MAKE_SERVICE_ID(WMI_SERVICE_GROUP,3)
#define WMI_MGMT_SVC	  MAKE_SERVICE_ID(WMI_SERVICE_GROUP,4)

#define WMI_DATA_VO_SVC   MAKE_SERVICE_ID(WMI_SERVICE_GROUP,5)
#define WMI_DATA_VI_SVC   MAKE_SERVICE_ID(WMI_SERVICE_GROUP,6)

#define WMI_DATA_BE_SVC   MAKE_SERVICE_ID(WMI_SERVICE_GROUP,7)
#define WMI_DATA_BK_SVC   MAKE_SERVICE_ID(WMI_SERVICE_GROUP,8)

/* raw stream service (i.e. flash, tcmd, calibration apps) */
#define HTC_RAW_STREAMS_SVC MAKE_SERVICE_ID(HTC_TEST_GROUP,0)

#endif /*HTC_SERVICES_H_*/
