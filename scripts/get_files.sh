#!/bin/sh

#CP=$CP

CP=cp


$CP -v $1/net/ipv4/udp.c			../ipv4/
$CP -v $1/net/ipv4/ip_output.c			../ipv4/
$CP -v $1/net/ipv4/ip_sockglue.c		../ipv4/
$CP -v $1/include/net/ip.h			../ipv4/


$CP -v $1/net/ipv6/udp.c			../ipv6/
$CP -v $1/net/ipv6/ip6_output.c			../ipv6/


$CP -v $1/include/uapi/linux/errqueue.h		../uapi/
$CP -v $1/include/uapi/linux/socket.h		../uapi/

$CP -v $1/net/mac80211/tx.c			../
$CP -v $1/net/mac80211/main.c			../
$CP -v $1/net/mac80211/Makefile			../
$CP -v $1/net/mac80211/ABPS_mac80211.h		../
$CP -v $1/net/mac80211/ABPS_mac80211.c		../
$CP -v $1/net/mac80211/status.c			../



$CP -v $1/include/linux/skbuff.h		../

$CP -v $1/net/core/skbuff.c			../


