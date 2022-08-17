struct tar_header
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
};


unsigned int getsize(const char *in);
unsigned int initrd_get_number_files(unsigned int address);
void initrd_list_filenames(unsigned int address);