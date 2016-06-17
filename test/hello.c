#include <stdio.h>
#include <dlfcn.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <systemd/sd-bus.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include "ipmid.H"
#include <sys/time.h>
#include <errno.h>
#include "sensorhandler.h"

sd_bus *bus = NULL;
sd_bus_slot *ipmid_slot = NULL;

FILE *ipmiio, *ipmidbus, *ipmicmddetails;

void print_usage(void) {
  fprintf(stderr, "Options:  [-d mask]\n");
  fprintf(stderr, "    mask : 0x01 - Print ipmi packets\n");
  fprintf(stderr, "    mask : 0x02 - Print DBUS operations\n");
  fprintf(stderr, "    mask : 0x04 - Print ipmi command details\n");
  fprintf(stderr, "    mask : 0xFF - Print all trace\n");
}

const char * DBUS_INTF = "org.openbmc.HostIpmi";

const char * FILTER = "type='signal',interface='org.openbmc.HostIpmi',member='ReceivedMessage'";


typedef std::pair<ipmi_netfn_t, ipmi_cmd_t> ipmi_fn_cmd_t;
typedef std::pair<ipmid_callback_t, ipmi_context_t> ipmi_fn_context_t;

// Global data structure that contains the IPMI command handler's registrations.
std::map<ipmi_fn_cmd_t, ipmi_fn_context_t> g_ipmid_router_map;

// IPMI Spec, shared Reservation ID.
unsigned short g_sel_reserve = 0xFFFF;

unsigned short get_sel_reserve_id(void)
{
    return g_sel_reserve;
}
