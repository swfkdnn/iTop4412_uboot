#include <common.h>
#include <command.h>
#include <mmc.h>
#include <movi.h> //mj

#define		BLOCK_SIZE			512
#define		BLOCK_END			0xFFFFFFFF
#define		_10MB				(10*1024*1024)
#define		_100MB				(100*1024*1024)
#define		_300MB				(300*1024*1024)
#define		_8_4GB				(1023*254*63)
#define		_1GB				(1024*1024*1024)
#define		DISK_START			RAW_AREA_SIZE//mj (16*1024*1024) //same as raw area size

#define		SYSTEM_PART_SIZE		_1GB //_300MB
#define		USER_DATA_PART_SIZE		_1GB //_300MB //_1GB
#define		CACHE_PART_SIZE			_300MB

#define		CHS_MODE			0
#define		LBA_MODE			!(CHS_MODE)

typedef struct
{
	int		C_start;
	int		H_start;
	int		S_start;

	int		C_end;
	int		H_end;
	int		S_end;

	int		available_block;
	int		unit;
	int		total_block_count;
	int		addr_mode;	// LBA_MODE or CHS_MODE
} SDInfo;

typedef struct
{
	unsigned char bootable;
	unsigned char partitionId;

	int		C_start;
	int		H_start;
	int		S_start;

	int		C_end;
	int		H_end;
	int		S_end;

	int		block_start;
	int		block_count;
	int		block_end;
} PartitionInfo;

/////////////////////////////////////////////////////////////////
unsigned int calc_unit(unsigned int length, SDInfo sdInfo)
{
	if (sdInfo.addr_mode == CHS_MODE)
		return ( (length / BLOCK_SIZE / sdInfo.unit + 1 ) * sdInfo.unit);
	else
		return ( (length / BLOCK_SIZE) );
}

/////////////////////////////////////////////////////////////////
void encode_chs(int C, int H, int S, unsigned char *result)
{
	*result++ = (unsigned char) H;
	*result++ = (unsigned char) ( S + ((C & 0x00000300) >> 2) );
	*result   = (unsigned char) (C & 0x000000FF); 
}

/////////////////////////////////////////////////////////////////
void encode_partitionInfo(PartitionInfo partInfo, unsigned char *result)
{
	*result++ = partInfo.bootable;

	encode_chs(partInfo.C_start, partInfo.H_start, partInfo.S_start, result);
	result +=3;
	*result++ = partInfo.partitionId;

	encode_chs(partInfo.C_end, partInfo.H_end, partInfo.S_end, result);
	result += 3;

	memcpy(result, (unsigned char *)&(partInfo.block_start), 4);
	result += 4;	
	
	memcpy(result, (unsigned char *)&(partInfo.block_count), 4);
}

/////////////////////////////////////////////////////////////////
void decode_partitionInfo(unsigned char *in, PartitionInfo *partInfo)
{
	partInfo->bootable	= *in;
	partInfo->partitionId	= *(in + 4); 

	memcpy((unsigned char *)&(partInfo->block_start), (in + 8), 4);
	memcpy((unsigned char *)&(partInfo->block_count), (in +12), 4);
}

/////////////////////////////////////////////////////////////////
void get_SDInfo(int block_count, SDInfo *sdInfo)
{
       int C, H, S;

        int C_max = 1023, H_max = 255, S_max = 63;
        int H_start = 1, S_start = 1;
        int diff_min = 0, diff = 0;

        if(block_count >= _8_4GB)
                sdInfo->addr_mode = LBA_MODE;
        else
                sdInfo->addr_mode = CHS_MODE;

//-----------------------------------------------------
        if (sdInfo->addr_mode == CHS_MODE)
        {
                diff_min = C_max;

                for (H = H_start; H <= H_max; H++)
                        for (S  = S_start; S <= S_max; S++)
                        {
                                C = block_count / (H * S);

                                if ( (C <= C_max) )
                                {
                                        diff = C_max - C;
                                        if (diff <= diff_min)
                                        {
                                                diff_min = diff;
                                                sdInfo->C_end = C;
                                                sdInfo->H_end = H;
                                                sdInfo->S_end = S;
                                        }
                                }
                        }
        }
//-----------------------------------------------------
        else
        {
                sdInfo->C_end = 1023;
                sdInfo->H_end = 254;
                sdInfo->S_end = 63;
        }

//-----------------------------------------------------
        sdInfo->C_start                 = 0;
        sdInfo->H_start                 = 1;
        sdInfo->S_start                 = 1;

        sdInfo->total_block_count       = block_count;
        sdInfo->available_block         = sdInfo->C_end * sdInfo->H_end * sdInfo->S_end;
        sdInfo->unit                    = sdInfo->H_end * sdInfo->S_end;
}

/////////////////////////////////////////////////////////////////
void make_partitionInfo(int LBA_start, int count, SDInfo sdInfo, PartitionInfo *partInfo)
{
        unsigned int             temp = 0;
        unsigned int             _10MB_unit;

        partInfo->block_start   = LBA_start;

//-----------------------------------------------------
        if (sdInfo.addr_mode == CHS_MODE)
        {
                printf("sdInfo.addr_mode = CHS_MODE  \n\r");
                partInfo->C_start       = partInfo->block_start / (sdInfo.H_end * sdInfo.S_end);
                temp                    = partInfo->block_start % (sdInfo.H_end * sdInfo.S_end);
                partInfo->H_start       = temp / sdInfo.S_end;
                partInfo->S_start       = temp % sdInfo.S_end + 1;

                if (count == BLOCK_END)
                {
                        _10MB_unit = calc_unit(_10MB, sdInfo);
                        partInfo->block_end     = sdInfo.C_end * sdInfo.H_end * sdInfo.S_end - _10MB_unit - 1;
                        partInfo->block_count   = partInfo->block_end - partInfo->block_start + 1;

                        partInfo->C_end = partInfo->block_end / sdInfo.unit;
                        partInfo->H_end = sdInfo.H_end - 1;
                        partInfo->S_end = sdInfo.S_end;
                }
                else
                {
                        partInfo->block_count   = count;

                        partInfo->block_end     = partInfo->block_start + count - 1;
                        partInfo->C_end         = partInfo->block_end / sdInfo.unit;

                        temp                    = partInfo->block_end % sdInfo.unit;
                        partInfo->H_end         = temp / sdInfo.S_end;
                        partInfo->S_end         = temp % sdInfo.S_end + 1;
                }
        }
//-----------------------------------------------------
        else
        {
                partInfo->C_start       = 0;
                partInfo->H_start       = 1;
                partInfo->S_start       = 1;

                partInfo->C_end         = 1023;
                partInfo->H_end         = 254;
                partInfo->S_end         = 63;

                if (count == BLOCK_END)
                {
                        _10MB_unit = calc_unit(_10MB, sdInfo);
                        partInfo->block_end     = sdInfo.total_block_count - _10MB_unit - 1;
                        partInfo->block_count   = partInfo->block_end - partInfo->block_start + 1;

                }
                else
                {
                        partInfo->block_count   = count;
                        partInfo->block_end     = partInfo->block_start + count - 1;
                }
        }
}

int make_mmc_partition(int total_block_count, unsigned char *mbr, int flag, char *argv[])
{
  printf("[%s %s] %s: %s: %d\n", \
      __DATE__, __TIME__, __FILE__, __func__, __LINE__);
  printf("in make_mmc_partition \n\r");
  printf("argv[0] = %s .. argv[1] = %s .. argv[2] = %s ..argv[3] = %s.. argv[4] = %s.. argv[5] = %s \n\r" \
      ,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
  unsigned int  block_start = 0, block_offset;
  SDInfo sdInfo;
  PartitionInfo partInfo[4];
	memset((unsigned char *)&sdInfo, 0x00, sizeof(SDInfo));
	get_SDInfo(total_block_count, &sdInfo);
	block_start = calc_unit(DISK_START, sdInfo);
/* modify by cym 20131206 */
#if 0
	block_offset	= calc_unit(SYSTEM_PART_SIZE, sdInfo);
#else
	if (flag)
    {
      block_offset = calc_unit((unsigned long long)simple_strtoul(argv[3], NULL, 0)*1024*1024, sdInfo);
      printf("calc_unit length = %ld  \n\r  ",(unsigned long long)simple_strtoul(argv[3], NULL, 0)*1024*1024);
    }
	else
		block_offset = calc_unit(SYSTEM_PART_SIZE, sdInfo);
#endif
/* end modify */

  printf("sdInfo.C_start           = %x \n\r",sdInfo.C_start            );
  printf("sdInfo.H_start           = %d \n\r",sdInfo.H_start            );
  printf("sdInfo.S_start           = %d \n\r",sdInfo.S_start            );
  printf("sdInfo.C_end             = %d \n\r",sdInfo.C_end              );
  printf("sdInfo.H_end             = %d \n\r",sdInfo.H_end              );
  printf("sdInfo.S_end             = %d \n\r",sdInfo.S_end              );
  printf("sdInfo.available_block   = %d \n\r",sdInfo.available_block    );
  printf("sdInfo.unit              = %d \n\r",sdInfo.unit               );
  printf("sdInfo.total_block_count = %d \n\r",sdInfo.total_block_count  );
  printf("sdInfo.addr_mode         = %d \n\r",sdInfo.addr_mode          ); // LBA_MODE or CHS_MODE
  printf("total_block_count = %d \n\r",total_block_count);
  printf("DISK_START = %d \n\r ",DISK_START);
  printf("flag = %d \n\r ",flag);
  printf("block_start = %d \n\r "   ,block_start);
  printf("block_offset = %d \n\r " ,block_offset);
	partInfo[0].bootable	= 0x00;
	partInfo[0].partitionId	= 0x83;
	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[0]);
  printf("partInfo[0].bootable      = %x \n\r", partInfo[0].bootable      );
  printf("partInfo[0].partitionId   = %x \n\r", partInfo[0].partitionId   );
  printf("partInfo[0].C_start       = %d \n\r", partInfo[0].C_start       );
  printf("partInfo[0].H_start       = %d \n\r", partInfo[0].H_start       );
  printf("partInfo[0].S_start       = %d \n\r", partInfo[0].S_start       );
  printf("partInfo[0].C_end         = %d \n\r", partInfo[0].C_end         );
  printf("partInfo[0].H_end         = %d \n\r", partInfo[0].H_end         );
  printf("partInfo[0].S_end         = %d \n\r", partInfo[0].S_end         );
  printf("partInfo[0].block_start   = %d \n\r", partInfo[0].block_start   );
  printf("partInfo[0].block_count   = %d \n\r", partInfo[0].block_count   );
  printf("partInfo[0].block_end     = %d \n\r", partInfo[0].block_end     );
	block_start += block_offset;
/* modify by cym 20131206 */
#if 0
	if (strcmp(argv[2], "1") == 0)// TF card
		block_offset = calc_unit(_300MB, sdInfo);
	else
		block_offset = calc_unit(USER_DATA_PART_SIZE, sdInfo);
#else
	if (flag)
		block_offset = calc_unit((unsigned long long)simple_strtoul(argv[4], NULL, 0)*1024*1024, sdInfo);
	else
	{
		if (strcmp(argv[2], "1") == 0)// TF card
			block_offset = calc_unit(_300MB, sdInfo);
		else
			block_offset = calc_unit(USER_DATA_PART_SIZE, sdInfo);
	}
#endif
/* end modify */
	
	partInfo[1].bootable	= 0x00;
	partInfo[1].partitionId	= 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[1]);


  printf("partInfo[1].bootable      = %x \n\r", partInfo[1].bootable      );
  printf("partInfo[1].partitionId   = %x \n\r", partInfo[1].partitionId   );
  printf("partInfo[1].C_start       = %d \n\r", partInfo[1].C_start       );
  printf("partInfo[1].H_start       = %d \n\r", partInfo[1].H_start       );
  printf("partInfo[1].S_start       = %d \n\r", partInfo[1].S_start       );
  printf("partInfo[1].C_end         = %d \n\r", partInfo[1].C_end         );
  printf("partInfo[1].H_end         = %d \n\r", partInfo[1].H_end         );
  printf("partInfo[1].S_end         = %d \n\r", partInfo[1].S_end         );
  printf("partInfo[1].block_start   = %d \n\r", partInfo[1].block_start   );
  printf("partInfo[1].block_count   = %d \n\r", partInfo[1].block_count   );
  printf("partInfo[1].block_end     = %d \n\r", partInfo[1].block_end     );


	block_start += block_offset;
/* modify by cym 20131206 */
#if 0
	block_offset = calc_unit(CACHE_PART_SIZE, sdInfo);
#else
	if(flag)
		block_offset = calc_unit((unsigned long long)simple_strtoul(argv[5], NULL, 0)*1024*1024, sdInfo);
	else
		block_offset = calc_unit(CACHE_PART_SIZE, sdInfo);
#endif
/* end modify */
	
	partInfo[2].bootable	= 0x00;
	partInfo[2].partitionId	= 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[2]);


  printf("partInfo[2].bootable      = %x \n\r", partInfo[2].bootable      );
  printf("partInfo[2].partitionId   = %x \n\r", partInfo[2].partitionId   );
  printf("partInfo[2].C_start       = %d \n\r", partInfo[2].C_start       );
  printf("partInfo[2].H_start       = %d \n\r", partInfo[2].H_start       );
  printf("partInfo[2].S_start       = %d \n\r", partInfo[2].S_start       );
  printf("partInfo[2].C_end         = %d \n\r", partInfo[2].C_end         );
  printf("partInfo[2].H_end         = %d \n\r", partInfo[2].H_end         );
  printf("partInfo[2].S_end         = %d \n\r", partInfo[2].S_end         );
  printf("partInfo[2].block_start   = %d \n\r", partInfo[2].block_start   );
  printf("partInfo[2].block_count   = %d \n\r", partInfo[2].block_count   );
  printf("partInfo[2].block_end     = %d \n\r", partInfo[2].block_end     );




	block_start += block_offset;
	block_offset = BLOCK_END;

	partInfo[3].bootable	= 0x00;
	partInfo[3].partitionId	= 0x0C;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[3]);

  printf("partInfo[3].bootable      = %x \n\r", partInfo[3].bootable      );
  printf("partInfo[3].partitionId   = %x \n\r", partInfo[3].partitionId   );
  printf("partInfo[3].C_start       = %d \n\r", partInfo[3].C_start       );
  printf("partInfo[3].H_start       = %d \n\r", partInfo[3].H_start       );
  printf("partInfo[3].S_start       = %d \n\r", partInfo[3].S_start       );
  printf("partInfo[3].C_end         = %d \n\r", partInfo[3].C_end         );
  printf("partInfo[3].H_end         = %d \n\r", partInfo[3].H_end         );
  printf("partInfo[3].S_end         = %d \n\r", partInfo[3].S_end         );
  printf("partInfo[3].block_start   = %d \n\r", partInfo[3].block_start   );
  printf("partInfo[3].block_count   = %d \n\r", partInfo[3].block_count   );
  printf("partInfo[3].block_end     = %d \n\r", partInfo[3].block_end     );



	memset(mbr, 0x00, sizeof(*mbr)*512);// liang, clean the mem again
	mbr[510] = 0x55; mbr[511] = 0xAA;




	encode_partitionInfo(partInfo[0], &mbr[0x1CE]);
	encode_partitionInfo(partInfo[1], &mbr[0x1DE]);
	encode_partitionInfo(partInfo[2], &mbr[0x1EE]);
	encode_partitionInfo(partInfo[3], &mbr[0x1BE]);
	
	return 0;
}

int get_mmc_block_count(char *device_name)
{
  printf("[%s %s] %s: %s: %d\n", \
      __DATE__, __TIME__, __FILE__, __func__, __LINE__);
	int rv;
	struct mmc *mmc;
	int block_count = 0;
	int dev_num;
	dev_num = simple_strtoul(device_name, NULL, 0);
	printf("dev_num = %d",dev_num);
	mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}	
	
	block_count = mmc->capacity * (mmc->read_bl_len / BLOCK_SIZE);
		
//	printf("block_count = %d\n", block_count);
	return block_count;
}

/////////////////////////////////////////////////////////////////
int get_mmc_mbr(char *device_name, unsigned char *mbr)
{
	int rv;
	struct mmc *mmc;
	int dev_num;

	dev_num = simple_strtoul(device_name, NULL, 0);
	
	mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}	
	
	rv = mmc->block_dev.block_read(dev_num, 0, 1, mbr);

	if(rv == 1)
		return 0;
	else
		return -1; 
}

/////////////////////////////////////////////////////////////////
int put_mmc_mbr(unsigned char *mbr, char *device_name)
{
	int rv;
	struct mmc *mmc;
	int dev_num;

	dev_num = simple_strtoul(device_name, NULL, 0);
	mmc = find_mmc_device(dev_num);
	sddbg("find the dev-num %d,%s",dev_num,mmc->name);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}	

	rv = mmc->block_dev.block_write(dev_num, 0, 1, mbr);

	if(rv == 1)
		return 0;
	else
		return -1; 
}

/////////////////////////////////////////////////////////////////
int get_mmc_part_info(char *device_name, int part_num, int *block_start, int *block_count, unsigned char *part_Id)
{
	int		rv;
	PartitionInfo	partInfo;
	unsigned char	mbr[512];
	
	rv = get_mmc_mbr(device_name, mbr);
	if(rv !=0)
		return -1;
				
	switch(part_num)
	{
		case 1:
			decode_partitionInfo(&mbr[0x1BE], &partInfo);
			*block_start	= partInfo.block_start;	
			*block_count	= partInfo.block_count;	
			*part_Id 	= partInfo.partitionId;	
			break;
		case 2:
			decode_partitionInfo(&mbr[0x1CE], &partInfo);
			*block_start	= partInfo.block_start;	
			*block_count	= partInfo.block_count;	
			*part_Id 	= partInfo.partitionId;	
			break;
		
		case 3:
			decode_partitionInfo(&mbr[0x1DE], &partInfo);
			*block_start	= partInfo.block_start;	
			*block_count	= partInfo.block_count;	
			*part_Id 	= partInfo.partitionId;	
			break;
		case 4:
			decode_partitionInfo(&mbr[0x1EE], &partInfo);
			*block_start	= partInfo.block_start;	
			*block_count	= partInfo.block_count;	
			*part_Id 	= partInfo.partitionId;	
			break;
		default:
			return -1;
	}	

	return 0;
}

/////////////////////////////////////////////////////////////////
int print_mmc_part_info(int argc, char *argv[])
{
	int		rv;

	PartitionInfo	partInfo[4];
	
	rv = get_mmc_part_info(argv[2], 1, &(partInfo[0].block_start), &(partInfo[0].block_count),
			&(partInfo[0].partitionId) );
	
	rv = get_mmc_part_info(argv[2], 2, &(partInfo[1].block_start), &(partInfo[1].block_count),
			&(partInfo[1].partitionId) );

	rv = get_mmc_part_info(argv[2], 3, &(partInfo[2].block_start), &(partInfo[2].block_count),
			&(partInfo[2].partitionId) );

	rv = get_mmc_part_info(argv[2], 4, &(partInfo[3].block_start), &(partInfo[3].block_count),
			&(partInfo[3].partitionId) );

	printf("\n");	
	printf("partion #    size(MB)     block start #    block count    partition_Id \n");

	if ( (partInfo[0].block_start !=0) && (partInfo[0].block_count != 0) ) 
		printf("   1        %6d         %8d        %8d          0x%.2X \n",
			(partInfo[0].block_count / 2048), partInfo[0].block_start,
			partInfo[0].block_count, partInfo[0].partitionId);
	
	if ( (partInfo[1].block_start !=0) && (partInfo[1].block_count != 0) ) 
		printf("   2        %6d         %8d        %8d          0x%.2X \n",
			(partInfo[1].block_count / 2048), partInfo[1].block_start,
			partInfo[1].block_count, partInfo[1].partitionId);
	
	if ( (partInfo[2].block_start !=0) && (partInfo[2].block_count != 0) ) 
		printf("   3        %6d         %8d        %8d          0x%.2X \n",
			(partInfo[2].block_count / 2048), partInfo[2].block_start,
			partInfo[2].block_count, partInfo[2].partitionId);

	if ( (partInfo[3].block_start !=0) && (partInfo[3].block_count != 0) ) 
		printf("   4        %6d         %8d        %8d          0x%.2X \n",
			(partInfo[3].block_count / 2048), partInfo[3].block_start,
			partInfo[3].block_count, partInfo[3].partitionId);

	return 1;
}

/////////////////////////////////////////////////////////////////
int create_mmc_fdisk(int argc, char *argv[])
{
  printf("[%s %s] %s: %s: %d\n", \
      __DATE__, __TIME__, __FILE__, __func__, __LINE__);
	int		rv;
	int		total_block_count;
	unsigned char	mbr[512];
	memset(mbr, 0x00, 512);
	total_block_count = get_mmc_block_count(argv[2]);
  printf("total_block_count = %d",total_block_count);
	if (total_block_count < 0)
		return -1;
	make_mmc_partition(total_block_count, mbr, (argc==6?1:0), argv);
	rv = put_mmc_mbr(mbr, argv[2]);
	if (rv != 0)
		return -1;
	printf("fdisk is completed\n");
	argv[1][1] = 'p';
	print_mmc_part_info(argc, argv);
	return 0;
}

/////////////////////////////////////////////////////////////////
int do_fdisk(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if ( argc == 3 || argc ==6 )
	{
		if ( strcmp(argv[1], "-c") == 0 )
			return create_mmc_fdisk(argc, argv);
		else if ( strcmp(argv[1], "-p") == 0 )
			return print_mmc_part_info(argc, argv);
	}
	else
	{
		printf("Usage:\nfdisk <-p> <device_num>\n");
		printf("fdisk <-c> <device_num> [<sys. part size(MB)> <user part size> <cache part size>]\n");
	}
	return 0;
}

U_BOOT_CMD(
	fdisk, 6, 0, do_fdisk,
	"fdisk\t- fdisk for sd/mmc.\n",
	"-c <device_num> [<sys. part size(MB)> <user part size> <cache part size>]\t- create partition.\n"
	"fdisk -p <device_num>\t- print partition information\n"
);

