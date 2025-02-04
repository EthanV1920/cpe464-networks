/* Filename: handle.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file is responsible for handling
 *              arp header.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct handle_table_entry {
  uint16_t socket_number;
  char handle_name[100];
};

struct handle_table {
  struct handle_table_entry *handle_table_entries[2];
  uint16_t entry_count;
  uint16_t table_size;
};

struct handle_table *create_table();

int add_entry(uint16_t socket_number, char handle_name[100],
              struct handle_table *handle_table);

uint16_t get_socket(struct handle_table *cur_handle_table,
                    char handle_name[100]);

char* get_handle(struct handle_table *cur_handle_table,
                     uint16_t socket_number);
