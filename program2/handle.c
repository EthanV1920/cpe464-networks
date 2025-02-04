/* Filename: handle.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file is responsible for handling
 *              arp header.
 */

#include "handle.h"
#include "safeUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct handle_table *create_table() {
  struct handle_table *tmp_table =
      (struct handle_table *)malloc(sizeof(struct handle_table));

  tmp_table->handle_table_entries = (HandleEntries*)malloc(2*sizeof(struct handle_table_entry));
  tmp_table->entry_count = 0;
  tmp_table->table_size = 2;

  return tmp_table;
};

int add_entry(uint16_t new_socket_number, char new_handle_name[100],
              struct handle_table *cur_handle_table) {

  // printf("INFO: adding entry: %s , %d\n", new_handle_name,
  // new_socket_number);
  if (get_socket(cur_handle_table, new_handle_name)) {
    return 2;
  }

  if (cur_handle_table->entry_count >= cur_handle_table->table_size) {
    uint16_t new_table_size = (cur_handle_table->table_size * 2 + 1);
    // uint16_t new_entry_count = cur_handle_table->entry_count;
    cur_handle_table->handle_table_entries =
        (struct handle_table_entry *)srealloc(
            cur_handle_table->handle_table_entries,
            sizeof(struct handle_table_entry) * new_table_size);
    cur_handle_table->table_size = new_table_size;
    // cur_handle_table->entry_count = new_entry_count;
    printf("INFO: resizing the array\n");
  }

  // NOTE: make sure to malloc when setting up
  struct handle_table_entry *tmp_entry =
      malloc(sizeof(struct handle_table_entry));

  tmp_entry->socket_number = new_socket_number;
  // printf("INFO: Socket Number is %d\n", tmp_entry->socket_number);
  memcpy(tmp_entry->handle_name, new_handle_name, 100);
  // printf("INFO: Handle Name is %s\n", tmp_entry->handle_name);

  cur_handle_table->handle_table_entries[cur_handle_table->entry_count] =
      *tmp_entry;
  cur_handle_table->entry_count++;

  return 0;
}

uint16_t get_socket(struct handle_table *cur_handle_table,
                    char handle_name[100]) {
  // Iterate thorough the list and find the socket corresponding to the handle
  // name
  for (int i = 0; i < cur_handle_table->entry_count; i++) {
    if (strcmp(cur_handle_table->handle_table_entries[i].handle_name,
               handle_name) == 0) {
      // printf("INFO: Found Match for %s\n", handle_name);
      return cur_handle_table->handle_table_entries[i].socket_number;
    }
  }
  return 0;
}

char *get_handle(struct handle_table *cur_handle_table,
                 uint16_t socket_number) {
  for (int i = 0; i < cur_handle_table->entry_count; i++) {
    if (cur_handle_table->handle_table_entries[i].socket_number ==
        socket_number) {
      // printf("INFO: Found Match for %hu\n", socket_number);
      return cur_handle_table->handle_table_entries[i].handle_name;
    }
  }
  return 0;
}
