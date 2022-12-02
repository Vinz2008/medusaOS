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
#include <kernel/pit.h>
#include <kernel/kmalloc.h>
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
char directory[100] = {'\0'};

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
	printf("\n");
	char* line_cli_copy = kmalloc(sizeof(char) * 300);
	memset(line_cli_copy, 0, sizeof(line_cli_copy));
	log(LOG_SERIAL, false,"line_cli_copy: %s\n", line_cli_copy);
	log(LOG_SERIAL, false,"line_cli: %s\n", line_cli);
	char* token;
	strcpy(line_cli_copy, line_cli);
	char* command = strtok(line_cli_copy, " ");
	if (command == NULL){
		command = kmalloc(strlen(line_cli_copy) * sizeof(char));
		strcpy(command, line_cli_copy);
	}
	log(LOG_SERIAL, false,"command: %s\n", command);
	//log(LOG_SERIAL, false,"command ptr: %p\n", command);
	char* args = kmalloc((strlen(line_cli) - strlen(command)) * sizeof(char));
	memset(args, 0, 300);
	int i2 = 0;
	log(LOG_SERIAL, false, "strlen(command) : %d\n", strlen(command));
	/*for (int i = 0; i < 300; i++){
		log(LOG_SERIAL, false, "before args[%d] : %c\n", i, args[i]);
	}*/
	for (int i = strlen(command) + 1; i < strlen(line_cli); i++){
		//log(LOG_SERIAL, false, "line_cli[%d] : %c\n", i, line_cli[i]);
		if (line_cli[i] != '\0'){
		args[i2] = line_cli[i];
		}
		//log(LOG_SERIAL, false, "args[%d] : %c\n", i2, args[i2]);
		i2++;
	}
	log(LOG_SERIAL, false, "AFTER END args[%d] : %c\n", i2, args[i2]);
	log(LOG_SERIAL, false,"args: %s\n", args);
	char *buf = strdup(args);
	char** argv = calloc(1, sizeof(char*));
	char *delim;
	int argc = 1;
	argv[0] = buf;
    while (delim = strchr(argv[argc - 1], ' ')) {
        argv = krealloc(argv, (argc + 1) * sizeof (char *));
        argv[argc] = delim + 1;
        *delim = 0x00;
        argc++;
    }
	for (int i = 0; i < argc; i++){
		log(LOG_SERIAL, false, "argv[%d] : %s\n", i, argv[i]);
	}
	
    if (strcmp("clear", command) == 0){
		terminal_framebuffer_clear();
	} else if (strcmp("echo", command) == 0){
		for (int i = 0; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("\n");
	} else if (strcmp("ls", command) == 0){
		fs_node_t* root = get_initrd_root();
		struct dirent* dir = NULL;
		int i = 0;
		bool print = false;
		while ((dir = readdir_fs(fs_root, i))!=NULL){
			if (strcmp(directory, "\0") != 0){
				if (startswith(directory, dir->name)){
					print = true;
				}
			} else {
        	print = true;
			}
			if (print){
			printf("filename [%i] : %s\n",i, dir->name);
			}
        	i++;
			print = false;
        }
	} else if (strcmp("cat", command) == 0){
		log(LOG_SERIAL, false, "test cat cmd\n");
		char*  filename = argv[0];
		char temp = kmalloc(200 * sizeof(char));
		memset(temp, 0, sizeof(temp));
		strcpy(temp, directory);
		strcat(temp, filename);
		fs_node_t* root = get_initrd_root();
		struct dirent* dir = NULL;
		int i = 0;
		log(LOG_SERIAL, false, "TEST\n");
		fs_node_t* file = vfs_open(temp, "r");
		log(LOG_SERIAL, false, "TEST2\n");
		if (file == NULL){
			printf("file not found: %s\n", temp);
		} else {
		uint8_t buffer[file->length];
        read_fs(file, 0, file->length, buffer);
		printf("%s\n", buffer);
		}
		close_fs(file);
		kfree(temp);
	} else if (strcmp("cd", command) == 0){
		char* dir = argv[0];
		log(LOG_SERIAL, false, "length dir %d\n",strlen(dir));
		char temp[100];
		char temp_directory[100];
		int i;
		if (strcmp(dir, "..") == 0){
			int pos = 0;
			log(LOG_SERIAL, false, "directory : %s\n", directory);
			log(LOG_SERIAL, false, "length directory %d\n",strlen(directory));
			for (i = strlen(temp_directory) - 2; i >= 0; i--){
				log(LOG_SERIAL, false, "directory %d : %c\n",i, directory[i]);
				if (directory[i] == '/'){
					log(LOG_SERIAL, false, "pos :  %d\n",i);
					pos = i;
					break;
				}
			}
			if (pos != 0){
			for (i = 0; i < pos; i++){
				log(LOG_SERIAL, false, "temp_directory %d : %c\n",i, temp_directory[i]);
				temp_directory[i] = directory[i];
			}
			}
			temp_directory[i+1] = '\0';
			printf("moved one directory up\n");
		} else {
		strcpy(temp_directory, directory);
		strcpy(temp, dir);
		log(LOG_SERIAL, false, "temp : %s\n", temp);
		log(LOG_SERIAL, false, "directory : %s\n", directory);
		if (strcmp(temp_directory, "\0") == 0){
			strcpy(temp_directory, temp);
		} else {
		strcat(temp_directory, temp);
		}
		strcat(temp_directory, "/");
		}
		log(LOG_SERIAL, false, "temp_directory : %s\n", temp_directory);
		strcpy(directory, temp_directory);		
	} else if (strcmp("pwd", command) == 0){
		printf("%s\n", directory);
	} else if (strcmp("chown", command) == 0){
		char* user = argv[0];
		char* group = NULL;
		char sep = ';';
		int pos = 0;
		char* temp = NULL;
		if (strchr(user, sep) != NULL){
			temp = strdup(user);
			for (int i = 0; i < strlen(temp); i++){
				if (temp[i] == sep){
					pos = i;
				}
			}
			log(LOG_SERIAL, false, "pos : %d\n", pos);
			group = temp + pos + 1;
			temp[pos] = '\0';
			char* user2 = temp;
			log(LOG_SERIAL, false, "user : %s, group : %s\n", user2, group);
		}
		char* file = argv[1];
		fs_node_t* node = vfs_open(file, "w");
		close_fs(file);
		if (temp != NULL){
			kfree(temp);
		}


	} else if (strcmp("help", command) == 0){
		printf("usage help : \n");
		printf("echo : print string\n");
		printf("ls : list directory and files\n");
		printf("cd : change directory\n");
		printf("pwd : print directory\n");
		printf("clear : clear screen\n");
		printf("reboot : reboot the computer\n");
		printf("arch : print arch of the computer\n");
		printf("thirdtemple : ...\n");
		printf("help : print this help\n");
	} else if (strcmp("arch", command) == 0){
		printf("i386\n");
	} else if (strcmp("reboot", command) == 0){
		reboot();
	} else if (strcmp("beep", command) == 0){
		pcspkr_beep(1000, 100);
		pcspkr_beep(1000, 10);
		pcspkr_beep(1000, 300);
	} else if (strcmp("sleep", command) == 0){
		int seconds = atoi(argv[0]);
		pit_mdelay(1000 * seconds);
	} else if (strcmp("thirdtemple", command) == 0){
		printf("If you search the third temple of god, you are in the wrong OS. \n Install TempleOS\n");
	} else {
		printf("command not found\n");
	}
	strcpy(last_line_cli, line_cli);
	empty_line_cli_framebuffer();
	kfree(command);
	kfree(args);
	kfree(argv);
	kfree(buf);
}

void terminal_framebuffer_setcolor(uint32_t col){
    old_color = color;
    color = col;
}

void terminal_framebuffer_reset_color(){
    color = old_color;
}