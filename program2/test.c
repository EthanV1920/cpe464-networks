#include "handle.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/* test01: Tests converting trivial tabs and spaces. */
void test01() {
  struct handle_table *tmp_handle_table = create_table();
  int8_t entry_count = 0;

  char str[] = "testHandle";
  uint16_t socket_number = 12345;
  assert(tmp_handle_table->entry_count == entry_count);
  assert(add_entry(socket_number, str, tmp_handle_table) == 0);
  entry_count++;
  assert(get_socket(tmp_handle_table, str) == socket_number);
  assert(tmp_handle_table->entry_count == entry_count);

  char str2[] = "testHandle2";
  uint16_t socket_number2 = 12346;
  assert(add_entry(socket_number2, str2, tmp_handle_table) == 0);
  entry_count++;
  assert(get_socket(tmp_handle_table, str2) == socket_number2);
  assert(tmp_handle_table->entry_count == entry_count);
  assert(strcmp(get_handle(tmp_handle_table, socket_number2), str2) == 0);
  assert(tmp_handle_table->entry_count == entry_count);

  assert(add_entry(socket_number2, str2, tmp_handle_table) == 2);
}

int main() {
  test01();
  printf("-------ALL TESTS PASSED-------\n");
  return 0;
}
