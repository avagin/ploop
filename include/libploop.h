/*
 *  Copyright (C) 2008-2012, Parallels, Inc. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _LIBPLOOP_H_
#define _LIBPLOOP_H_
#pragma GCC visibility push(default)

#include <linux/types.h>

#define DISKDESCRIPTOR_XML      "DiskDescriptor.xml"

#define PLOOP_LEAVE_TOP_DELTA	0x01

enum ploop_image_mode {
	PLOOP_EXPANDED_MODE = 0,
	PLOOP_EXPANDED_PREALLOCATED_MODE = 1,
	PLOOP_RAW_MODE = 2,
};

struct ploop_mount_param {
	char device[64];
	int ro;
	int flags;
	int unused1;
	char *fstype;
	char *target;
	char *guid;
	int quota;
	char *mount_data;
	unsigned int blocksize; /* blocksize for raw image */
	char dummy[32];
};

struct ploop_create_param {
	unsigned long long size;
	int mode;
	char *image;
	char *fstype;
	int without_partition;
	unsigned int blocksize;
	char dummy[32];
};

struct ploop_image_data {
	char *guid;
	char *file;
};

struct ploop_snapshot_data {
	char *guid;
	char *parent_guid;
};

struct ploop_disk_images_runtime_data;

struct ploop_disk_images_data {
	unsigned long long size;
	unsigned int heads;
	unsigned int cylinders;
	unsigned int sectors;
	int mode;
	int nimages;
	struct ploop_image_data **images;
	char *top_guid;
	int nsnapshots;
	struct ploop_snapshot_data **snapshots;
	struct ploop_disk_images_runtime_data *runtime;
	unsigned int blocksize;
	char dummy[32];
};

struct ploop_resize_param {
	unsigned long long size;
	char dummy[32];
};

struct ploop_snapshot_param {
	char *guid;	/* guid for new snapshot, autogenerated if NULL */
	int snap_guid;	/* guid selector: is new guid for new top_delta or for current top_delta  */
	char dummy[32];
};

struct ploop_merge_param {
	int unused1;
	int merge_all;
	const char *guid;
	char dummy[32];
};

struct ploop_info {
	unsigned long long fs_bsize;
	unsigned long long fs_blocks;
	unsigned long long fs_bfree;
	unsigned long long fs_inodes;
	unsigned long long fs_ifree;
};

struct ploop_cancel_handle;

#ifdef __cplusplus
extern "C" {
#endif

int ploop_getdevice(int *minor);
int ploop_read_diskdescriptor(const char *dir, struct ploop_disk_images_data *di);
int ploop_set_component_name(struct ploop_disk_images_data *di,
		const char *component_name);
struct ploop_disk_images_data *ploop_alloc_diskdescriptor(void);
void ploop_free_diskdescriptor(struct ploop_disk_images_data *di);
char *ploop_get_base_delta_uuid(struct ploop_disk_images_data *di);
int ploop_get_top_delta_fname(struct ploop_disk_images_data *di, char *out, int len);
int ploop_store_diskdescriptor(const char *fname, struct ploop_disk_images_data *di);
int ploop_find_dev_by_delta(char *delta, char *buf, int size);
int ploop_get_dev_by_mnt(const char *path, char *buf, int size);
int ploop_get_mnt_by_dev(const char *dev, char *buf, int size);
int ploop_get_dev(struct ploop_disk_images_data *di, char *out, int len);
int ploop_get_partition_by_mnt(const char *path, char *buf, int size);
int ploop_create_image(struct ploop_create_param *param);
int ploop_mount_image(struct ploop_disk_images_data *di, struct ploop_mount_param *param);
int ploop_mount_snapshot(struct ploop_disk_images_data *di, struct ploop_mount_param *param);
int ploop_umount(const char *device, struct ploop_disk_images_data *di);
int ploop_umount_image(struct ploop_disk_images_data *di);
int ploop_resize_image(struct ploop_disk_images_data *di, struct ploop_resize_param *param);
int ploop_convert_image(struct ploop_disk_images_data *di, int mode, int flags);
int ploop_get_info(struct ploop_disk_images_data *di, struct ploop_info *info);
int ploop_get_info_by_descr(const char *descr, struct ploop_info *info);
int ploop_create_snapshot(struct ploop_disk_images_data *di, struct ploop_snapshot_param *param);
int ploop_merge_snapshot(struct ploop_disk_images_data *di, struct ploop_merge_param *param);
int ploop_switch_snapshot(struct ploop_disk_images_data *di, const char *uuid, int flags);
int ploop_delete_snapshot(struct ploop_disk_images_data *di, const char *guid);
int ploop_delete_top_delta(struct ploop_disk_images_data *di);
int ploop_find_top_delta_name_and_format(
		const char *device,
		char *image,
		size_t image_size,
		char *format,
		size_t format_size);
char *ploop_find_parent_by_guid(struct ploop_disk_images_data *di, const char *guid);
int ploop_uuid_generate(char *uuid, int len);

const char *ploop_get_last_error(void);
int ploop_set_log_file(const char *fname);
/* set log file logging level */
void ploop_set_log_level(int level);
/* set console logging level */
void ploop_set_verbose_level(int level);
/* Constants to be used for ploop_set_verbose_level(): */
#define PLOOP_LOG_NOCONSOLE	-2	/* disable all console logging */
#define PLOOP_LOG_NOSTDOUT	-1	/* disable all but errors to stderr */

/* Cancelation API */
struct ploop_cancel_handle *ploop_get_cancel_handle(void);
void ploop_cancel_operation(struct ploop_cancel_handle *handle);

/* pcopy routines */
int send_process(const char *device, int ofd, const char *flush_cmd,
		int is_pipe);
int receive_process(const char *dst);

struct ploop_discard_stat {
	off_t data_size;
	off_t ploop_size;
	off_t image_size;
	off_t balloon_size;
};

int ploop_discard_get_stat(const char *device, const char *mount_point,
		struct ploop_discard_stat *pd_stat);

#ifdef __cplusplus
}
#endif


enum
{
	SYSEXIT_CREAT = 1,
	SYSEXIT_DEVICE,
	SYSEXIT_DEVIOC,
	SYSEXIT_OPEN,
	SYSEXIT_MALLOC,
	SYSEXIT_READ,
	SYSEXIT_WRITE,
	SYSEXIT_BLKDEV,
	SYSEXIT_SYSFS,
	SYSEXIT_FS,
	SYSEXIT_PLOOPFMT,
	SYSEXIT_SYS,
	SYSEXIT_PROTOCOL,
	SYSEXIT_LOOP,
	SYSEXIT_FSTAT,
	SYSEXIT_FSYNC,
	SYSEXIT_EBUSY,
	SYSEXIT_FLOCK,
	SYSEXIT_FTRUNCATE,
	SYSEXIT_FALLOCATE,
	SYSEXIT_MOUNT,
	SYSEXIT_UMOUNT,
	SYSEXIT_LOCK,
	SYSEXIT_MKFS,
	SYSEXIT_NOMEM,
	SYSEXIT_RESIZE_FS,
	SYSEXIT_MKDIR,
	SYSEXIT_RENAME,
	SYSEXIT_ABORT,
	SYSEXIT_RELOC,
	SYSEXIT_RESOLVE,
	SYSEXIT_CONNECT,
	SYSEXIT_CHANGE_GPT,
	SYSEXIT_CANCEL,
	SYSEXIT_UNLINK,
	SYSEXIT_MKNOD,
	SYSEXIT_PLOOPINUSE,
	SYSEXIT_PARAM,
	SYSEXIT_DISKDESCR,
};

#pragma GCC visibility pop
#endif
