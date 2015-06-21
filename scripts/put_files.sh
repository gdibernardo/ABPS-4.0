#!/bin/sh

CP=cp

$CP -vaa ../ABPS_mac80211.c       $1/net/mac80211/ABPS_mac80211.c
$CP -vaa ../ABPS_mac80211.h       $1/net/mac80211/ABPS_mac80211.h
$CP -vaa ../status.c              $1/net/mac80211/status.c
$CP -vaa ../tx.c                  $1/net/mac80211/tx.c
$CP -vaa ../skbuff.c              $1/net/core/skbuff.c
$CP -vaa ../skbuff.h              $1/include/linux/skbuff.h

$CP -vaa ../uapi/errqueue.h       $1/include/uapi/linux/errqueue.h
$CP -vaa ../uapi/errqueue.h       /usr/include/linux/errqueue.h
$CP -vaa ../uapi/socket.h         $1/include/uapi/linux/socket.h
$CP -vaa ../uapi/socket.h         /usr/include/linux/socket.h

$CP -vaa ../Makefile              $1/net/mac80211/Makefile
$CP -vaa ../socket.c              $1/net/socket.c


$CP -vaa ../ipv4/ip_output.c      $1/net/ipv4/ip_output.c
$CP -vaa ../ipv4/udp.c            $1/net/ipv4/udp.c
$CP -vaa ../ipv6/ip6_output.c     $1/net/ipv6/ip6_output.c
$CP -vaa ../ipv6/udp.c            $1/net/ipv6/udp.c

$CP -va ../ipv4/ip.h              $1/include/net/ip.h
$CP -va ../socket.h               $1/include/linux/socket.h



$CP -va ../ipv4/ip_sockglue.c     $1/net/ipv4/ip_sockglue.c
