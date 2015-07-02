#ifndef __ABPS_MAC80211_H__
#define __ABPS_MAC80211_H__

/* ABPS VIC */

#include <linux/ieee80211.h>	/* per definizione struct ieee80211_hdr */
#include <net/mac80211.h>			/* per definizione struct ieee80211_hw */
															/* per definizione struct ieee80211_tx_info */
															/* che sostituisce ieee80211_tx_status */
#include <net/sock.h>					/* per definizione struct sock  */



#include <linux/skbuff.h>

#include "ieee80211_i.h"

/* not used in kernel Linux 4.0 */
extern int ABPS_extract_pkt_info(struct ieee80211_hdr *hdr);

extern int ABPS_extract_pkt_info_with_skb(struct ieee80211_hdr *hdr, struct sk_buff *skb);

/* usato in net/mac80211/main.c  */
extern int ABPS_info_response(struct sock *sk, struct ieee80211_hw *hw, struct ieee80211_hdr *hdr, struct ieee80211_tx_info *info, struct ieee80211_sub_if_data *sdata);

#endif  /*  __ABPS_MAC80211_H__ */


