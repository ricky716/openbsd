/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Interface for the packet protocol functions.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

/* RCSID("$OpenBSD: src/usr.bin/ssh/packet.h,v 1.25 2001/06/26 17:27:24 markus Exp $"); */

#ifndef PACKET_H
#define PACKET_H

#include <openssl/bn.h>

void     packet_set_connection(int, int);
void     packet_set_nonblocking(void);
int      packet_get_connection_in(void);
int      packet_get_connection_out(void);
void     packet_close(void);
void	 packet_set_encryption_key(const u_char *, u_int, int);
void     packet_set_protocol_flags(u_int);
u_int	 packet_get_protocol_flags(void);
void     packet_start_compression(int);
void     packet_set_interactive(int);
int      packet_is_interactive(void);

void     packet_start(u_char);
void     packet_put_char(int ch);
void     packet_put_int(u_int value);
void     packet_put_bignum(BIGNUM * value);
void     packet_put_bignum2(BIGNUM * value);
void     packet_put_string(const char *buf, u_int len);
void     packet_put_cstring(const char *str);
void     packet_put_raw(const char *buf, u_int len);
void     packet_send(void);

int      packet_read(int *payload_len_ptr);
void     packet_read_expect(int *payload_len_ptr, int type);
int      packet_read_poll(int *packet_len_ptr);
void     packet_process_incoming(const char *buf, u_int len);

u_int	 packet_get_char(void);
u_int	 packet_get_int(void);
void     packet_get_bignum(BIGNUM * value, int *length_ptr);
void     packet_get_bignum2(BIGNUM * value, int *length_ptr);
char	*packet_get_raw(int *length_ptr);
char    *packet_get_string(u_int *length_ptr);
void     packet_disconnect(const char *fmt,...) __attribute__((format(printf, 1, 2)));
void     packet_send_debug(const char *fmt,...) __attribute__((format(printf, 1, 2)));

void     packet_write_poll(void);
void     packet_write_wait(void);
int      packet_have_data_to_write(void);
int      packet_not_very_much_data_to_write(void);

int	 packet_connection_is_on_socket(void);
int	 packet_connection_is_ipv4(void);
int	 packet_remaining(void);
void	 packet_send_ignore(int);
void	 packet_inject_ignore(int);

void	 tty_make_modes(int, struct termios *);
void	 tty_parse_modes(int, int *);

extern int max_packet_size;
int      packet_set_maxsize(int);
#define  packet_get_maxsize() max_packet_size

#define packet_integrity_check(payload_len, expected_len, type) \
do { \
	int _p = (payload_len), _e = (expected_len); \
	if (_p != _e) { \
		log("Packet integrity error (%d != %d) at %s:%d", \
		    _p, _e, __FILE__, __LINE__); \
		packet_disconnect("Packet integrity error. (%d)", (type)); \
	} \
} while (0)

#define packet_done() \
do { \
	int _len = packet_remaining(); \
	if (_len > 0) { \
		log("Packet integrity error (%d bytes remaining) at %s:%d", \
		    _len ,__FILE__, __LINE__); \
		packet_disconnect("Packet integrity error."); \
	} \
} while (0)

#endif				/* PACKET_H */
