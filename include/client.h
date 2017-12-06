#pragma once
#include <s2n.h>
#include <stdbool.h>

#include "frame.h"
#include "buf_chain.h"

#define CLIENT_EPOLL_EVENTS (EPOLLIN | EPOLLET | EPOLLRDHUP)

struct client {
	int fd; // This must be first, because of a little epoll hack we use
	int timer_fd;
	struct s2n_connection *tls;
	enum client_state {
		HH_IDLE, // Connection is ready for general HTTP/2 use
		HH_NEGOTIATING_TLS, // Connection initiated, doing TLS negotiation
		HH_WAITING_MAGIC, // Waiting for client to send connection preface
		HH_WAITING_SETTINGS, // Waiting for client to send initial SETTINGS frame
		HH_BLINDED, // Blinded by s2n, waiting for timer to expire
		HH_TLS_SHUTDOWN, // Sent TLS alert, waiting for acknowledgement
		HH_CLOSED, // Already closed (client disconnected)
	} state;
	s2n_blocked_status blocked;
	bool is_write_blocked;
	struct ib_frame ib_frame;
	struct buf_chain *pending_writes;
};

struct client *client_new(int, int, int);
void client_free(struct client *);
int close_client(struct client *);

int client_on_timer_expired(struct client *);
int client_on_write_ready(struct client *);
int client_on_data_received(struct client *);
