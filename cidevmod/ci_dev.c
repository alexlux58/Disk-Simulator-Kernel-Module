//////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////

#include "../inc/ci_dev.h"
#include "../inc/disk.h"

// arbitrary number
#define CIDEV_MAJOR 444
#define CIDEV_NAME "cidev"


// static unsigned long ioctl_control_data;
// static unsigned long ioctl_status_data = 0xAABBCCDD;
DISK_REGISTER cidev_register;

// open function - called when the "file" /dev/cidev is opened in userspace
static int cidev_open (struct inode *inode, struct file *file)
{
   // this is a special print functions that allows a user to print from the kernel
	printk("cidev_open\n");
	return 0;
}

// close function - called when the "file" /dev/cidev is closed in userspace
static int cidev_release (struct inode *inode, struct file *file)
{
	printk("cidev_release\n");
	return 0;
}

// read function called when  /dev/cidev is read
static ssize_t cidev_read( struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

	 printk("CIDEV: READING %ld BYTES AT LBA %d\n", count, cidev_register.lba);

   cidev_register.ready = 0;
	 
	 cidev_register.error_code = readDisk(cidev_register.lba, count, buf);

	 if(cidev_register.error_code != CIDEV_SUCCESS)
	 {
		 cidev_register.error_occured = 1;
	 }

	 cidev_register.ready = 1;

   return count;
}

// write function called when /dev/cidev is written to
static ssize_t cidev_write( struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("CIDEV: WRITING %ld BYTES AT LBA %d\n", count, cidev_register.lba);
		cidev_register.ready = 0;

		cidev_register.error_code = writeDisk(cidev_register.lba, count, buf);

		if(cidev_register.error_code != CIDEV_SUCCESS)
 	 {
 		 cidev_register.error_occured = 1;
 	 }

 	 cidev_register.ready = 1;

	return count;
}

// ioctl function called when /dev/cidev receives an ioctl command
// Ubuntu 10.10: static int cidev_ioctl(struct inode *inode, struct file *file, unsigned int command, unsigned long arg)
// Ubuntu 11.04:
static long cidev_unlocked_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	switch ( command )
	{
		case IOCTL_CIDEV_WRITE:/* for writing data to arg */
			if (copy_from_user(&cidev_register, (int *)arg, sizeof(int)))
			   return -EFAULT;
			break;

		case IOCTL_CIDEV_READ:/* for reading data from arg */
			if (copy_to_user((int *)arg, &cidev_register, sizeof(int)))
			   return -EFAULT;
			break;

		default:
			return -EINVAL;
	}
	return -EINVAL;
}

//
// mapping of file operations to the driver functions
//

/* NEW VERSION -- OLD VERSION LOWER

struct file_operations {
        struct module *owner;
        loff_t (*llseek) (struct file *, loff_t, int);
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
        ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
        ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
        ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
        ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
        int (*iterate) (struct file *, struct dir_context *);
        unsigned int (*poll) (struct file *, struct poll_table_struct *);
        long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
        int (*mmap) (struct file *, struct vm_area_struct *);
        void (*mremap)(struct file *, struct vm_area_struct *);
        int (*open) (struct inode *, struct file *);
        int (*flush) (struct file *, fl_owner_t id);
        int (*release) (struct inode *, struct file *);
        int (*fsync) (struct file *, loff_t, loff_t, int datasync);
        int (*aio_fsync) (struct kiocb *, int datasync);
        int (*fasync) (int, struct file *, int);
        int (*lock) (struct file *, int, struct file_lock *);
        ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
        unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
        int (*check_flags)(int);
        int (*flock) (struct file *, int, struct file_lock *);
        ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
        ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
        int (*setlease)(struct file *, long, struct file_lock **, void **);
        long (*fallocate)(struct file *file, int mode, loff_t offset,
                          loff_t len);
        void (*show_fdinfo)(struct seq_file *m, struct file *f);
};

*/
struct file_operations cidev_file_operations = {
	.owner	=	THIS_MODULE,
	.llseek	=	NULL,
	.read	=	cidev_read,
	.write	=	cidev_write,
//	.readdir	=	NULL, // Ubuntu 14.04
	.poll	=	NULL,
//	.ioctl	=	cidev_ioctl, // Ubuntu 10.10
	.unlocked_ioctl	=	cidev_unlocked_ioctl, // Ubuntu 11.04
	.mmap	=	NULL,
	.open	=	cidev_open,
	.flush	=	NULL,
	.release	=	cidev_release,
	.fsync	=	NULL,
	.fasync	=	NULL,
	.lock	=	NULL,
};

// Loads a module in the kernel
static int cidev_init_module (void)
{
   // here we register cidev as a character device
	if (register_chrdev(CIDEV_MAJOR, CIDEV_NAME, &cidev_file_operations) != 0)
	   return -EIO;

	// reserve memory with kmalloc - Allocating Memory in the Kernel
	// GFP_KERNEL --> this does not have to be atomic, so kernel can sleep
   printk("Simulated Driver Module Installed\n");
   return 0;
}

//  Removes module from kernel
static void cidev_cleanup_module(void)
{
   // specialized function to free memeory in kernel
	unregister_chrdev (CIDEV_MAJOR, CIDEV_NAME);
   printk("Simulated Driver Module Uninstalled\n");
}

// map the module initialization and cleanup functins
module_init(cidev_init_module);
module_exit(cidev_cleanup_module);

MODULE_AUTHOR("http://www.cs.csuci.edu/~ajbieszczad");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CI Device Linux Device Driver");
