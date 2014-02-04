//
//  netfat.cpp
//  netfat
//
//  Created by tess3ract on 2/4/14.
//  Copyright (c) 2014 tess3ract. All rights reserved.
//

#include <mach/mach_types.h>
#include <mach/task.h>
#include <mach/semaphore.h>
#include <sys/systm.h>
#include <sys/kernel_types.h>
#include <sys/kern_event.h>
#include <sys/sysctl.h>
#include <net/kpi_interface.h>
#include <net/kpi_interfacefilter.h>

extern "C" kern_return_t netfat_start(kmod_info_t * ki, void *d);
extern "C" kern_return_t netfat_stop(kmod_info_t *ki, void *d);

interface_filter_t p_filter = NULL;
ifnet_t p_ifnet = NULL;

int oid_registered;
int connect_trigger;
int disconnect_trigger;

SYSCTL_INT(_kern, OID_AUTO, netfat_connect_trigger, CTLFLAG_RW,
           &connect_trigger, 0, "Netfat connect trigger");
SYSCTL_INT(_kern, OID_AUTO, netfat_disconnect_trigger, CTLFLAG_RW,
           &disconnect_trigger, 0, "Netfat disconnect trigger");

void netfat_detached_handler(void *cookie, ifnet_t interface)
{
    
}

void netfat_event_handler(void *cookie, ifnet_t interface,
                          protocol_family_t protocol, const struct kev_msg *event_msg)
{
    printf("NetFat: received event [vendor_code = %u, kev_class = %u, kev_subclass = %u, event_code = %u]\n", event_msg->vendor_code, event_msg->kev_class, event_msg->kev_class, event_msg->event_code);
    // handle post-up event
    if (event_msg->event_code == 13) {
        connect_trigger = 1;
    }
    // handle post-down event
    if (event_msg->event_code == 12) {
        disconnect_trigger = 1;
    }
}

extern "C" kern_return_t netfat_start(kmod_info_t * ki, void *d)
{
    struct iff_filter filter;
    errno_t err = ifnet_find_by_name("en0", &p_ifnet);
    oid_registered = 0;
    if (err)
    {
        printf("NetFat: interface en0 not found\n");
        return KERN_SUCCESS;
    }
    filter.iff_cookie   = 0;
    filter.iff_name     = "NetFatFlt";
    filter.iff_protocol = 0;
    filter.iff_input    = NULL;
    filter.iff_output   = NULL;
    filter.iff_event    = netfat_event_handler;
    filter.iff_ioctl    = NULL;
    filter.iff_detached = netfat_detached_handler;
    iflt_attach(p_ifnet, &filter, &p_filter);
    sysctl_register_oid(&sysctl__kern_netfat_connect_trigger);
    sysctl_register_oid(&sysctl__kern_netfat_disconnect_trigger);
    oid_registered = 1;
    printf("NetFat: ready to serve ;-)\n");
    return KERN_SUCCESS;
}

extern "C" kern_return_t netfat_stop(kmod_info_t *ki, void *d)
{
    if (p_ifnet != NULL)
        ifnet_release(p_ifnet);
    if (p_filter != NULL)
        iflt_detach(p_filter);
    if (oid_registered) {
        sysctl_unregister_oid(&sysctl__kern_netfat_connect_trigger);
        sysctl_unregister_oid(&sysctl__kern_netfat_disconnect_trigger);
    }
    return KERN_SUCCESS;
}
