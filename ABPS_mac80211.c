 /* ABPS_ieee80211.c */

#include <net/mac80211.h>
#include <net/ieee80211_radiotap.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <linux/rtnetlink.h>
#include <linux/bitmap.h>
#include <net/net_namespace.h>
#include <net/cfg80211.h>
/* ABPS VIC */
#include <net/ip.h>

#include <net/ipv6.h>

#include <linux/syscalls.h>
/* per do_gettimeofday */
#include <linux/time.h>
/* fine ABPS VIC */

#include "ieee80211_i.h"
#include "rate.h"
#include "mesh.h"
#include "wep.h"
#include "wme.h"
#include "aes_ccm.h"
#include "led.h"
#include "cfg.h"
#include "debugfs.h"
#include "debugfs_netdev.h"


/* ABPS VIC */
#ifndef ABPS_DEBUG
#define ABPS_DEBUG
#endif

#ifdef ABPS_DEBUG
static unsigned long int DIFFMSEC(long DOPOSEC, long DOPONANOSEC, long PRIMASEC, long PRIMANANOSEC) {
	long long int primamsec,dopomsec,diffmsec;
	dopomsec=(((long long int)DOPOSEC)*1000L)+(((long long int)DOPONANOSEC)/1000L);
	primamsec=(((long long int)PRIMASEC)*1000L)+(((long long int)PRIMANANOSEC)/1000L);
	diffmsec=dopomsec-primamsec;
	return(  (unsigned long int)diffmsec );
}
#endif

#define WLAN_FC_GET_TYPE(fc) ((fc) & IEEE80211_FCTL_FTYPE)
#define WLAN_FC_GET_STYPE(fc) ((fc) & IEEE80211_FCTL_STYPE)
#define WLAN_GET_SEQ_FRAG(seq) ((seq) & IEEE80211_SCTL_FRAG)

struct ieee80211_hdr_4addr {
	__le16 frame_ctl;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctl;
	u8 addr4[ETH_ALEN];
	u8 payload[0];
} __attribute__ ((packed));


/* forse da mettere in un header a parte */
struct packet {
	u8 acked; /* acked, not acked, no ack requested or ack filtered */
	u8 retry_count; /* retry counter */
	unsigned long filtered_count;
						/* number of times the frame has been filtered */
	u16 fragment_data_len; /* only data, not header */
	u16 fragment_offset;
	u8 more_fragment; /* 0 if the last fragment, 1 otherwise */
	uint32_t ip_id; /* ip packet id */
	__be16 udp_sport; /* source udp port */
};
#define ACK 0
#define ACK_NOT 1
#define ACK_NOT_REQ 2
#define ACK_FILTERED 3
#define ACK_ERROR 5

#define FRAG_LAST 0
#define FRAG_NOT_LAST 1
/* fine forse da mettere in un header a parte */

typedef struct
{
	/* the first 5 parameters follow the order of the network */
	__be16 ipdgramid;
	__be32 saddr;
	__be16 sport;
	__be32 daddr;
	__be16 dport;
	/* the following parameters are used to the client to sort packages */
	u16 fragment_data_len; /* only data, not header */
	u16 fragment_offset;
	u8 more_fragment;
} IPdgramInfo;



static int ABPS_info_counter = 0 ;


struct ABPS_info
{
	__le16 id; /* sequence id frame 802.11 */
	struct packet datagram_info ;
	struct timespec tx_time;
	struct timespec rx_time;
	struct ABPS_info *next;
    int is_ipv6;
};


/* WAT the heck is that ?! */

static struct ABPS_info sentinel = { 0, { 0, 0, 0, 0, 0, 0 }, { 0, 0 }, { 0, 0 }, NULL };
static struct timespec LastCheck_ABPS_info_list={0,0};


/*

 * print the information saved into the ABPS_info
 
*/


//static void ABPS_info_take_response(struct ABPS_info *packet_info)
//{
//
//	if(1) {
//		struct timeval tv;
//		unsigned long msec;
//
//		do_gettimeofday(&tv);
//		msec=((unsigned long)tv.tv_sec)*1000L + (tv.tv_usec/1000L);
//		/*
//		printk(KERN_DEBUG "*** VIC_ABPS *** info_take_response:"
//		" IP ID %d time %d sec %d usec \n" ,
//		ntohs(packet_info->datagram_info.ip_id),
//		tv.tv_sec, tv.tv_usec );
//		*/
///*
//		printk(KERN_DEBUG "*** MURDA VIC_ABPS *** :"
//			" IP ID %20.0d 2time msec %0lu "
//			"retry_count %d "
//			"LOST %d "
//			"info_take_response\n",
//			ntohs(packet_info->datagram_info.ip_id),
//			msec,
//			packet_info->datagram_info.retry_count,
//			packet_info->datagram_info.acked
//			);
// */
//	}
///*
//	printk(KERN_DEBUG "*** MERDA VIC_ABPS *** :"
//		" IP ID %0d difftimemsec %0lu "
//		"retry_count %d "
//		"LOST %d "
//		"verdetto in info_take_response\n",
//		ntohs(packet_info->datagram_info.ip_id),
//		DIFFMSEC(
//			packet_info->rx_time.tv_sec,
//			packet_info->rx_time.tv_nsec,
//			packet_info->tx_time.tv_sec,
//			packet_info->tx_time.tv_nsec
//			),
//		packet_info->datagram_info.retry_count,
//		packet_info->datagram_info.acked
//		);
//
//
//	printk(KERN_DEBUG "*** ABPS *** frame id : %d\n",
//			le16_to_cpu(packet_info->id));
//	printk(KERN_DEBUG "*** ABPS *** datagram id : %u\n",
//            ntohl(packet_info->datagram_info.ip_id));
//	/*
//    printk(KERN_DEBUG "*** ABPS *** source port : %d\n",
//			ntohs(packet_info->datagram_info.udp_sport));
//	printk(KERN_DEBUG "*** ABPS *** more_fragment : %d\n",
//			packet_info->datagram_info.more_fragment);
//	printk(KERN_DEBUG "*** ABPS *** acked : %d\n",
//			packet_info->datagram_info.acked);
//	printk(KERN_DEBUG "*** ABPS *** retry_count : %d\n",
//			packet_info->datagram_info.retry_count);
//	printk(KERN_DEBUG "*** ABPS *** filtered_count : %lud\n",
//			packet_info->datagram_info.filtered_count);
//	printk(KERN_DEBUG "*** ABPS *** tx time : %ld , %ld \n",
//			packet_info->tx_time.tv_sec, packet_info->tx_time.tv_nsec);
//	printk(KERN_DEBUG "*** ABPS *** rx time : %ld , %ld \n",
//			packet_info->rx_time.tv_sec, packet_info->rx_time.tv_nsec);
//	printk(KERN_DEBUG "*** ABPS *** CALCOLOFORSEERRATO diff time : %ld , %ld \n",
//			packet_info->rx_time.tv_sec - packet_info->tx_time.tv_sec,
//  			packet_info->rx_time.tv_nsec - packet_info->tx_time.tv_nsec);
//     */
//}
//#endif


/*
 
 * search into the ABPS_info list the ABPS_info with the field id like
 * the param id if it found it, return this ABPS_info, else return 0

 */

static struct ABPS_info *ABPS_info_search(__le16 id)
{
	struct ABPS_info *aux = &sentinel;
#ifdef ABPS_DEBUG
	int debug_counter = 0;
	printk(KERN_DEBUG "*** ABPS *** COUNTER %d \n", ABPS_info_counter);
#endif
	while (aux->next != NULL)
    {
        if (id == aux->next->id)
        {	/* ABPS_info is found */
            return aux->next;
		}
#ifdef ABPS_DEBUG
		debug_counter++;
		if (debug_counter == 100) {
			printk(KERN_ERR "*** ABPS *** ABPS_info queue too long\n");
			return 0;
		}
#endif
  		aux = aux->next;
	}
	return 0;
}

static void Check_ABPS_info_list(void)
{
	/* faccio il check ogni 5 secs */
	struct timespec now = CURRENT_TIME;

	if(now.tv_sec > (LastCheck_ABPS_info_list.tv_sec+10) ) {
		/* elimino i pkt che stanno in lista da troppo tempo */
		struct ABPS_info *aux = &sentinel;

#ifdef ABPS_DEBUG
		printk(KERN_NOTICE "*** ABPS *** Check_ABPS_info_list\n");
#endif
		while (aux->next != NULL)
        {
			if ( now.tv_sec > (aux->next->tx_time.tv_sec+10) ) {
  				struct ABPS_info *temp = aux->next->next;
                kfree(aux->next);
  				aux->next = temp;
				ABPS_info_counter -- ;
#ifdef ABPS_DEBUG
				printk(KERN_NOTICE "*** ABPS *** Check_ABPS_info_list rimuove uno\n");
#endif
			}
			else
				aux = aux->next;
		}
#ifdef ABPS_DEBUG
		printk(KERN_NOTICE "*** ABPS *** Check_ABPS_info_list rimasti %d info\n",ABPS_info_counter);
#endif
		LastCheck_ABPS_info_list=now;
	}
}

/*
 *add the new element packet_info at the ABPS_info list
 */
static void ABPS_info_add(struct ABPS_info *packet_info )
{
 	Check_ABPS_info_list();
 	if (sentinel.next == NULL)
    { /* empty list */
        sentinel.next = packet_info;
		packet_info->next = NULL;
	} else {
		struct ABPS_info *aux = &sentinel;
 		while (aux->next != NULL) {
 			aux = aux->next;
		}
		aux->next = packet_info;
 		packet_info->next = NULL;
	}
	ABPS_info_counter++ ;
#ifdef ABPS_DEBUG
	printk(KERN_NOTICE "*** ABPS *** ABPS_info_add rimasti %d info\n",ABPS_info_counter);
#endif
}

/*
 * remove the packet_info from the ABPS_info list
 */
static void ABPS_info_remove(struct ABPS_info *packet_info)
{
 	struct ABPS_info *aux = &sentinel;
	while (aux->next != NULL) {
 		if (aux->next->id == packet_info->id ) {
 			struct ABPS_info *temp = aux->next->next;
 			kfree(aux->next);
 			aux->next = temp;
 			ABPS_info_counter -- ;
			break;
		}
 		aux = aux->next;
	}
}


static int ipv6_get_udp_info(struct sk_buff *skb, unsigned char *payload, int data_length, u16 *fragment_data_length, u16 *fragment_offset, u8 *more_fragment)
{
    struct ipv6hdr *payload_iphdr;

    struct udphdr *payload_udphdr;
    
    struct frag_hdr *header_fragment;
    struct frag_hdr _header_fragment;
    
    int result_value;
    
    unsigned int pointer = 0;
    
    unsigned short frag_offset;
    
    
    
    int flags = 0;
    
    if(data_length < sizeof(struct ipv6hdr))
    {
        printk(KERN_DEBUG "*** ABPS *** get_udp_info: data_len less"
               " then IPv6 header length\n");
        return(-3);
    }
    
    payload_iphdr = (struct ipv6hdr *) payload;
    if(payload_iphdr->version != 6)
    {
        printk(KERN_NOTICE "Transmission Error Detector: no IPv6 header in ipv6_get_udp_info");
        return 0;
    }
    
    printk(KERN_NOTICE "payload length %d ", payload_iphdr->payload_len);
    
    printk(KERN_NOTICE "payload length %d ", ntohs(payload_iphdr->payload_len));
    
    
    printk(KERN_NOTICE "nxthdr %d \n",payload_iphdr->nexthdr);
    u8 nexthdr = ipv6_hdr(skb)->nexthdr;
    printk(KERN_NOTICE "nxthdr skb %d \n", nexthdr);
    
    *fragment_offset = 0;
    
    *more_fragment = 0;
    
    /* analyze extension header for fragmentation */
    printk(KERN_NOTICE "search for extension \n");
    
    if(payload_iphdr->nexthdr == NEXTHDR_FRAGMENT)
    {
        header_fragment = (struct frag_hdr *) (payload + sizeof(struct ipv6hdr));
        
        printk(KERN_NOTICE "fragmentation %d \n", header_fragment->frag_off << 3);
        
        printk(KERN_NOTICE "fragmentantion %d \n", (ntohs(header_fragment->frag_off & htons(IP6_OFFSET)))<<3);
        
    }
    
    
    result_value = ipv6_find_hdr(skb, &pointer, IPPROTO_FRAGMENT, NULL, NULL);
    if(result_value < 0)
    {
        printk(KERN_NOTICE "Transmission Error Detector goes wrong getting next header %d \n",result_value);
    }
    
    header_fragment = skb_header_pointer(skb, pointer, sizeof(_header_fragment), &_header_fragment);
    if(header_fragment)
    {
        printk(KERN_NOTICE "header is not null \n");
    }
    else
    {
        printk(KERN_NOTICE "header is null \n");
    }
    
    
    return 1;
}

static int get_udp_info(unsigned char *payload,
                        int data_len,
                        __be32 *saddr,
					    __be32 *daddr,
                        __be16 *sport,
                        __be16 *dport,
				     	__be16 *IPdatagram_id,
                        /* 
                         the following parameters are used
					        to the client to sort packages 
                         */
				    	u16 *fragment_data_len,
                        /* only data, not header */
				    	u16 *fragment_offset,
                        u8 *more_fragment )
{
	struct iphdr *piphdr;
	struct udphdr *pudphdr;

	if (data_len < sizeof(struct iphdr))
    {
#ifdef ABPS_DEBUG
		printk(KERN_DEBUG "*** ABPS *** get_udp_info: data_len less"
				" then IP header length\n");
#endif
		return(-3);
	}
    
	piphdr = (struct iphdr *)payload;
  
    /*   hearder IP   */
    
	if ( piphdr->protocol == IPPROTO_UDP )
    {
		/*   TCP: 0x06 ; UDP: 0x11 ; ICMP: 0x01   **/
		if (data_len < (4*(piphdr->ihl)) )
        {
#ifdef ABPS_DEBUG
			printk(KERN_DEBUG "*** ABPS *** get_udp_info: data_len less"
					"then UDP header length\n");
#endif
			return(-2);
		}
		pudphdr = ((struct udphdr *) (payload + 4*(piphdr->ihl))); /* header UDP */
		if (data_len < (4*(piphdr->ihl)+sizeof(struct udphdr)) )
        {
#ifdef ABPS_DEBUG
			printk(KERN_DEBUG "*** ABPS *** get_udp_info: data_len less "
					"then UDP datagam length\n");
#endif
			return(-1);
		}
		*saddr = piphdr->saddr;
		*daddr = piphdr->daddr;
		*sport = pudphdr->source;
		*dport = pudphdr->dest;
        
		//*IPdatagram_id = piphdr->id;
        
#ifdef ABPS_DEBUG
		printk(KERN_DEBUG "ABPS get_udp_info IP_tot_len %d\n",ntohs(piphdr->tot_len));
#endif
		/* the following parameters are used to the client to sort packages */
		*fragment_data_len = ntohs(piphdr->tot_len) - (4*(piphdr->ihl));
						/* only data, not header */
#ifdef ABPS_DEBUG
		printk(KERN_DEBUG "ABPS get_udp_info frag_len %d\n", *fragment_data_len);
#endif
		*fragment_offset = (ntohs(piphdr->frag_off & htons(IP_OFFSET)))<<3;
#ifdef ABPS_DEBUG
		printk(KERN_DEBUG "ABPS get_udp_info frag_off %d\n", *fragment_offset);
#endif
		*more_fragment = (ntohs(piphdr->frag_off & htons(IP_MF)) > 0);
		return(1);
	}
	/* no udp */
	return(0);
}
/* *** ABPS ***
 * End
 */


/* Extract some information from the header ieee80211, ip and udp: sequence
 * number frame ieee, id datagram ip, source port udp... and put these in the
 * ABPS_info list if return 1 all it's ok.
 */

int ABPS_extract_pkt_info_with_skb(struct ieee80211_hdr *hdr, struct sk_buff *skb)
{
    struct ABPS_info *packet_info;
    struct ieee80211_hdr_4addr *hdr4 = (struct ieee80211_hdr_4addr *)hdr;
    size_t hdrlen;
    u16 fc, type, stype, sc;
    unsigned int frag;
    u8 *payload;
    u8 *IPdatagram;
    u16 ethertype;
    int flen, result_from_get_udp_info;
    IPdgramInfo *p_IPDGInfo;
    
    uint32_t identifier = skb->sk_buff_identifier;
    
    fc = le16_to_cpu(hdr->frame_control) ;
    stype = WLAN_FC_GET_STYPE(fc);
  
    switch (WLAN_FC_GET_TYPE(fc)) {
        case IEEE80211_FTYPE_DATA:
            break;
            return 0;
    }
    p_IPDGInfo = kmalloc(sizeof (IPdgramInfo), GFP_ATOMIC);
    packet_info = kmalloc(sizeof(struct ABPS_info), GFP_ATOMIC);
    
    packet_info->id = hdr->seq_ctrl;
    
    fc = le16_to_cpu(hdr4->frame_ctl);
    type = WLAN_FC_GET_TYPE(fc);
    stype = WLAN_FC_GET_STYPE(fc);
    sc = le16_to_cpu(hdr4->seq_ctl);
    frag = WLAN_GET_SEQ_FRAG(sc);
    /* OLD hdrlen = ieee80211_get_hdrlen(fc); */
    hdrlen = ieee80211_hdrlen(fc);
    
    stype &= ~IEEE80211_STYPE_QOS_DATA;
    
    if (stype != IEEE80211_STYPE_DATA &&
        stype != IEEE80211_STYPE_DATA_CFACK &&
        stype != IEEE80211_STYPE_DATA_CFPOLL &&
        stype != IEEE80211_STYPE_DATA_CFACKPOLL)
        goto rx_dropped;
    
    
    payload = ((u8*) (hdr4)) + hdrlen;
    ethertype = (payload[6] << 8) | payload[7];
    if (ethertype == ETH_P_IP)
    {
        IPdatagram = ((u8*) hdr4) + hdrlen + 8;
        flen = sizeof(struct iphdr) + sizeof(struct udphdr);
        printk(KERN_NOTICE "ABPS Module Current identifier: %d ", identifier);
        result_from_get_udp_info = get_udp_info(IPdatagram,
                                                flen,
                                                &(p_IPDGInfo->saddr),
                                                &(p_IPDGInfo->daddr),
                                                &(p_IPDGInfo->sport),
                           &(p_IPDGInfo->dport), &(p_IPDGInfo->ipdgramid),
                           &(p_IPDGInfo->fragment_data_len),
                           /* only data, not header */
                           &(p_IPDGInfo->fragment_offset),
                           &(p_IPDGInfo->more_fragment));
        if (result_from_get_udp_info > 0) {
            
            /* set the fields of the ABPS_info that will be put in the ABPS_info list */
            
            packet_info->datagram_info.ip_id = identifier;
           
            /* maybe ntohs, not sure */
            
            packet_info->datagram_info.udp_sport = p_IPDGInfo->sport;
            packet_info->datagram_info.fragment_data_len = p_IPDGInfo->fragment_data_len;
            packet_info->datagram_info.fragment_offset = p_IPDGInfo->fragment_offset;
            packet_info->datagram_info.more_fragment = p_IPDGInfo->more_fragment;
            packet_info->is_ipv6 = 0;
            packet_info->tx_time = CURRENT_TIME;
            ABPS_info_add(packet_info);
            return(1);
        }
        return(0);
    }
    else
    {
       if(ethertype == ETH_P_IPV6)
       {
           IPdatagram = ((u8*)hdr4) + hdrlen + 8;
           flen = sizeof(struct ipv6hdr) + sizeof(struct udphdr);

           result_from_get_udp_info = ipv6_get_udp_info(skb,IPdatagram,
                                                        flen,
                                                        &(p_IPDGInfo->fragment_data_len),
                                                        &(p_IPDGInfo->fragment_offset),
                                                        &(p_IPDGInfo->more_fragment));
           if (result_from_get_udp_info > 0)
           {
               /* set the fields of the ABPS_info that will be put in the ABPS_info list*/
               packet_info->datagram_info.ip_id = identifier;
               packet_info->is_ipv6 = 1;
               packet_info->tx_time = CURRENT_TIME;
            
               ABPS_info_add(packet_info);
               
               return(1);
           }
       }
    }
rx_dropped:
    return 0;
}


int ABPS_extract_pkt_info(struct ieee80211_hdr *hdr)
{
	struct ABPS_info *packet_info;
	struct ieee80211_hdr_4addr *hdr4 = (struct ieee80211_hdr_4addr *)hdr;
	size_t hdrlen;
	u16 fc, type, stype, sc;
	unsigned int frag;
	u8 *payload;
	u8 *IPdatagram;
	u16 ethertype;
	int flen;
	IPdgramInfo *p_IPDGInfo;
	fc = le16_to_cpu(hdr->frame_control) ;
	stype = WLAN_FC_GET_STYPE(fc);

	switch (WLAN_FC_GET_TYPE(fc))
    {
		case IEEE80211_FTYPE_DATA:
			break;
			return 0;
	}
	p_IPDGInfo = kmalloc(sizeof (IPdgramInfo), GFP_ATOMIC);
	packet_info = kmalloc(sizeof(struct ABPS_info), GFP_ATOMIC);

    packet_info->id = hdr->seq_ctrl;

	fc = le16_to_cpu(hdr4->frame_ctl);
	type = WLAN_FC_GET_TYPE(fc);
	stype = WLAN_FC_GET_STYPE(fc);
	sc = le16_to_cpu(hdr4->seq_ctl);
	frag = WLAN_GET_SEQ_FRAG(sc);

	/* OLD hdrlen = ieee80211_get_hdrlen(fc); */
	hdrlen = ieee80211_hdrlen(fc);

	stype &= ~IEEE80211_STYPE_QOS_DATA;

	if (stype != IEEE80211_STYPE_DATA &&
		   stype != IEEE80211_STYPE_DATA_CFACK &&
		   stype != IEEE80211_STYPE_DATA_CFPOLL &&
		   stype != IEEE80211_STYPE_DATA_CFACKPOLL)
		goto rx_dropped;

	payload = ((u8*)(hdr4)) + hdrlen;
	ethertype = (payload[6] << 8) | payload[7];
	if (ethertype == ETH_P_IP) {
		int ris;
		IPdatagram = ((u8*)hdr4) + hdrlen + 8;
		flen = sizeof(struct iphdr) + sizeof(struct udphdr);
		ris = get_udp_info(IPdatagram, flen, &(p_IPDGInfo->saddr),
							&(p_IPDGInfo->daddr), &(p_IPDGInfo->sport),
							&(p_IPDGInfo->dport), &(p_IPDGInfo->ipdgramid),
							&(p_IPDGInfo->fragment_data_len),
							/* only data, not header */
							&(p_IPDGInfo->fragment_offset),
							&(p_IPDGInfo->more_fragment));
		if (ris > 0) {
			/* set the fields of the ABPS_info that will be put in the
			 * ABPS_info list*/
			packet_info->datagram_info.ip_id = p_IPDGInfo->ipdgramid;
				/* maybe ntohs, not sure */
			packet_info->datagram_info.udp_sport = p_IPDGInfo->sport;
			packet_info->datagram_info.fragment_data_len = p_IPDGInfo->fragment_data_len;
			packet_info->datagram_info.fragment_offset = p_IPDGInfo->fragment_offset;
			packet_info->datagram_info.more_fragment = p_IPDGInfo->more_fragment;
			packet_info->tx_time = CURRENT_TIME;
			ABPS_info_add(packet_info);
			return(1);
		}
		return(0);
	}
rx_dropped:
		return 0;
}

/*
 * join the information into the hdr with the correct ABPS_info
int ABPS_info_response(struct sock *sk, struct ieee80211_hw *hw, struct ieee80211_hdr *hdr, struct ieee80211_tx_status *status)
 */
int ABPS_info_response(struct sock *sk, struct ieee80211_hw *hw, struct ieee80211_hdr *hdr, struct ieee80211_tx_info *info, struct  ieee80211_sub_if_data *sdata)
{
	int success = 0;
	u8 acked = -1;
	u8 retry_count = -1;
	unsigned long filtered_count = -1;
 	struct ieee80211_local *local = hw_to_local(hw);
	struct ABPS_info *packet_info;
	int i;
 	/* se era richiesto l'ack */
    if (!(info->flags & IEEE80211_TX_CTL_NO_ACK))
    {
		/* e l'ack e' arrivato */
		if (info->flags & IEEE80211_TX_STAT_ACK)
			success=1;
	}
 	/* 
     
     VEDERE SE RIMETTERE A POSTO
	else {
		if (!(info->excessive_retries))
			success=2;
	}
	*/

 	if (info->flags & IEEE80211_TX_CTL_NO_ACK)
    {
		/* ack not required */
		acked = ACK_NOT_REQ;
	}
	else if (info->flags & IEEE80211_TX_STAT_TX_FILTERED)
    {
		/* filtered frame */
		acked = ACK_FILTERED;
	}
	else if (info->flags & IEEE80211_TX_STAT_ACK)
    {
		/* frame acked */
		struct sta_info *sta;
		acked = ACK;

		retry_count = 0;
		/* modifiche per kernel da 2.6.27 in poi */
		for (i = 0; i < IEEE80211_TX_MAX_RATES; i++)
        {
			/* the HW cannot have attempted that rate */
			if (i >= hw->max_rates) { ; }
			else
				retry_count += info->status.rates[i].count;
		}
		if (retry_count > 0)
			retry_count--;

		sta = sta_info_get(sdata, hdr->addr1);
		if (sta)
			filtered_count = sta->tx_filtered_count;
		else
			filtered_count = ACK_ERROR ;
	}
	else
    {
		/* frame not acked, ack not recieved */
        struct sta_info *sta;
		acked = ACK_NOT;

		retry_count = 0;
		/* modifiche per kernel da 2.6.27 in poi */
		for (i = 0; i < IEEE80211_TX_MAX_RATES; i++) {
			/* the HW cannot have attempted that rate */
			if (i >= hw->max_rates) { ; }
			else
				retry_count += info->status.rates[i].count;
		}
		if (retry_count > 0)
			retry_count--;

     	sta = sta_info_get(sdata, hdr->addr1);
		if (sta) filtered_count = sta->tx_filtered_count;
		else filtered_count = ACK_ERROR;
	}
 
    packet_info = ABPS_info_search(hdr->seq_ctrl);
    
	if (packet_info != 0)
    {
      	packet_info->datagram_info.acked = acked;
		packet_info->datagram_info.retry_count = retry_count;

		packet_info->rx_time = CURRENT_TIME;
        /* mando la notifica al socket */
        /* NOTA ABPS DIE KURO: adesso estrae solo data_len, offset e more_frags, comunque non potevo
        estrearre dati da udp in caso di frammentazione, l'indirizzo ip invece non e'
        invece mai propagato fino all'utente */
        
        if(!packet_info->is_ipv6)
        {
            ip_local_error_notify(sk,
                                  success,
                                  packet_info->datagram_info.ip_id,
                                  packet_info->datagram_info.fragment_data_len,
                                  packet_info->datagram_info.fragment_offset,
                                  packet_info->datagram_info.more_fragment,
                                  packet_info->datagram_info.retry_count
                                  );
        }
        else
        {
            ipv6_local_error_notify(sk,
                                    success,
                                    packet_info->datagram_info.ip_id,
                                    packet_info->datagram_info.fragment_data_len,
                                    packet_info->datagram_info.fragment_offset,
                                    packet_info->datagram_info.more_fragment,
                                    packet_info->datagram_info.retry_count);
        }
        
//#ifdef ABPS_DEBUG
//		ABPS_info_take_response(packet_info);
//#endif
    	ABPS_info_remove(packet_info);
		return(1);
	}
	return(0);
}

/* fine ABPS VIC */

