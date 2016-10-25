# pppdp-sendmail

A plugin for pptpd that will run script when someone logs in / a client connects to pptpd

Based on pptpd plugin [pptpd-logwtmp.c](https://github.com/chrisballinger/poptop/blob/master/plugins/pptpd-logwtmp.c)

Basically it's a modified copy of this plugin.

### Installation

There is a dependency with pppd.h.

```sudo apt-get install ppp-dev```

Build by running ```make``` in this dir.

cp pptpd-slack-ack.so /usr/lib/pppd/2.4.7/

add plugin in /etc/ppp/pptpd-options (the value of "option" in /etc/pptpd.conf) at the bottom:
```
plugin pptpd-slack-ack.so
```
