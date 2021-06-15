//////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////

#include "../inc/disk.h"

disk_t disk;

/*
 * name: Alex Lux
 * lab: Project 1 Step 3
 * date: 4/21/21
 */

/***
 *
 * Verifies correctness of the logical block address, and then translates it to a cylinder-head-sector equivalent.
 *
 */
CIDEV_RET_CODE lba2chs(lba_t lba, chs_t *chs)
{
    // todo: done

    if (lba >= MAX_LOGICAL_BLOCK)
        return CIDEV_ADDRESS_ERROR;

    if(chs == NULL)
        return CIDEV_ANOTHER_ERROR;

    chs->sect = lba % NUM_OF_SECTS;
    chs->cyl = (lba / NUM_OF_SECTS) % NUM_OF_CYLS;
    chs->head = lba / (NUM_OF_SECTS * NUM_OF_CYLS);

    return CIDEV_SUCCESS;
}

/***
 *
 * Verifies correctness of the cylinder-head-sector address, and then translates it to a logical block address.
 *
 */
CIDEV_RET_CODE chs2lba(chs_t *chs, lba_t *lba)
{
    // todo: done

    if((chs->head > NUM_OF_HEADS) || (chs->cyl > NUM_OF_CYLS) || chs->sect > NUM_OF_SECTS)
        return CIDEV_ADDRESS_ERROR;

    if(chs == NULL || lba == NULL)
        return CIDEV_ANOTHER_ERROR;

    *lba = (NUM_OF_CYLS * NUM_OF_SECTS * chs->head) + (NUM_OF_SECTS * chs->cyl) + chs->sect;

    return CIDEV_SUCCESS;
}
/***
 *
 * Verifies the parameters, then allocates space for the requested data (using
 * the caller-provided pointer buffer), copies the data from the disk to the buffer,
 * and appends '\0' to terminate the string.
 *
 */


CIDEV_RET_CODE readDisk(lba_t lba, unsigned int size, char __user *buffer)
{
    // todo: done

    chs_t chs;

    int i;

    CIDEV_RET_CODE errCode = CIDEV_SUCCESS;

    unsigned int sectorTotal = ( size / SECT_SIZE) ;

    unsigned int remainder;

    unsigned int newSize = size;

    if(size > MAX_LOGICAL_BLOCK * SECT_SIZE)
        return CIDEV_ADDRESS_ERROR;

    if(newSize % SECT_SIZE != 0)
    {
        sectorTotal++;
    }

    lba_t currentLba;

    for(i = 0; i < sectorTotal; i++)
    {
      currentLba = (lba+i) % MAX_LOGICAL_BLOCK;
      errCode = lba2chs(currentLba, &chs);
        if(errCode != CIDEV_SUCCESS)
            break;

        if(newSize >= SECT_SIZE)
        {
            remainder = SECT_SIZE;
        }
        else
        {
            remainder = newSize;
        }
        // a special copy function that allows to copy from kernel space to user space
        copy_to_user(buffer + (i * SECT_SIZE), disk[chs.cyl][chs.head][chs.sect], remainder);

        newSize -= remainder;
    }
    return errCode;
}

/***
 *
 * An auxiliary function to fill a single disk block with '.'
 *
*/
CIDEV_RET_CODE clearBlock(lba_t lba)
{
    char *writeBuffer;
    int i;
#ifdef __DEBUG_DISK
    writeBuffer = malloc(SECT_SIZE);
#else
    writeBuffer = kmalloc(SECT_SIZE, GFP_USER);
#endif
    if (writeBuffer == NULL)
        return CIDEV_SPACE_ERROR;

    for (i = 0; i < SECT_SIZE; i++)
        writeBuffer[i] = '.';

    CIDEV_RET_CODE errCode = writeDisk(lba, SECT_SIZE, writeBuffer);

#ifdef __DEBUG_DISK
    free(writeBuffer);
#else
    kfree(writeBuffer);
#endif

    return errCode;
}

/***
 *
 * Validates the parameters, and then writes the caller-provided data to the disk starting at the block pointed
 * to by the logical block address.
 *
 */
CIDEV_RET_CODE writeDisk(lba_t lba, unsigned int size, char __user *buffer)
{
    // todo:

    CIDEV_RET_CODE errCode = CIDEV_SUCCESS;
    chs_t chs;

    unsigned int sectorTotal = (size / SECT_SIZE);

    unsigned int remainingBits = size;

    unsigned int remainder;

    int i;

    if (lba >= MAX_LOGICAL_BLOCK || buffer == NULL)
        return CIDEV_ADDRESS_ERROR;

    if(remainingBits % SECT_SIZE != 0)
    {
        sectorTotal++;
    }

    for(i = 0; i < sectorTotal; i++)
    {
        if(lba2chs((lba+i) % MAX_LOGICAL_BLOCK, &chs) != CIDEV_SUCCESS)
            return CIDEV_ANOTHER_ERROR;

        if(remainingBits >= SECT_SIZE)
        {
            remainder = SECT_SIZE;
        }
        else
        {
            remainder = remainingBits;
        }

        if (copy_from_user(disk[chs.cyl][chs.head][chs.sect], buffer + (i * SECT_SIZE), remainder) != 0)
          return -EFAULT;

        remainingBits -= remainder;
    }

    return errCode;
}
