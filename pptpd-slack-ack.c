/*
 * $Id: pptpd-sendmail.c,v 0.1 2015/28/06 18:04:39 mdvanes Exp $
 * pptpd-sendmail.c - pppd plugin to send mail for a pptpd user
 *
 * 2015 M.D. van Es - based on pptpd-logwtmp by James Cameron.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */
#include <unistd.h>
#include <utmp.h>
#include <stdio.h>
#include <string.h>
#include <pppd/pppd.h>

char pppd_version[] = VERSION;

static char pptpd_original_ip[PATH_MAX+1];
/* TODO remove pptpd_logwtmp_strip_domain */
static bool pptpd_logwtmp_strip_domain = 0;

static option_t options[] = {
  { "pptpd-original-ip",          /* name from pppd.h */
    o_string,                     /* type */
    pptpd_original_ip,            /* addr */
    "Original IP address of the PPTP connection", /* description */
    OPT_STATIC,                   /* flags */
    NULL,                         /* addr2 */
    PATH_MAX                      /* upper_limit */
  },
  { "pptpd-logwtmp-strip-domain", o_bool, &pptpd_logwtmp_strip_domain,
    "Strip domain from username before logging", OPT_PRIO | 1 },
  { NULL }
};

static char *reduce(char *user)
{
  char *sep;
  if (!pptpd_logwtmp_strip_domain) return user;

  sep = strstr(user, "//"); /* two slash */
  if (sep != NULL) user = sep + 2;
  sep = strstr(user, "\\"); /* or one backslash */
  if (sep != NULL) user = sep + 1;
  return user;
}

static void runScript(char user[], char ifname[]) {
    char cmd[100];  // to hold the command.
    sprintf(cmd,"slack-vpn-ack.py %s %s",user,ifname); // prepare command.
    system(cmd);     // execute it.
}

static void ip_up(void *opaque, int arg)
{
  char *user = reduce(peer_authname);
  runScript(user,ifname);
}

static void ip_down(void *opaque, int arg)
{
  if (debug) 
    /* notice - pppd.h; log a notice-level message */
    notice("pptpd-slack-ack ip-down %s", ifname);
  /*logwtmp(ifname, "", "");*/
}

void plugin_init(void)
{
  //printf("pptpd-sendmail: plugin_init\n");
  add_options(options);
  add_notifier(&ip_up_notifier, ip_up, NULL);
  add_notifier(&ip_down_notifier, ip_down, NULL);
  if (debug) 
    /* notice - pppd.h; log a notice-level message */
    notice("pptpd-slack-ack: $Version$");
}
