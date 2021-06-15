//////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////


#include "../inc/ci_dev.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define oops(msg, errnum) { perror(msg); exit(errnum); }

#define BUFSIZE 4096

int numberOfTrials; // global to allow passing it to the timeout handler


/***
*
* Generates random readable/printable content for testing
*
*/
char *generatePrintableContent(int size)
{
   char *content = malloc(size + 1);
	 int i;

   int firstPrintable = ' ';
   int lastPrintable = '~';
   int len = lastPrintable - firstPrintable;

   for (i = 0; i < size; i++)
      *(content + i) = firstPrintable + rand() % len;

   content[size] = '\0';
   return content;
}

/***
 *
 * This function verifies matches in writing and reading same part of disks.
 *
 * The locations and the chunk size are generated randomly.
 *
 */
// void testReadWrite()
// {
//    lba_t lba;
// 	 lba_t blockNum;
//
//    CIDEV_RET_CODE errorCode;
//
//    char *readBuffer = NULL;
//    char *writeBuffer = NULL;
//
//    for (blockNum = 0; blockNum < MAX_LOGICAL_BLOCK; blockNum++)
//       clearBlock(blockNum);
//
//    int size; // of the read or written chunk
//
//    int i;
//
//    for (i = 0; i < numberOfTrials; i++)
//    {
//       size = (((float) rand()) / RAND_MAX) * (MAX_LOGICAL_BLOCK * SECT_SIZE) + 1;
//       lba = (((float) rand()) / RAND_MAX) * MAX_LOGICAL_BLOCK;
//
//       writeBuffer = generatePrintableContent(size);
//       if ((errorCode = writeDisk(lba, writeBuffer)) != CIDEV_SUCCESS)
//       {
//          printf("\n*** ERROR WRITING %d BYTES: %d\n\n", size, errorCode);
//          free(writeBuffer);
//          continue;
//       }
//       else
//          printf("WROTE %d bytes:\n%s\n", size, writeBuffer);
//
//
//       if ((errorCode = readDisk(lba, size, &readBuffer)) != CIDEV_SUCCESS)
//       {
//          printf("\n*** ERROR READING %d BYTES: %d\n\n", size, errorCode);
//          continue;
//       }
//       else
//          printf("READ %ld bytes:\n%s\n", strlen(readBuffer), readBuffer);
//
//       if (strcmp(writeBuffer, readBuffer) == 0)
//          printf("\nSUCCESS: READ AND WRITE MATCH.\n\n");
//       else
//          printf("\n*** FAILURE IN TEST 1: READ AND WRITE DO NOT MATCH.\n\n");
//
//       free(writeBuffer);
//       free(readBuffer);
//    }
// }

int randomIntExclusive(int min, int max) {
  return (rand() % (max - min)) + min;
}

int main(int argc, char *argv[])
{
	int fd, len, i, size;
	unsigned int randomLBA;

	srand((unsigned int) time(NULL));

	if(argc < 2)
	{
		printf("USAGE: <num of trials>\n");
		exit(EXIT_FAILURE);
	}

	numberOfTrials = strtol(argv[1], NULL, 10);

	// test message to send to the device
	char *msg = NULL;



	// variables for holding device control data
	// int ioctl_control_data, ioctl_status_data;

	// open the I/O channel to the device
	fd = open("/dev/cidev", O_RDWR | O_SYNC);
	if ( fd == -1)
	{
		oops("Unable to open device...", 1);
	}
	else
	{
		printf("Opened I/O channel to the device\n");
	}

	DISK_REGISTER cidev_register;

  for(i = 0; i < numberOfTrials; i++)
  {
    size = randomIntExclusive(1, 1280 * 16);
    randomLBA = randomIntExclusive(0, 1280);

    msg = generatePrintableContent(size);

    ioctl(fd, IOCTL_CIDEV_READ, &cidev_register); // read current register

  	cidev_register.lba = randomLBA ; // update with desired parameters

  	ioctl(fd, IOCTL_CIDEV_WRITE, &cidev_register); // write updated register

  	write(fd, msg, size); // do operation

  	ioctl(fd, IOCTL_CIDEV_READ, &cidev_register); // read result register

  	if (cidev_register.error_occured == 1) {
  		printf("ERROR WRITING TO DISK: %d\n", cidev_register.error_code);
  	} else {
      printf("\n----------------- RUN %d -----------------\n", i + 1);
      printf("WROTE %d bytes to LBA %d\n", size, randomLBA);

  		ioctl(fd, IOCTL_CIDEV_READ, &cidev_register); // read current register

  		cidev_register.lba = randomLBA; // update with desired parameters

  		ioctl(fd, IOCTL_CIDEV_WRITE, &cidev_register); // write updated register

      char *receive_buffer = malloc(size + 1);

  		read(fd, receive_buffer, size); // do operations
  		receive_buffer[size] = '\0';

  		ioctl(fd, IOCTL_CIDEV_READ, &cidev_register); // read result register

  		if (cidev_register.error_occured == 1) {
  			printf("ERROR READING FROM DISK: %d\n", cidev_register.error_code);
  		} else {
        printf("READ %d bytes to LBA %d: %s\n", size, randomLBA, receive_buffer);
        printf("\n-----------------------------------------\n");
  			if (strcmp(msg, receive_buffer) == 0) {
  				printf("\nSUCCESS READ AND WRITE MATCH\n");
  			} else {
  				printf("FAILURE READ AND WRITE DO NOT MATCH\n");
  			}
  		}
      free(receive_buffer);

  	}
  }


	close(fd);
}
