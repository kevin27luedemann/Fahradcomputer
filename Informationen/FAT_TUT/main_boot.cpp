//#include <iostream>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned int start_sector;
    unsigned int length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct {
    unsigned char jmp[3];
    char oem[8];
    unsigned short sector_size;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short; // if zero, later field is used
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int total_sectors_long;
    
    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    unsigned short boot_sector_signature;
} __attribute((packed)) Fat16BootSector;


int main(){
	FILE * in = fopen("test.img", "rb");
	unsigned int i;//, start_sector, length_sector;
	PartitionTable pt[4];
	Fat16BootSector bs;

	fseek(in, 0x1BE, SEEK_SET);	//goto partition Table
	fread(pt, sizeof(PartitionTable), 4, in); //alle 4 eintraege lesen

	for(i=0; i<4; i++){ //lese alle 4 eintraege
		//lese alles von Hand
/* 		printf("Partition eintrag %u: Erstes Byte %02X\n", i, fgetc(in));
		printf("\tPartition start in CHS: %02X:%02X:%02X\n", fgetc(in), fgetc(in), fgetc(in));
		printf("\tPartition Art %02X\n", fgetc(in));
		printf("\tPartition end in CHS: %02X:%02X:%02X\n", fgetc(in), fgetc(in), fgetc(in));
		fread(&start_sector, 4, 1, in);
		fread(&length_sector, 4, 1, in);
		printf("\trealtive LBA Adresse %08X, %u sectoren lang\n", start_sector, length_sector);*/
		//Alles mit strucktur eingelesen
		/*
		printf("Partition %u, Art %02X\n", i, pt[i].partition_type);
		printf("\t Start Sector %08X, %u Sector Laenge\n", pt[i].start_sector, pt[i].length_sectors);
		*/
		
		if(pt[i].partition_type == 4 || pt[i].partition_type == 6 || pt[i].partition_type == 16){
			printf("FAT16 Dateisystem gefunden in Partition %u\n", i);
			break;
		}
	}
	if(i == 4){
		printf("Kein FAT16 Dateisystem gefunden\n");
		return 0;
	}
	
	fseek(in, 512 * pt[i].start_sector, SEEK_SET);
	fread(&bs, sizeof(Fat16BootSector), 1, in);
	
	printf("  Jump code: %02X:%02X:%02X\n", bs.jmp[0], bs.jmp[1], bs.jmp[2]);
	printf("  OEM code: [%.8s]\n", bs.oem);
	printf("  sector_size: %u\n", bs.sector_size);
	printf("  sectors_per_cluster: %u\n", bs.sectors_per_cluster);
	printf("  reserved_sectors: %u\n", bs.reserved_sectors);
	printf("  number_of_fats: %u\n", bs.number_of_fats);
	printf("  root_dir_entries: %u\n", bs.root_dir_entries);
	printf("  total_sectors_short: %u\n", bs.total_sectors_short);
	printf("  media_descriptor: 0x%02X\n", bs.media_descriptor);
	printf("  fat_size_sectors: %u\n", bs.fat_size_sectors);
	printf("  sectors_per_track: %u\n", bs.sectors_per_track);
	printf("  number_of_heads: %u\n", bs.number_of_heads);
	printf("  hidden_sectors: %u\n", bs.hidden_sectors);
	printf("  total_sectors_long: %u\n", bs.total_sectors_long);
	printf("  drive_number: 0x%02X\n", bs.drive_number);
	printf("  current_head: 0x%02X\n", bs.current_head);
	printf("  boot_signature: 0x%02X\n", bs.boot_signature);
	printf("  volume_id: 0x%08X\n", bs.volume_id);
	printf("  Volume label: [%.11s]\n", bs.volume_label);
	printf("  Filesystem type: [%.8s]\n", bs.fs_type);
	printf("  Boot sector signature: 0x%04X\n", bs.boot_sector_signature);
	

	fclose(in);
	return 0;
}
