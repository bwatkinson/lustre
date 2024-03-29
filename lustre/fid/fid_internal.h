/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2007, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
 * Copyright (c) 2012, 2014, Intel Corporation.
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 *
 * lustre/fid/fid_internal.h
 *
 * Author: Yury Umanets <umka@clusterfs.com>
 */
#ifndef __FID_INTERNAL_H
#define __FID_INTERNAL_H

#include <libcfs/libcfs.h>

#ifdef HAVE_SERVER_SUPPORT
# define HAVE_SEQ_SERVER

struct req_capsule;

struct seq_thread_info {
	struct req_capsule	*sti_pill;
	struct lu_seq_range	sti_space;
	struct lu_buf		sti_buf;
};

enum {
	SEQ_TXN_STORE_CREDITS = 20
};

extern struct lu_context_key seq_thread_key;

extern struct ldebugfs_vars seq_server_debugfs_list[];

/* Store API functions. */
struct dt_device;

int seq_store_init(struct lu_server_seq *seq,
		   const struct lu_env *env,
		   struct dt_device *dt);

void seq_store_fini(struct lu_server_seq *seq,
		    const struct lu_env *env);

int seq_store_read(struct lu_server_seq *seq,
		   const struct lu_env *env);

int seq_store_update(const struct lu_env *env, struct lu_server_seq *seq,
		     struct lu_seq_range *out, int sync);

int seq_server_alloc_spec(struct lu_server_seq *seq,
			  struct lu_seq_range *spec,
			  const struct lu_env *env);

int fid_server_mod_init(void);

void fid_server_mod_exit(void);

# endif /* HAVE_SERVER_SUPPORT */

/* Functions used internally in module. */
int seq_client_alloc_super(struct lu_client_seq *seq,
			   const struct lu_env *env);

extern struct dentry *seq_debugfs_dir;

extern struct ldebugfs_vars seq_client_debugfs_list[];

extern const struct file_operations seq_fld_debugfs_seq_fops;

#endif /* __FID_INTERNAL_H */
