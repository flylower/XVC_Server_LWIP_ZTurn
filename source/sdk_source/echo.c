/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

#include "Xil_io.h"

#include "Queue.h"

//#define XVC_DEBUG
QUEUE recvhead;
//Deque sndhead = {NULL, NULL};

uint8_t reply_buf[1024];
uint8_t cmd_buf[32];
uint8_t data_buf[2048];
uint8_t ln_buf[4];
uint32_t reply_len;
uint32_t cmd_len;
uint32_t data_len;
uint32_t ln_len;
unsigned long nsperiod = 0;
#define MAX_PACKET_LEN 1024
#ifndef XVC_VERSION
#define XVC_VERSION 10
#endif

#define DBG_HUB_BASE     0x43C00000
#define DBG_LENGTH       (DBG_HUB_BASE+0x00)
#define DBG_TMS_VECTOR   (DBG_HUB_BASE+0x04)
#define DBG_TDI_VECTOR   (DBG_HUB_BASE+0x08)
#define DBG_TDO_VECTOR   (DBG_HUB_BASE+0x0C)
#define DBG_CTRL         (DBG_HUB_BASE+0x10)

static struct tcp_pcb *gbl_pcb;
static err_t xvc_start(DP *recv_dp);

int Que_Init() {
//	Init_queue(&sndhead);
	Init_queue(&recvhead);
	return 0;
}

int transfer_data() {
	static unsigned int prev_id = 0;
	err_t err;
//	if(recvhead.front != NULL)
//	{
		if(!Isempty(&recvhead))
		{
			DP dp;
			Out_queue(&recvhead, &dp);
			err = xvc_start(&dp);
			free(dp.payload);
			if(err!=0)
			{

			}
			if(prev_id + 1 != dp.id)
				xil_printf("Lost %d, packet\n\r", dp.id - prev_id);
			else {
//				xil_printf("Packet %d:", prev_id);
				prev_id = dp.id;
			}
		}
//	}
	return 0;
}

void print_app_header() {
#if (LWIP_IPV6==0)
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
#else
	xil_printf("\n\r\n\r-----lwIPv6 TCP echo server ------\n\r");
#endif
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	/* do not read the packet if we are not in ESTABLISHED state */
	static unsigned int id = 0;
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);
//	xil_printf("cmd:%s\n\r", p->payload);
	/* echo back the payload */
	/* in this case, we assume that the payload is < TCP_SND_BUF */
	if (tcp_sndbuf(tpcb) > p->len) {
//		if(sndhead.front == NULL)
//			Init_queue(&sndhead);
//		else if(recvhead.front == NULL)
//			Init_queue(&recvhead);
//		else
//		{
			unsigned char *temp;
			temp = (unsigned char *)malloc(sizeof(unsigned char)*(p->len+1));
//			memset(temp, 0, p->len+1);
			if(temp == NULL)
			{
				xil_printf("Dynamic memory allocation failed!\n");
				exit(0);
			}
			for(int i=0; i<p->len; i++)
				temp[i]=*((unsigned char *)p->payload+i);
			temp[p->len+1]=0;
//			memcpy(temp, p->payload, p->len);
			id ++;
			DP dp = {id, temp, p->len};
			In_queue(&recvhead, dp);

//		}
//		xvc_start(tpcb, p->payload, p->len, err);
//		err = tcp_write(tpcb, p->payload, p->len, 1);
	} else
		xil_printf("no space in tcp_sndbuf\n\r");

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	gbl_pcb = newpcb;
	/* just use an integer number indicating the connection id as the
	 callback argument */
	tcp_arg(newpcb, (void*) (UINTPTR) connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}

int start_application() {
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 2542;

	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!(pcb)) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}
	pcb->flags = TF_NODELAY;
	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);
	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}

static unsigned get_uint_le(void * buf, int len) {
	uint8_t * p = (u8_t *) buf;
	unsigned value = 0;

	while (len-- > 0) {
		value = (value << 8) | p[len];
	}
	return value;
}

static void set_uint_le(void * buf, int len, unsigned value) {
	uint8_t * p = (uint8_t *) buf;

	while (len-- > 0) {
		*p++ = (uint8_t) value;
		value >>= 8;
	}
}

static void shift_tms_tdi(uint32_t len, uint8_t* inbuf, uint8_t *result) {
	int nr_bytes = (len + 7) / 8;
	int bytesLeft = nr_bytes;
	int bitsLeft = len;
	int byteIndex = 0;
	int tdi, tms, tdo;
	memset(result, 0, nr_bytes);

	while (bytesLeft > 0) {
		tms = 0;
		tdi = 0;
		tdo = 0;
		if (bytesLeft >= 4) {
//		if (bitsLeft >= 32) {
			memcpy(&tms, &inbuf[byteIndex], 4);
			memcpy(&tdi, &inbuf[byteIndex + nr_bytes], 4);
			Xil_Out32(DBG_TMS_VECTOR, tms);
			Xil_Out32(DBG_TDI_VECTOR, tdi);
			Xil_Out32(DBG_LENGTH, 32);
			Xil_Out32(DBG_CTRL, 0x1);
			while (Xil_In32(DBG_CTRL) != 0)
				;
			tdo = Xil_In32(DBG_TDO_VECTOR);
			memcpy(&result[byteIndex], &tdo, 4);
			bytesLeft -= 4;
			bitsLeft -= 32;
			byteIndex += 4;
#ifdef XVC_DEBUG
			xil_printf("bitsLeft : %d", bitsLeft);
			xil_printf("LEN : 0x%02x", 32);
			xil_printf("TMS : 0x%08x", tms);
			xil_printf("TDI : 0x%08x", tdi);
			xil_printf("TDO : 0x%08x", tdo);
			xil_printf("\n\r");
#endif
		} else {
			memcpy(&tms, &inbuf[byteIndex], bytesLeft);
			memcpy(&tdi, &inbuf[byteIndex + nr_bytes], bytesLeft);
			Xil_Out32(DBG_TMS_VECTOR, tms);
			Xil_Out32(DBG_TDI_VECTOR, tdi);
			Xil_Out32(DBG_LENGTH, 32);
			Xil_Out32(DBG_CTRL, 0x1);
			while (Xil_In32(DBG_CTRL) != 0)
				;
			tdo = Xil_In32(DBG_TDO_VECTOR);
			memcpy(&result[byteIndex], &tdo, bytesLeft);
#ifdef XVC_DEBUG
			xil_printf("bitsLeft : %d", bitsLeft);
			xil_printf("LEN : 0x%02x", bitsLeft);
			xil_printf("TMS : 0x%08x", tms);
			xil_printf("TDI : 0x%08x", tdi);
			xil_printf("TDO : 0x%08x", tdo);
			xil_printf("\n\r");
#endif
			break;
		}
	}
}

static err_t xvc_start(DP *recv_dp) {
	err_t err;
	DP snd_dp;
	unsigned char *p;
	unsigned char *pe;
	int enable_status = 0;
	int pending_error[32] = { 0 };
	p = recv_dp->payload;
	pe = p+recv_dp->len;
//	xil_printf("LEN: %d\n\r", in_len);
read_more:
	for (int i = 0; i < pe-p; i++) {
#ifdef XVC_DEBUG
		xil_printf("%c", *(p+i));
#endif
		if ((*(p + i)) == ':') {
			cmd_buf[i] = *(p+i);
			cmd_len = i + 1;
			break;
		}
		cmd_buf[i] = *(p+i);
	}
	p = p + cmd_len;
	if (cmd_len == 8 && memcmp(cmd_buf, "getinfo:", cmd_len) == 0) {
		snprintf((char *) reply_buf, 100, "xvcServer_v%u.%u:%u\n", XVC_VERSION / 10, XVC_VERSION % 10, MAX_PACKET_LEN);
		xil_printf("%s\n\r", reply_buf);
		reply_len = strlen((char *) reply_buf);
		goto reply;
	}
	if (cmd_len == 7 && memcmp(cmd_buf, "settck:", cmd_len) == 0) {
		unsigned long resnsperiod;
		if (nsperiod != 0)
			resnsperiod = nsperiod;
		else
			resnsperiod = get_uint_le(p, 4);
		// if (!pending_error[0])
		// set_tck(c->client_data, nsperiod, &resnsperiod);
		// if (pending_error[0])
		// resnsperiod = nsperiod;
		p+=4;
		set_uint_le(reply_buf, 4, resnsperiod);
		reply_len = 4;
		goto reply_with_optional_status;
	}
	if (cmd_len == 6 && memcmp(cmd_buf, "shift:", cmd_len) == 0) {
#ifdef XVC_COUNT_EN
		// gettimeofday(&start, NULL); //TODO:
#endif
		unsigned bits;
		unsigned bytes;
		bits = get_uint_le(p, 4);
		bytes = (bits + 7) / 8;
		p += 4;
#ifdef XVC_DEBUG
		xil_printf("\nBITS NUM: %d\n", bits);
#endif
		// assert(bytes * 2 <= max_packet_len);
		if (pe-p < 2 * bytes)
			return ERR_MEM;
		if (!pending_error[0]) {
			// fprintf(stdout, "bits received %d %d %x %x\n", bits, bytes, p[0], p[bytes]);
			shift_tms_tdi(bits, p, reply_buf);
			p+=2*bytes;
		} else {
			xil_printf("XVC: Problem\n");
			memset(reply_buf, 0, bytes);
		}
		reply_len = bytes;

		goto reply_with_optional_status;
	}
	reply_with_optional_status: if (!enable_status)
		goto reply;
#if XVC_VERSION >= 11
	reply_with_status:
	reply_status(c);
#endif
	reply:
	// }
	if (cmd_len) {
		// if (c->handlers->flush)
		// if (c->handlers->flush(c->client_data) < 0) goto error;
		err = tcp_write(gbl_pcb, (void *)reply_buf, reply_len, 1);
		if (err) {
			xil_printf("Reply Error: %d", err);
			return err;
		} else {
//			xil_printf("Send Packet!");
		}
		err = tcp_output(gbl_pcb);
		if (err) {
			xil_printf("Force Reply Error: %d", err);
			return err;
		} else {
//			xil_printf("Send Packet!");
		}
	}

	cmd_len = 0;
	if(pe > p)
		goto read_more;
	//                        LDateTime.getRtc(&stop);
	// if (start.tv_usec != 0)
	//     printf("Shift send packet %lu u-seconds\n", stop.tv_usec - start.tv_usec);
	//          }
	return ERR_OK;
//	error:
////	      xil_printf("XVC connection terminated: error %d\n", errno);
//	xil_printf("Error");
//	cmd_len = 0;
//	return -1;
}

