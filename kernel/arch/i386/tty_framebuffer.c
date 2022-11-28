#include <kernel/tty_framebuffer.h>
#include <stdio.h>
#include <types.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <kernel/fb.h>
#include <kernel/graphics.h>
#include <kernel/misc.h>
#include <kernel/vfs.h>
#include <kernel/initrd.h>
#include <kernel/speaker.h>
#include <kernel/config.h>

#if GUI_MODE
#else
#include <kernel/font.h>
#endif

extern char keyboard_us[];
extern void sys_sleep(int seconds);
typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t height;
} font_header_t;

static fb_t fb;
int row = 0;
int column = 0;
uint32_t color = 0xFFFFFF;
uint32_t old_color;

static char line_cli[300];
static char last_line_cli[300];


void terminal_framebuffer_initialize(){
    log(LOG_SERIAL, false, "Starting terminal framebuffer\n");
    fb = fb_get_info();
}

void terminal_framebuffer_putc(char c){
#if GUI_MODE
#else
    if (c == '\n'){
        row += 12;
        column = 0;
        return;
    }
    uint8_t* offset = font_psf + sizeof(font_header_t) + c*16;
    for (int i = 0; i < 16; i ++){
        for (int j = 0; j < 8; j++){
            if (offset[i] & (1 << j)){
                //draw_pixel(fb, x + 8 - j, y + i, col);
                draw_pixel(fb, column + 8 - j, row + i, color);
            }
        }
    }
    column += 8;
    if (column >= fb.width){
        column = 0;
        row  += 12;
        if (row == fb.height){
            row = 0;
        }
    }
#endif
}

void terminal_framebuffer_clear(){
    row = 0;
    column = 0;
    for (uint32_t i = 0; i < fb.width; i++){
        for (uint32_t j = 0; j < fb.height; j++){
            draw_pixel(fb, i, j, BLACK);
        }
    }
}

void terminal_framebuffer_keypress(uint8_t scan_code){
    char c = keyboard_us[scan_code];
    append(line_cli, c);
    //log(LOG_SERIAL, false, "char after scancode converting : %c\n", c);
    putchar(c);
}

void empty_line_cli_framebuffer(){
    memset(line_cli, 0, 300);
}

void launch_command_framebuffer(){
	char line_cli_copy[300];
	char* token;
	strcpy(line_cli_copy, line_cli);
	char* command = strtok(line_cli_copy, " ");
	log(LOG_SERIAL, false,"command: %s\n", command);
	char** argv = kmalloc(sizeof(char*) * 20);
	int i = 0;
	token = strtok(NULL, " ");
	log(LOG_SERIAL, false, "token : %s\n", token);
	argv[i] = token;
	log(LOG_SERIAL, false, "argv[%d] : %s\n", i, argv[i]);
	i++;
	while (token != NULL){
		token = strtok(NULL, " ");
		log(LOG_SERIAL, false, "token : %s\n", token);
		argv[i] = token;
		log(LOG_SERIAL, false, "argv[%d] : %s\n", i, argv[i]);
		i++;
	}
	int argc = i + 1;
    if (startswith("clear", line_cli)){
		terminal_framebuffer_clear();
	} else if (startswith("echo", line_cli)){
		char temp[95];
		int i2 = 5;
		for (int i = 0; i < strlen(line_cli); i++){
			temp[i] = line_cli[i2];
			i2++;
		}
		printf("\n%s", temp);
	} else if (startswith("ls", line_cli)){
		fs_node_t* root = get_initrd_root();
		struct dirent* dir = NULL;
		int i = 0;
		while ((dir = readdir_fs(fs_root, i))!=NULL){
        	printf("\nfilename [%i] : %s\n",i, dir->name);
        	i++;
        }
		/*uint32_t addr = get_initrd_address();
		initrd_list_filenames(addr);*/
	} else if (startswith("cat", line_cli)){
		char filename[20];
		int i2 = 4;
		for (int i = 0; i < strlen(line_cli); i++){
			filename[i] = line_cli[i2];
			i2++;
		}
		fs_node_t* root = get_initrd_root();
		struct dirent* dir = NULL;
		int i = 0;
		fs_node_t* file = vfs_open(filename, "r");
		if (file == NULL){
			printf("\nfile not found: %s\n", filename);
		} else {
		uint8_t buffer[file->length];
        read_fs(file, 0, file->length, buffer);
		printf("\n%s\n", buffer);
		}
		close_fs(file);
	} else if (startswith("help", line_cli)){
		printf("\nusage help : \n");
		printf("echo : print string\n");
		printf("ls : list directory and files\n");
		printf("clear : clear screen\n");
		printf("reboot : reboot the computer\n");
		printf("thirdtemple : ...\n");
		printf("help : print this help\n");
	} else if (startswith("arch", line_cli)){
		printf("\ni386");
	} else if (startswith("gui", line_cli)){
		
	} else if (startswith("reboot", line_cli)){
		reboot();
	} else if (startswith("beep", line_cli)){
		beep();
	} else if (startswith("sleep", line_cli)){
		char temp[95];
		int i2 = 6;
		for (int i = 0; i < strlen(line_cli); i++){
			if (!isalnum(line_cli[i2])){
				break;
			}
			printf("\nc[%d] : %c\n", i2, line_cli[i2]);
			temp[i] = line_cli[i2];
			i2++;
		}
		int seconds = atoi(temp);
		sys_sleep(seconds);
	} else if (startswith("thirdtemple", line_cli)){
		printf("\nIf you search the third temple of god, you are in the wrong OS. \n Install TempleOS");
	} else {
		printf("\ncommand not found");
	}
	strcpy(last_line_cli, line_cli);
}

void terminal_framebuffer_setcolor(uint32_t col){
    old_color = color;
    color = col;
}

void terminal_framebuffer_reset_color(){
    color = old_color;
}