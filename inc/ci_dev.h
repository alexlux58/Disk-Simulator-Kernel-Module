//////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////

#ifndef __CIDEV_H_
#define __CIDEV_H_

#define IOCTL_CIDEV_WRITE _IOW(0, 1, int)
#define IOCTL_CIDEV_READ _IOR(0, 2, int)


typedef struct disk_register {
    unsigned ready: 1;
    unsigned error_occured: 1;
    unsigned error_code: 2;
    unsigned lba: 28;
} DISK_REGISTER;

#endif
